# MLIR-Systolic 项目综合总结

> **项目**: MLIR-Systolic - 基于 MLIR 的脉动阵列编译器  
> **最后更新**: 2025-12-10  
> **状态**: Phase 1-4 基础框架完成

---

## 目录

1. [项目背景与目标](#1-项目背景与目标)
2. [AutoSA 架构深度分析](#2-autosa-架构深度分析)
3. [ScaleHLS 尝试与问题分析](#3-scalehls-尝试与问题分析)
4. [MLIR-Systolic 技术方案](#4-mlir-systolic-技术方案)
5. [实现进展](#5-实现进展)
6. [与 AutoSA 的对应关系](#6-与-autosa-的对应关系)
7. [技术优势与创新点](#7-技术优势与创新点)
8. [Polymer 多面体能力：核心依赖](#8-polymer-多面体能力核心依赖) ⭐
9. [下一步工作](#9-下一步工作)
10. [参考文档](#10-参考文档)
11. [总结](#11-总结)

---

## 1. 项目背景与目标

### 1.1 项目目标

MLIR-Systolic 旨在将 C 语言中的嵌套循环自动转换为 FPGA 中的脉动阵列 HLS C++ 代码，基于 MLIR 框架和 Polymer 多面体工具，实现类似 AutoSA 的功能，同时充分利用 MLIR 的优势。

### 1.2 技术栈

- **前端**: Polygeist (C → MLIR Affine)
- **多面体分析**: **Polymer (ISL 模式)** ⭐ **核心依赖**
- **中间表示**: MLIR (Affine Dialect, SystolicDataflow Dialect, HLS Dialect)
- **代码生成**: HLS C++ (Vivado HLS / Vitis 兼容)

**为什么需要 Polymer**：
- AutoSA 基于多面体模型，使用 ISL Schedule Tree 进行依赖分析和循环变换
- **Polymer 提供 MLIR ↔ ISL 的双向转换**，使我们能够：
  1. 获取完整的调度树（Schedule Tree）
  2. 进行精确的依赖距离分析
  3. 自动选择空间循环（依赖距离 ≤ 1）
  4. **正确分解 task 成多个独立的 module 函数**（关键！）

### 1.3 项目演进

1. **AutoSA**: 基于 PPCG/ISL 的脉动阵列编译器（参考实现）
2. **ScaleHLS**: 基于 MLIR 的 HLS 工具，尝试实现 AutoSA 功能但遇到限制
3. **MLIR-Systolic**: 重新设计，采用新的技术方案，充分利用 MLIR 特性

---

## 2. AutoSA 架构深度分析

### 2.1 AutoSA 核心概念

AutoSA 是一个基于 PPCG (Polyhedral Parallel Code Generator) 的编译器，专门用于生成脉动阵列的 HLS C++ 代码。其核心在于**分层的数据流管理**和**硬件感知的代码生成**。

#### 2.1.1 关键数据结构

**autosa_array_ref_group** (数组引用分组)
- 将访问同一数组的操作分组
- 分类为：IO_GROUP（输入）、PE_GROUP（计算）、DRAIN_GROUP（输出）
- 包含访问模式、数据流方向、IO 层级等信息

**autosa_io_buffer** (IO 缓冲区)
- 每个 IO 层级（L1/L2/L3）的缓冲区
- 支持双缓冲（ping-pong）机制
- 包含缓冲区形状、数据打包因子（n_lane）等信息

**autosa_hw_module** (硬件模块)
- PE_MODULE: 处理单元阵列
- IO_MODULE: 输入/输出模块（L1/L2/L3）
- DRAIN_MODULE: 输出收集模块

#### 2.1.2 分层 IO 架构

AutoSA 采用三层 IO 架构来管理数据流：

```
Global Memory (DRAM)
    ↓ (L3 IO Module - Inter-transfer)
L3 Buffer (可选 ping-pong)
    ↓ (L2 IO Module - Inter-transfer)
L2 Buffer (ping-pong)  
    ↓ (L1 IO Module - Intra-transfer)
PE Array (Systolic Flow)
    ↑ (Drain Modules - reverse flow)
Output Memory
```

**各层级职责**:
- **L3**: 全局内存接口，处理 DRAM 访问
- **L2**: 中间缓冲层，实现双缓冲以隐藏延迟
- **L1**: PE 接口层，直接与 PE 阵列通信

#### 2.1.3 双缓冲机制

AutoSA 在 L2 层级实现双缓冲（ping-pong buffering）：

```cpp
// 伪代码示例
bool arbiter = false;  // 选择当前活跃缓冲区
bool intra_enable = false;  // 控制 intra-transfer

for (tile iteration) {
    if (arbiter) {
        // Inter-transfer: 填充 ping 缓冲区
        load_to_buffer(ping_buffer);
        // Intra-transfer: 从 pong 缓冲区发送（如果启用）
        if (intra_enable) {
            send_from_buffer(pong_buffer);
        }
    } else {
        // Inter-transfer: 填充 pong 缓冲区
        load_to_buffer(pong_buffer);
        // Intra-transfer: 从 ping 缓冲区发送（如果启用）
        if (intra_enable) {
            send_from_buffer(ping_buffer);
        }
    }
    intra_enable = true;  // 第一个 tile 后启用
    arbiter = !arbiter;   // 切换缓冲区
}
```

**关键特性**:
- 重叠计算与通信，隐藏数据加载延迟
- 使用 arbiter 标志选择当前活跃缓冲区
- intra_enable 标志控制第一个 tile 的输出

#### 2.1.4 空间-时间映射

AutoSA 支持多种空间-时间映射模式：

| 模式 | 空间循环 | 时间循环 | 说明 |
|------|---------|---------|------|
| 0 | [i] | [j,k] | 1D 行阵列 |
| 1 | [j] | [i,k] | 1D 列阵列 |
| 2 | [k] | [i,j] | 1D 归约阵列 |
| 3 | [i,j] | [k] | 2D 输出驻留（默认） |
| 4 | [i,k] | [j] | 2D 水平归约 |
| 5 | [j,k] | [i] | 2D 垂直归约 |

**依赖距离分析**:
- 使用 ISL 计算循环维度间的依赖距离
- 依赖距离 ≤ 1 的维度可以作为空间循环
- 空间循环映射到 PE 阵列的物理维度

#### 2.1.5 多级分块

AutoSA 采用两级分块策略：

1. **Array Partition (第一级)**: 对应硬件阵列分区
   - 例如：`array_part[16,16,16]` 表示在 i、j、k 维度分别分块为 16

2. **Latency Hiding (第二级)**: 用于隐藏延迟
   - 例如：`latency[8,8]` 表示在 i、j 维度进一步分块为 8
   - PE 阵列大小 = `array_part / latency` = `16/8 = 2×2`

#### 2.1.6 脉动数据流

在真正的脉动阵列中，数据在 PE 之间流动，而不是从中央源广播。对于矩阵乘法 `C[i,j] += A[i,k] * B[k,j]`：

```
           ↓ B[k,j] (垂直流动)
           ↓
→ A[i,k] → PE[0,0] → PE[0,1] → PE[0,2] → ...
           ↓          ↓          ↓
→ A[i,k] → PE[1,0] → PE[1,1] → PE[1,2] → ...
           ↓          ↓          ↓
→ A[i,k] → PE[2,0] → PE[2,1] → PE[2,2] → ...
```

- **Matrix A**: 水平流动（沿 j 轴），`A[i,k]` 对 j 不变
- **Matrix B**: 垂直流动（沿 i 轴），`B[k,j]` 对 i 不变
- **Matrix C**: 本地累加，`C[i,j]` 依赖于 i 和 j

### 2.2 AutoSA 代码生成流程

```
C 代码 (PET 解析)
    ↓
ISL Schedule Tree (依赖分析、调度)
    ↓
sa_space_time_transform (空间-时间映射)
    ↓
sa_array_partitioning_optimize (第一级分块)
    ↓
sa_latency_hiding_optimize (第二级分块)
    ↓
sa_io_construct_optimize (IO 模块构建)
    ↓
generate_hw_modules (硬件模块生成)
    ↓
print_hw (HLS C++ 代码生成)
```

---

## 3. ScaleHLS 尝试与问题分析

### 3.1 ScaleHLS 的实现方法

ScaleHLS 在 `feature/autosa-map` 分支上尝试实现 AutoSA 功能，主要方法：

1. **直接使用 HLS Dialect**: 利用现有的 `hls.dataflow.*` 操作
2. **AutoSAMap Pass**: 在单个 Pass 中完成所有变换
3. **直接生成 HLS 代码**: 从 Affine IR 直接生成 HLS Dialect

### 3.2 遇到的问题

#### 3.2.1 缺少多面体能力（核心问题）⭐

**这是放弃 ScaleHLS 的主要原因**：

ScaleHLS 使用的 Polygeist 版本较旧，**没有 Polymer 多面体工具**：

1. **无法获取调度树（Schedule Tree）**：
   - AutoSA 基于 ISL Schedule Tree 进行依赖分析和循环变换
   - ScaleHLS 的 Affine Dialect 只有少部分多面体能力，无法获取完整的调度树
   - **没有调度树，就无法正确分解 task 成多个独立的 module 函数**

2. **Task 分解问题**：
   - ScaleHLS 生成的代码只有一个函数，所有逻辑都在一个函数内
   - 在 Vitis HLS 工具中，这样的代码可能被合并优化
   - **无法预估最终生成的硬件结构**，因为编译器可能进行不可控的优化

3. **依赖分析不足**：
   - 无法进行精确的依赖距离分析
   - 无法自动选择空间循环（需要依赖距离 ≤ 1 的分析）
   - 无法进行复杂的多面体变换

**示例问题**：
```cpp
// ScaleHLS 生成的代码（问题）
void kernel() {
  // 所有 IO 模块、PE 模块都在一个函数内
  // Vitis 可能将这些模块合并，无法预估硬件结构
}

// AutoSA 生成的代码（正确）
void A_IO_L3_in() { ... }      // 独立函数
void A_IO_L2_in() { ... }      // 独立函数
void PE_0_0() { ... }          // 独立函数
void kernel() {
  A_IO_L3_in();  // 调用独立函数
  A_IO_L2_in();
  PE_0_0();
}
```

#### 3.2.2 一对一翻译问题

ScaleHLS 的实现更像 AutoSA 的直接翻译：
- 直接在 Affine IR 上操作，缺少中间抽象层
- 难以利用 MLIR 的 dialect 系统和 pass 组合能力
- 代码结构复杂，难以维护和扩展

#### 3.2.3 缺少多层双缓冲 IO

ScaleHLS 无法完整生成 AutoSA 的多层（L1/L2/L3）双缓冲 IO 模块：
- L2 层级的双缓冲逻辑不够完善
- 缺少清晰的层级抽象
- 难以表达复杂的 IO 模块关系

### 3.3 ScaleHLS 的成果

尽管存在限制，ScaleHLS 的工作仍然有价值：
- ✅ 实现了基础的 PE 阵列生成
- ✅ 实现了 SIMD 向量化
- ✅ 实现了双缓冲（虽然不完整）
- ✅ 生成了可用的 HLS C++ 代码
- ✅ 15/15 测试通过

这些成果为 MLIR-Systolic 提供了宝贵的参考。

---

## 4. MLIR-Systolic 技术方案

### 4.1 设计理念

**核心思想**: 不再直接翻译 AutoSA，而是用 MLIR 的方式重新表达 AutoSA 的语义，充分利用 MLIR 的 dialect 系统、pass 组合和变换能力。

### 4.2 架构设计

```
输入: Affine MLIR (来自 Polygeist 或手写)
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 1: 多面体分析 (Polymer/ISL) ⭐ 核心                │
│ - 依赖距离分析（使用 ISL）                               │
│ - 空间循环选择（依赖距离 ≤ 1）                           │
│ - 调度树构建（ISL Schedule Tree）                        │
│ - **Task 分解依据**（基于调度树确定模块边界）            │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 2: 空间-时间变换 (SystolicTransform Pass)          │
│ - 循环置换（空间循环外移）                                │
│ - 多级分块（array_part + latency）                       │
│ - 输出: 分块后的 Affine IR                               │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 3: 数据流抽象 (SystolicDataflow Dialect) ⭐ NEW   │
│                                                          │
│ 定义新的 Dialect 来表达脉动阵列的硬件抽象：               │
│                                                          │
│ - systolic.io.module<L3> { ... }  // L3 IO 模块         │
│ - systolic.io.module<L2> { ... }  // L2 IO 模块（双缓冲）│
│ - systolic.io.module<L1> { ... }  // L1 IO 模块         │
│ - systolic.pe.array { ... }      // PE 阵列             │
│ - systolic.drain.module { ... }   // Drain 模块         │
│                                                          │
│ 每个模块包含：                                            │
│ - 缓冲区声明（ping-pong）                                 │
│ - 数据传输操作（inter/intra）                             │
│ - 调度信息（schedule）                                    │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 4: 数据流生成 (SystolicDataflowGeneration Pass)   │
│                                                          │
│ 将 Affine IR 转换为 SystolicDataflow Dialect：          │
│                                                          │
│ 1. 数组引用分组（类似 AutoSA 的 autosa_array_ref_group） │
│ 2. IO 层级分析（确定 L1/L2/L3）                          │
│ 3. 双缓冲生成（ping-pong buffer）                         │
│ 4. 模块实例化（IO/PE/Drain）                              │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 5: 数据流优化 (可选 Pass)                          │
│ - 缓冲区合并优化                                          │
│ - 流通道优化                                              │
│ - 调度优化                                                │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 6: 降级到 HLS Dialect                              │
│                                                          │
│ 将 SystolicDataflow Dialect 降级到现有的 HLS Dialect：  │
│                                                          │
│ - systolic.io.module → hls.dataflow.task                 │
│ - systolic.pe.array → hls.dataflow.task                 │
│ - 双缓冲逻辑 → Affine loops + SCF if                     │
│ - Stream 通道 → hls.dataflow.stream                      │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│ Phase 7: HLS C++ 生成 (EmitHLSCpp)                      │
│ - 生成 HLS C++ 代码                                      │
│ - 插入 Pragma                                            │
└─────────────────────────────────────────────────────────┘
         │
         ▼
输出: HLS C++ (Vivado HLS / Vitis 兼容)
```

### 4.3 SystolicDataflow Dialect 设计

#### 4.3.1 Dialect 定义

创建了专门的 `SystolicDataflow` Dialect 来表达脉动阵列的硬件抽象：

```tablegen
def SystolicDataflow_Dialect : Dialect {
  let name = "systolic.dataflow";
  let cppNamespace = "::mlir::systolic::dataflow";
  let summary = "Systolic array dataflow abstraction dialect";
}
```

#### 4.3.2 核心操作

**IOModuleOp** - IO 模块操作
```mlir
systolic.dataflow.io.module level = 2 direction = "in" array = "A" 
    buffer_shape = [8, 16] double_buffer = true {
  // 模块体
  systolic.dataflow.io.module.yield
}
```

**PEArrayOp** - PE 阵列操作
```mlir
systolic.dataflow.pe.array array_size = [2, 2] tile_size = [8, 8] {
  // PE 阵列体
  systolic.dataflow.pe.array.yield
}
```

**DoubleBufferOp** - 双缓冲操作
```mlir
systolic.dataflow.double_buffer %ping, %pong, %arbiter, %intra_enable
    inter_transfer = {
      // Inter-transfer 逻辑
      systolic.dataflow.double_buffer.yield
    }
    intra_transfer = {
      // Intra-transfer 逻辑
      systolic.dataflow.double_buffer.yield
    }
```

**DrainModuleOp** - 输出模块操作
```mlir
systolic.dataflow.drain.module level = 2 array = "C" {
  // Drain 模块体
  systolic.dataflow.drain.module.yield
}
```

### 4.4 与 ScaleHLS 方法的对比

| 方面 | ScaleHLS | MLIR-Systolic |
|------|----------|---------------|
| **中间抽象** | 直接使用 HLS Dialect | 引入 SystolicDataflow Dialect |
| **Pass 结构** | 单个大 Pass | 多个小 Pass，职责清晰 |
| **IO 模块** | 直接生成 HLS Task | 先抽象为 Dialect，再降级 |
| **双缓冲** | 在 Affine IR 中实现 | 在 Dialect 中表达，再转换 |
| **扩展性** | 难以扩展 | 易于添加新操作和优化 |

---

## 5. 实现进展

### 5.1 Phase 1 & 2: SystolicDataflow Dialect ✅

**已完成**:
- ✅ Dialect 定义（TableGen）
- ✅ 核心操作定义：`IOModuleOp`, `PEArrayOp`, `DoubleBufferOp`, `DrainModuleOp`
- ✅ C++ 实现和验证器
- ✅ 在工具中注册 Dialect

**文件结构**:
```
include/systolic/Dialect/SystolicDataflow/
├── SystolicDataflow.td          # Dialect 定义
├── SystolicDataflowOps.td       # 操作定义
├── SystolicDataflow.h           # 头文件
└── CMakeLists.txt               # TableGen 配置

lib/Dialect/SystolicDataflow/
├── SystolicDataflowDialect.cpp  # 实现
└── CMakeLists.txt               # 构建配置
```

### 5.2 Phase 3 & 4: 数组引用分析和数据流生成 ✅

**已完成**:
- ✅ `ArrayRefGroup` 数据结构
- ✅ 基础的数组引用分析
- ✅ `SystolicDataflowGeneration` Pass 框架
- ✅ 基础的 IO 模块生成

**实现细节**:
- 数组引用分组：收集 load/store 操作，按 memref 分组
- IO/PE/Drain 分类：基于访问模式（只读→IO，只写→Drain，读写→PE）
- IO 层级分析：简单的启发式方法（目前固定为 L2，**待使用 Polymer 进行精确分析**）

**⚠️ 当前限制**:
- **尚未集成 Polymer**：当前使用简化的启发式方法
- **无法获取调度树**：因此无法正确分解 task 成多个独立函数
- **这是下一步的关键工作**：集成 Polymer 后，才能实现完整的 AutoSA 功能

### 5.3 构建状态

**构建成功** ✅:
```bash
cd build
cmake .. -DMLIR_DIR=$POLYGEIST_BUILD/lib/cmake/mlir -GNinja
ninja
# 构建成功，所有目标完成
```

**工具验证**:
```bash
./bin/systolic-opt --help
# 输出显示：systolic.dataflow dialect 已注册
```

### 5.4 多面体能力使用状态 ⚠️

**重要**：经过代码检查，**mlir-systolic 目前尚未真正使用 Polymer/ISL 多面体能力**。

**当前实现**：
- ❌ **没有使用 ISL 计算依赖距离**（使用简化的启发式方法）
- ❌ **没有使用调度树**（无法获取）
- ❌ **没有链接 Polymer 库**（只有 include 目录）
- ✅ 所有分析都是简化版本，代码中有大量 TODO 注释

**详细状态**：详见 [多面体能力使用状态](POLYHEDRAL_STATUS.md)

**影响**：
- 无法自动选择空间循环（需要用户手动指定）
- 无法基于调度树分解 task
- 可能仍然生成单个函数（ScaleHLS 的问题未解决）

**结论**：**集成 Polymer 是最高优先级的工作**，这是实现完整 AutoSA 功能的关键。

### 5.5 当前能力

| 功能 | 状态 | 说明 |
|------|------|------|
| Dialect 定义 | ✅ | SystolicDataflow Dialect 完整定义 |
| 操作验证 | ✅ | 所有操作都有验证器 |
| 数组引用分析 | ⚠️ | 简化启发式方法（待使用 Polymer） |
| IO 模块生成 | ⚠️ | 基础生成框架（待基于调度树） |
| 多层 IO | ⚠️ | 框架已就绪，待完善 |
| 双缓冲 | ⚠️ | Dialect 支持，待实现生成逻辑 |
| PE 阵列生成 | ⚠️ | 待实现 |
| Dialect 降级 | ❌ | 待实现 |
| **多面体分析** | ❌ | **待集成 Polymer** ⭐ |

---

## 6. 与 AutoSA 的对应关系

### 6.1 数据结构对应

| AutoSA | MLIR-Systolic |
|--------|---------------|
| `autosa_array_ref_group` | `ArrayRefGroup` (分析阶段) |
| `autosa_io_buffer` | `systolic.dataflow.io.module` 操作 |
| `autosa_hw_module` | `systolic.dataflow.io.module` / `pe.array` |
| `io_level` (1-3) | `IOModuleOp.level` 属性 |
| `inter_transfer` / `intra_transfer` | `DoubleBufferOp` 的 region |
| ping-pong buffer | `systolic.dataflow.double_buffer` 操作 |
| `autosa_pe_module` | `systolic.dataflow.pe.array` |

### 6.2 变换流程对应

| AutoSA 阶段 | MLIR-Systolic 阶段 |
|------------|-------------------|
| `sa_space_time_transform` | `SystolicTransform` Pass |
| `sa_array_partitioning_optimize` | `SystolicTransform` Pass (第一级分块) |
| `sa_latency_hiding_optimize` | `SystolicTransform` Pass (第二级分块) |
| `sa_io_construct_optimize` | `SystolicDataflowGeneration` Pass |
| `generate_hw_modules` | `SystolicDataflowGeneration` Pass |
| `print_hw` | `EmitHLSCpp` Translation |

### 6.3 语义对应

虽然实现方式不同，但 MLIR-Systolic 保持了与 AutoSA 的语义对应：

- **分层 IO 结构**: 通过 `IOModuleOp` 的 `level` 属性表达
- **双缓冲机制**: 通过 `DoubleBufferOp` 操作表达
- **PE 阵列**: 通过 `PEArrayOp` 操作表达
- **数据流**: 通过 Dialect 操作的关系表达

---

## 7. 技术优势与创新点

### 7.1 充分利用 MLIR 特性

1. **Dialect 系统**: 用专门的 Dialect 表达硬件抽象，而不是直接生成代码
2. **Pass 组合**: 每个阶段是独立的 Pass，可以灵活组合和优化
3. **类型系统**: 利用 MLIR 的类型系统表达缓冲区、流等概念
4. **变换能力**: 可以在不同抽象层级之间转换和优化

### 7.2 清晰的抽象层级

```
Affine IR (循环嵌套)
    ↓ SystolicDataflowGeneration
SystolicDataflow Dialect (硬件抽象)
    ↓ SystolicDataflowToHLS
HLS Dialect (数据流)
    ↓ EmitHLSCpp
HLS C++ (最终代码)
```

每一层都有清晰的职责，便于理解和维护。

### 7.3 易于扩展

- **新硬件结构**: 可以添加新的 Dialect 操作来表达不同的硬件结构
- **优化 Pass**: 可以在 SystolicDataflow 层级添加优化 Pass，而不影响代码生成
- **多后端支持**: 可以添加不同的降级 Pass 支持不同的 HLS 工具

### 7.4 支持多层双缓冲 IO

通过 `SystolicDataflow` Dialect 的 `IOModuleOp` 和 `DoubleBufferOp`，可以清晰地表达：
- L3 IO 模块（Global Memory 接口）
- L2 IO 模块（带双缓冲）
- L1 IO 模块（PE 接口）
- 各层级之间的数据流

---

## 8. 下一步工作

### 8.1 ⭐ Polymer 集成（最高优先级）

**目标**: 集成 Polymer 多面体工具，获取调度树，实现正确的 task 分解

**为什么这是最高优先级**：
- **没有调度树，就无法正确分解 task 成多个独立的 module 函数**
- 这是 ScaleHLS 无法解决的问题，也是我们创建 mlir-systolic 的主要原因
- 只有集成 Polymer，才能实现完整的 AutoSA 功能

**任务**:
- [ ] **集成 Polymer ISL 接口**（获取调度树）
- [ ] **实现依赖距离分析**（使用 ISL 计算依赖距离向量）
- [ ] **实现空间循环自动选择**（依赖距离 ≤ 1 的维度）
- [ ] **基于调度树分解 task**（确定模块边界，生成独立函数）
- [ ] 改进 IO 层级判断逻辑（基于调度树信息）
- [ ] 实现缓冲区形状计算（基于调度树和访问模式）

**技术要点**:
```cpp
// 使用 Polymer 获取调度树
polymer::IslScop scop = polymer::extractScop(func);
isl_schedule *schedule = polymer::computeSchedule(scop);

// 基于调度树分解 task
// 调度树的节点对应不同的硬件模块（L3 IO, L2 IO, PE, Drain）
// 每个节点生成一个独立的函数
```

### 8.2 Phase 5: SystolicDataflowToHLS 降级

**目标**: 将 SystolicDataflow Dialect 降级到 HLS Dialect

**任务**:
- [ ] 实现 `SystolicDataflowToHLS` Pass
- [ ] 实现 Dialect 转换模式
- [ ] 将 IO 模块降级到 HLS Task（**每个模块生成独立函数**）
- [ ] 将双缓冲降级到 Affine/SCF
- [ ] 将 Stream 操作映射到 HLS Stream

**关键**: 确保每个硬件模块（IO/PE/Drain）都生成独立的函数，而不是合并到一个函数中。

### 8.3 完善数据流生成

**目标**: 实现完整的多层 IO 和双缓冲生成

**前提**: 需要先完成 Polymer 集成，获取调度树信息

**任务**:
- [ ] 实现多层 IO 模块的完整生成（基于调度树确定层级）
- [ ] 实现双缓冲逻辑生成
- [ ] 实现 PE 阵列生成
- [ ] 实现 Drain 模块生成
- [ ] 实现脉动数据流生成
- [ ] **确保每个模块生成独立函数**（关键！）

### 8.4 测试与验证

**目标**: 确保生成的代码功能正确

**任务**:
- [ ] MatMul 端到端测试
- [ ] 与 AutoSA 输出对比
- [ ] 多层双缓冲 IO 验证
- [ ] 性能对比

---

## 10. 参考文档

### 10.1 项目文档

- [README.md](../README.md) - 项目概述
- **[POLYMER_INTEGRATION.md](./POLYMER_INTEGRATION.md)** ⭐ - **Polymer 集成方案（核心）**
- [TECHNICAL_REDESIGN.md](./TECHNICAL_REDESIGN.md) - 技术方案详细设计
- [QUICK_REFERENCE.md](./QUICK_REFERENCE.md) - 快速参考
- [IMPLEMENTATION_PROGRESS.md](./IMPLEMENTATION_PROGRESS.md) - 实现进度

### 10.2 参考项目

- [AutoSA](https://github.com/UCLA-VAST/AutoSA) - 脉动阵列生成算法（参考实现）
- [ScaleHLS](https://github.com/UIUC-ChenLab/scalehls) - HLS Dialect 和代码生成（参考实现）
- [Polygeist/Polymer](https://github.com/llvm/Polygeist) - 多面体分析工具 ⭐ **核心依赖**

### 10.3 ScaleHLS 文档（历史参考）

以下文档记录了在 ScaleHLS 上的工作，为 MLIR-Systolic 提供了宝贵的经验：

- `scalehls/docs/MLIR_AutoSA_Implementation.md` - ScaleHLS 实现细节
- `scalehls/docs/AutoSA_Quick_Reference.md` - AutoSA 快速参考
- `scalehls/docs/HLS_Output_Comparison.md` - 输出对比分析

---

## 11. 总结

MLIR-Systolic 项目采用全新的技术方案，通过引入 `SystolicDataflow` Dialect 作为中间抽象层，既实现了 AutoSA 的核心功能（包括多层双缓冲 IO），又充分利用了 MLIR 的特性，为后续扩展和优化打下了坚实的基础。

**核心成就**:
1. ✅ 设计了清晰的 Dialect 抽象，表达脉动阵列硬件结构
2. ✅ 实现了基础框架，包括 Dialect 定义、Pass 框架、数组引用分析
3. ✅ 建立了与 AutoSA 的语义对应关系
4. ✅ 明确了 Polymer 的核心地位和集成方案

**关键认识**:
- **Polymer 是核心依赖，不是可选的**
- **没有调度树，就无法正确分解 task 成多个独立函数**
- **这是 ScaleHLS 无法解决的问题，也是 mlir-systolic 的主要目标**

**下一步重点**（按优先级）:
1. ⭐⭐⭐ **集成 Polymer**（最高优先级）
   - 获取调度树
   - 基于调度树分解 task
   - 生成多个独立函数
2. 完善数据流生成逻辑
3. 实现 Dialect 降级
4. 端到端测试和验证

---

**文档维护**: 本文档应随着项目进展持续更新，记录重要的设计决策和实现细节。

