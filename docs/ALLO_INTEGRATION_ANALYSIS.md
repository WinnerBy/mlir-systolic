# Allo 集成分析报告

> **最后更新**: 2025-01-XX  
> **目的**: 分析 allo 项目，评估与 mlir-systolic 的集成方案  
> **关键结论**: 先完成 mlir-systolic 核心功能（Affine → 自动脉动阵列），再复用 Allo 的 Affine 处理流程进行衔接

---

## 目录

1. [Allo 项目概述](#1-allo-项目概述)
2. [Allo 架构分析](#2-allo-架构分析)
3. [Allo vs mlir-systolic 对比](#3-allo-vs-mlir-systolic-对比)
4. [集成方案设计](#4-集成方案设计)
5. [实施路线图](#5-实施路线图)

---

## 1. Allo 项目概述

### 1.1 核心特点

Allo 是一个 **Python-embedded, MLIR-based** 的加速器设计和编程语言，主要特点：

- **计算与调度分离**：类似 TVM，用户可以在 Python 层面编写 schedule 来描述硬件结构
- **PyTorch 模型支持**：支持从 PyTorch 模型自动转换为 FPGA HLS C 代码
- **脉动阵列支持**：提供 `systolic` 库函数，但需要手动编写 schedule 来描述脉动阵列
- **多后端支持**：支持 Vivado HLS、Vitis HLS、TAPA、Intel HLS 等

### 1.2 关键组件

#### 1.2.1 PyTorch 前端 (`allo/frontend/pytorch.py`)

**功能**：
- 使用 `torch.fx` 追踪 PyTorch 模型
- 将 PyTorch 操作映射到 Allo DSL 操作
- 生成 Allo Python 代码（字符串形式）

**关键流程**：
```python
from_pytorch(model, example_inputs) 
  → TorchBuilder.build() 
  → 生成 Allo Python 代码字符串
  → customize(code, global_vars) 
  → Schedule 对象
```

**示例**：
```python
# PyTorch 模型
class Linear(nn.Module):
    def forward(self, x, w):
        return F.linear(x, w)

# Allo 生成的代码（字符串）
def forward(x: float32[L, D], w: float32[D, 4*D]):
    return dsl.linear(x, w)
```

#### 1.2.2 Schedule 系统 (`allo/customize.py`, `allo/ir/transform.py`)

**核心概念**：
- **计算描述**：用户编写 Python 函数描述计算逻辑
- **调度描述**：通过 `s.customize()` 和 schedule API 描述硬件结构

**Schedule API**：
```python
s = allo.customize(func)           # 创建 schedule
s.partition(array, dim=0)          # 数组分区
s.pipeline(loop)                    # 流水线化
s.unfold(loop, [0, 1])             # 展开循环（空间循环）
s.to(buffer, pe, axis=1, depth=5)  # 创建 FIFO 通道
```

#### 1.2.3 脉动阵列库 (`allo/library/systolic.py`)

**提供的函数**：
- `PE_kernel`: PE 核心计算逻辑
- `systolic_tile`: 单个脉动阵列 tile
- `systolic`: 完整的脉动阵列（包含分块）
- `packed_systolic`: 支持数据打包的脉动阵列

**关键特点**：
- **手动编写**：用户需要手动调用 `systolic` 函数并编写 schedule
- **FIFO 管理**：需要手动指定 FIFO 深度和方向
- **无自动分析**：不进行依赖距离分析，不自动选择空间循环

**示例**：
```python
from allo.library.systolic import systolic

def forward(X, W, Y):
    systolic[int8, int8, int8, L, D, 4*D, M0, M1](X, W, Y)

s = allo.customize(forward)
# 必须手动编写 schedule
s.partition(s.local_C, dim=0)
s.partition(s.local_A, dim=1)
s.partition(s.local_B, dim=2)
pe = s.unfold("systolic_tile:PE", [0, 1])  # 手动指定空间循环
s.to(s.A_fifo, pe, axis=1, depth=M0+1)
s.to(s.B_fifo, pe, axis=0, depth=M1+1)
```

#### 1.2.4 MLIR Lowering Pipeline (`allo/passes.py`, `allo/backend/hls.py`)

**关键发现**：Allo **不降低 Affine Dialect**，直接保留 Affine 操作！

**流程**：
```
Allo Python 代码 
  → Allo IR (MLIR, 包含 Allo Dialect)
  → Schedule Transform (在 Allo IR 上应用变换)
  → MLIR Lowering Passes:
     - convert-linalg-to-affine-loops (Linalg → Affine)
     - empty-tensor-to-alloc-tensor
     - **注意：不降低 Affine Dialect！**
  → Allo IR (包含 Affine 操作)
  → 直接生成 HLS C++ (emit_vhls/emit_thls/emit_ihls)
```

**关键代码**（`allo/backend/hls.py:214-227`）：
```python
_mlir_lower_pipeline(self.module, lower_linalg=True)
pm = PassManager.parse(
    "builtin.module("
    "empty-tensor-to-alloc-tensor,"
    "func.func(convert-linalg-to-affine-loops)"
    # DO NOT LOWER AFFINE DIALECT  ← 关键！
    ")"
)
pm.run(self.module.operation)
```

**特点**：
- **直接代码生成**：从 Affine MLIR 直接生成 HLS C++，没有中间的 HLS Dialect
- **代码生成器**：`EmitVivadoHLS.cpp` 直接遍历 `AffineForOp`, `AffineLoadOp`, `AffineStoreOp` 等操作
- **逐句翻译**：每个 Affine 操作对应一段 C++ 代码
- **Pragma 插入**：根据操作属性（如 `pipeline_ii`）插入 HLS pragma

---

## 2. Allo 架构分析

### 2.1 编译流程（详细）

```
PyTorch 模型
    ↓
[PyTorch FX 追踪] (allo/frontend/pytorch.py)
    ↓
Allo Python 代码（字符串）
    ↓
[Allo Parser] → Allo IR (MLIR, 包含 Allo Dialect)
    ↓
[Schedule Transform] (allo/customize.py, allo/ir/transform.py)
    - partition, pipeline, unfold, to 等操作
    - 直接在 Allo IR 上应用变换
    ↓
[MLIR Lowering Passes] (allo/passes.py:45-60)
    - convert-linalg-to-affine-loops (Linalg → Affine)
    - empty-tensor-to-alloc-tensor
    - **关键：不降低 Affine Dialect！**
    ↓
Allo IR (包含 Affine 操作)
    ↓
[直接代码生成] (allo/mlir/lib/Translation/EmitVivadoHLS.cpp)
    - 遍历 AffineForOp, AffineLoadOp, AffineStoreOp 等
    - 根据属性插入 HLS pragma
    - 直接生成 HLS C++ 代码
    ↓
HLS C++ 代码
```

**关键点**：
1. **没有 HLS Dialect**：Allo 直接从 Affine MLIR 生成 C++，不经过 HLS Dialect
2. **代码生成器**：`EmitVivadoHLS.cpp` 是一个大型的 visitor 模式实现，直接处理各种 MLIR 操作
3. **Pragma 插入**：根据操作属性（如 `pipeline_ii`, `rewind`）在生成代码时插入 pragma

### 2.2 与 ScaleHLS 的关系

Allo 可以看作是 **更高级的 ScaleHLS**：

| 特性 | ScaleHLS | Allo |
|------|----------|------|
| **前端** | C/C++ → MLIR | PyTorch/Python → MLIR |
| **调度** | MLIR Pass | Python Schedule API |
| **抽象层级** | 低（接近 MLIR） | 高（Python DSL） |
| **易用性** | 需要理解 MLIR | Python 友好 |
| **性能** | 较好 | 更好（更灵活的调度） |

### 2.3 脉动阵列实现方式

**Allo 的脉动阵列实现**：
1. **库函数**：提供预定义的 `systolic` 函数
2. **手动调度**：用户必须手动编写 schedule 来：
   - 指定空间循环（`unfold`）
   - 创建 FIFO 通道（`to`）
   - 设置 FIFO 深度
3. **无自动分析**：不进行依赖距离分析，不自动选择空间循环

**示例**（来自 `allo/tests/test_systolic_array.py`）：
```python
def systolic_array(A: int8[M, K], B: int8[K, N], C: int16[M, N]):
    A_fifo: int8[M, N + 1, K]
    B_fifo: int8[N, M + 1, K]
    # ... 数据加载和 PE 计算 ...

s = allo.customize(systolic_array)
s.partition(s.C, dim=0)
s.partition(s.A, dim=1)
s.partition(s.B, dim=2)
pe = s.unfold("PE", [0, 1])  # 手动指定 [0, 1] 为空间循环
s.to(s.A_fifo, pe, axis=1, depth=M + 1)
s.to(s.B_fifo, pe, axis=0, depth=N + 1)
```

---

## 3. Allo vs mlir-systolic 对比

### 3.1 功能对比

| 功能 | Allo | mlir-systolic |
|------|------|---------------|
| **PyTorch 支持** | ✅ 完整支持 | ❌ 不支持 |
| **脉动阵列自动生成** | ❌ 需要手动 schedule | ✅ 自动分析+生成 |
| **依赖距离分析** | ❌ 无 | ✅ 使用 Polymer/ISL |
| **空间循环选择** | ❌ 手动指定 | ✅ 自动选择（依赖距离≤1） |
| **IO 模块生成** | ⚠️ 手动编写 | ✅ 自动生成（L1/L2/L3） |
| **双缓冲** | ⚠️ 手动管理 | ✅ 自动生成 |
| **HLS 代码质量** | 较好（依赖 schedule） | 目标：与 AutoSA 相当 |

### 3.2 优势互补

**Allo 的优势**：
1. ✅ **PyTorch 模型支持**：完整的 PyTorch → FPGA 流程
2. ✅ **Python 友好**：易于使用和调试
3. ✅ **灵活的调度**：可以精确控制硬件结构
4. ✅ **多后端支持**：Vivado/Vitis/TAPA/Intel

**mlir-systolic 的优势**：
1. ✅ **自动脉动阵列生成**：基于多面体分析的自动优化
2. ✅ **依赖距离分析**：使用 Polymer/ISL 进行精确分析
3. ✅ **自动 IO 模块生成**：多层双缓冲结构
4. ✅ **与 AutoSA 对齐**：参考 AutoSA 的成熟算法

### 3.3 集成价值

**理想方案**：
- **前端**：使用 Allo 的 PyTorch 支持
- **核心优化**：使用 mlir-systolic 的自动脉动阵列生成
- **代码生成**：可以复用 Allo 的 HLS 代码生成（或使用 mlir-systolic 的）

---

## 4. 集成方案设计

### 4.1 总体架构

```
┌─────────────────────────────────────────────────────────┐
│  PyTorch 模型                                            │
└─────────────────────────────────────────────────────────┘
         │
         ▼
┌─────────────────────────────────────────────────────────┐
│  Allo PyTorch Frontend (复用)                            │
│  - torch.fx 追踪                                        │
│  - 生成 Allo Python 代码                                │
└─────────────────────────────────────────────────────────┘
         │
         ▼ Allo Python 代码（字符串）
┌─────────────────────────────────────────────────────────┐
│  方案 A: Allo Parser → Allo IR                           │
│  方案 B: 直接转换为 Affine MLIR（跳过 Allo IR）          │
└─────────────────────────────────────────────────────────┘
         │
         ▼ Affine MLIR
┌─────────────────────────────────────────────────────────┐
│  mlir-systolic Pipeline (核心)                           │
│  - SystolicTransform (依赖分析+分块)                   │
│  - SystolicDataflowGeneration (IO 模块生成)             │
│  - SystolicDataflowToHLS (降级)                          │
└─────────────────────────────────────────────────────────┘
         │
         ▼ HLS Dialect
┌─────────────────────────────────────────────────────────┐
│  代码生成（二选一）                                      │
│  - 方案 A: Allo 的 HLS 代码生成                          │
│  - 方案 B: mlir-systolic 的 EmitHLSCpp                  │
└─────────────────────────────────────────────────────────┘
         │
         ▼
    HLS C++ 代码
```

### 4.2 方案 A：完整集成 Allo

**流程**：
1. 使用 Allo 的 PyTorch 前端生成 Allo Python 代码
2. 使用 Allo Parser 转换为 Allo IR (MLIR)
3. **关键步骤**：将 Allo IR 中的关键循环嵌套转换为 Affine MLIR
4. 应用 mlir-systolic 的脉动阵列变换
5. 降级回 Allo IR 或 HLS Dialect
6. 使用 Allo 的代码生成器生成 HLS C++

**优点**：
- ✅ 复用 Allo 的完整工具链
- ✅ 保持 Allo 的 Python API 兼容性
- ✅ 可以利用 Allo 的其他优化（非脉动阵列部分）

**缺点**：
- ⚠️ 需要处理 Allo IR → Affine MLIR 的转换
- ⚠️ 需要处理 Affine MLIR → Allo IR 的回转换
- ⚠️ 可能引入额外的抽象层开销

### 4.3 方案 B：轻量级集成（推荐）

**流程**：
1. 使用 Allo 的 PyTorch 前端生成 Allo Python 代码
2. **直接转换**：将 Allo Python 代码转换为 Affine MLIR（跳过 Allo IR）
3. 应用 mlir-systolic 的脉动阵列变换
4. 使用 mlir-systolic 的 EmitHLSCpp 生成 HLS C++

**优点**：
- ✅ 更简洁的流程
- ✅ 减少抽象层
- ✅ 直接利用 mlir-systolic 的优化

**缺点**：
- ⚠️ 需要实现 Allo Python → Affine MLIR 的转换器
- ⚠️ 失去 Allo 的其他优化能力（非脉动阵列部分）

### 4.4 方案 C：混合方案（最灵活）

**流程**：
1. 使用 Allo 的 PyTorch 前端生成 Allo Python 代码
2. **识别关键循环**：分析代码，识别适合脉动阵列的循环嵌套（如 GEMM）
3. **分支处理**：
   - **脉动阵列部分**：转换为 Affine MLIR → mlir-systolic 处理
   - **其他部分**：使用 Allo 的正常流程
4. **合并**：将两部分结果合并

**优点**：
- ✅ 最大化利用两个工具的优势
- ✅ 灵活选择优化策略
- ✅ 可以逐步迁移

**缺点**：
- ⚠️ 实现复杂度最高
- ⚠️ 需要处理代码合并

---

## 5. 实施路线图

### 5.1 Phase 1: 前端集成（优先级：高）

**目标**：复用 Allo 的 PyTorch 前端

**任务**：
1. [ ] 研究 Allo 的 PyTorch 前端实现
2. [ ] 提取 PyTorch → Allo Python 代码生成逻辑
3. [ ] 创建独立的 PyTorch 前端模块（可选）
4. [ ] 测试 PyTorch 模型转换

**预计工作量**：2-3 周

### 5.2 Phase 2: Allo Python → Affine MLIR 转换器（优先级：高）

**目标**：将 Allo Python 代码转换为 Affine MLIR

**任务**：
1. [ ] 分析 Allo Python 语法
2. [ ] 实现 AST 解析器（或复用 Allo 的）
3. [ ] 实现 Affine MLIR 生成器
4. [ ] 处理常见操作（matmul, linear, conv2d 等）
5. [ ] 测试转换正确性

**预计工作量**：3-4 周

### 5.3 Phase 3: 关键循环识别（优先级：中）

**目标**：自动识别适合脉动阵列的循环嵌套

**任务**：
1. [ ] 分析循环嵌套模式（GEMM, Conv2D 等）
2. [ ] 实现循环识别算法
3. [ ] 标记关键循环区域
4. [ ] 测试识别准确性

**预计工作量**：2-3 周

### 5.4 Phase 4: mlir-systolic 集成（优先级：高）

**目标**：将识别出的循环传递给 mlir-systolic 处理

**任务**：
1. [ ] 确保 mlir-systolic 可以处理来自 Allo 的 Affine MLIR
2. [ ] 实现接口适配层
3. [ ] 测试端到端流程
4. [ ] 性能对比（与 Allo 手动 schedule 对比）

**预计工作量**：2-3 周

### 5.5 Phase 5: 代码合并与优化（优先级：中）

**目标**：处理脉动阵列部分与其他部分的合并

**任务**：
1. [ ] 实现代码合并逻辑
2. [ ] 处理接口兼容性
3. [ ] 优化整体性能
4. [ ] 端到端测试

**预计工作量**：2-3 周

---

## 6. 关键技术挑战

### 6.1 Allo Python → Affine MLIR 转换

**挑战**：
- Allo Python 是动态语言，需要类型推断
- 需要处理 Allo 的特殊语法（如 `dsl.grid`, `dsl.matmul`）
- 需要正确映射到 Affine 操作

**解决方案**：
- 复用 Allo 的类型推断逻辑
- 创建操作映射表（Allo → Affine）
- 参考 Allo 的 Parser 实现

### 6.2 循环识别

**挑战**：
- 如何自动识别适合脉动阵列的循环？
- 如何处理复杂的嵌套结构？
- 如何区分计算密集型和内存密集型操作？

**解决方案**：
- 基于模式匹配（GEMM, Conv2D 等）
- 使用启发式规则（循环深度、数据重用等）
- 参考 AutoSA 的 SCoP 检测逻辑

### 6.3 代码合并

**挑战**：
- 脉动阵列部分和其他部分的接口
- 数据布局兼容性
- 性能优化

**解决方案**：
- 使用标准化的接口（memref）
- 实现数据布局转换
- 参考 Allo 的 compose 机制

---

## 7. 开发路径选择：原有思路 vs 在 Allo 下重新梳理

### 7.1 两种路径对比

#### 路径 A：继续按照原有思路实现 mlir-systolic

**架构**：
```
Affine MLIR
  → SystolicTransform (依赖分析+分块)
  → SystolicDataflowGeneration (生成 SystolicDataflow Dialect)
  → SystolicDataflowToHLS (降级到 HLS Dialect)
  → EmitHLSCpp (从 HLS Dialect 生成 C++)
```

**优点**：
- ✅ **清晰的抽象层级**：SystolicDataflow Dialect 作为中间抽象，表达硬件结构清晰
- ✅ **与 AutoSA 对齐**：参考 AutoSA 的成熟架构，算法经过验证
- ✅ **独立性**：不依赖 Allo，可以作为独立工具使用
- ✅ **可扩展性**：Dialect 系统便于后续扩展和优化
- ✅ **代码质量**：结构化的代码生成，易于维护和调试

**缺点**：
- ⚠️ **实现复杂度**：需要实现完整的 Dialect 定义、Pass 和代码生成
- ⚠️ **开发周期**：需要较长时间才能达到可用状态
- ⚠️ **缺少前端**：没有 PyTorch 支持，需要手动编写 MLIR

#### 路径 B：在 Allo 下重新梳理思路

**架构**：
```
PyTorch 模型
  → Allo PyTorch Frontend (复用)
  → Allo Python 代码
  → Allo IR (包含 Affine)
  → 在 Allo IR 上应用脉动阵列变换
  → 直接生成 HLS C++ (复用 Allo 的 emit_vhls)
```

**优点**：
- ✅ **快速启动**：复用 Allo 的 PyTorch 前端和代码生成
- ✅ **用户友好**：Python API，易于使用
- ✅ **完整工具链**：前端、调度、代码生成一应俱全
- ✅ **多后端支持**：自动获得 Vivado/Vitis/TAPA/Intel 支持

**缺点**：
- ⚠️ **抽象层级低**：直接在 Affine MLIR 上操作，缺少硬件抽象
- ⚠️ **代码生成质量**：Allo 的代码生成是逐句翻译，可能不如结构化生成
- ⚠️ **依赖 Allo**：与 Allo 项目耦合，受其限制
- ⚠️ **难以表达复杂结构**：缺少 SystolicDataflow Dialect 这样的硬件抽象

### 7.2 混合路径：最佳方案（推荐）

**架构**：
```
PyTorch 模型
  → Allo PyTorch Frontend (复用) ⭐
  → Allo Python 代码
  → Allo IR (包含 Affine)
  → 提取关键循环嵌套
  → 转换为标准 Affine MLIR
  → mlir-systolic Pipeline (原有思路) ⭐
    - SystolicTransform
    - SystolicDataflowGeneration
    - SystolicDataflowToHLS
  → HLS Dialect
  → 代码生成（二选一）：
    - 方案 1: mlir-systolic 的 EmitHLSCpp ⭐
    - 方案 2: 降级回 Affine，使用 Allo 的 emit_vhls
```

**核心思想**：
1. **前端复用**：使用 Allo 的 PyTorch 支持，快速获得模型处理能力
2. **核心优化独立**：使用 mlir-systolic 的自动脉动阵列生成，保持算法独立性
3. **代码生成灵活**：可以选择使用 mlir-systolic 的结构化生成，或降级回 Affine 使用 Allo 的生成器

**优点**：
- ✅ **最佳组合**：结合两者的优势
- ✅ **快速启动**：复用 Allo 前端，快速支持 PyTorch
- ✅ **高质量优化**：使用 mlir-systolic 的自动脉动阵列生成
- ✅ **灵活性**：可以选择不同的代码生成策略
- ✅ **独立性**：mlir-systolic 核心逻辑独立，不依赖 Allo

**实施策略**：
1. **Phase 1**：继续实现 mlir-systolic 的核心功能（SystolicTransform, SystolicDataflowGeneration 等）
2. **Phase 2**：实现 Allo Python → Affine MLIR 转换器
3. **Phase 3**：集成 Allo 的 PyTorch 前端
4. **Phase 4**：实现代码生成器的选择机制

### 7.3 最终建议

**推荐：混合路径（在原有思路基础上集成 Allo 前端）**

**理由**：
1. **保持核心优势**：mlir-systolic 的 SystolicDataflow Dialect 和自动生成逻辑是核心价值，不应放弃
2. **快速获得前端能力**：复用 Allo 的 PyTorch 支持，快速支持用户需求
3. **最佳实践**：前端复用 + 核心独立 + 灵活代码生成，这是最佳实践
4. **长期发展**：mlir-systolic 作为独立项目，可以持续发展，不受 Allo 限制

**关键决策点**：
- ✅ **继续实现 SystolicDataflow Dialect**：这是 mlir-systolic 的核心抽象
- ✅ **复用 Allo 的 PyTorch 前端**：快速获得模型支持
- ✅ **保持代码生成独立性**：可以选择使用 mlir-systolic 的生成器或 Allo 的生成器
- ❌ **不在 Allo 下重新梳理**：这会失去 mlir-systolic 的核心优势

### 7.4 开发顺序（重要）

**阶段 1：先完成 mlir-systolic 核心功能** ⭐ **当前重点**

```
Affine MLIR
  → SystolicTransform (依赖分析+分块)
  → SystolicDataflowGeneration (生成 SystolicDataflow Dialect)
  → SystolicDataflowToHLS (降级到 HLS Dialect)
  → EmitHLSCpp (生成 HLS C++)
```

**目标**：打通 `affine.for` → 自动脉动阵列生成的完整流程

**关键里程碑**：
- [ ] SystolicTransform 完成（依赖分析、空间循环选择、分块）
- [ ] SystolicDataflowGeneration 完成（IO 模块生成、双缓冲）
- [ ] SystolicDataflowToHLS 完成（Dialect 降级）
- [ ] EmitHLSCpp 完成（代码生成）
- [ ] 端到端测试通过（MatMul 等）

**阶段 2：集成 Allo 前端** ⭐ **后续工作**

```
PyTorch 模型
  → Allo PyTorch Frontend (复用)
  → Allo Python 代码
  → Allo IR (包含 Affine)
  → 提取 Affine MLIR
  → mlir-systolic Pipeline (阶段 1 已完成)
  → HLS C++ 代码
```

**衔接点**：Allo IR 中的 `AffineForOp` → mlir-systolic 的输入

**技术细节**：
1. **Allo 保留 Affine Dialect**：Allo 的 lowering pipeline 不降低 Affine，可以直接提取
2. **接口标准化**：mlir-systolic 接受标准的 Affine MLIR，与 Allo 兼容
3. **无缝衔接**：Allo IR → 提取 Affine 部分 → mlir-systolic → HLS Dialect → 代码生成

**优势**：
- ✅ **分阶段开发**：先完成核心功能，再集成前端
- ✅ **独立验证**：mlir-systolic 可以独立测试和验证
- ✅ **低耦合**：两个工具通过标准的 Affine MLIR 接口连接
- ✅ **灵活性**：可以选择使用 mlir-systolic 的代码生成或 Allo 的代码生成

---

## 8. 结论与建议

### 8.1 可行性评估

**高度可行** ✅

理由：
1. Allo 和 mlir-systolic 都基于 MLIR，兼容性好
2. Allo 的 PyTorch 前端已经成熟
3. mlir-systolic 的脉动阵列生成逻辑独立，易于集成
4. 两个项目的目标互补
5. Allo 保留 Affine Dialect，可以直接传递给 mlir-systolic 处理

### 8.2 推荐方案

**推荐：混合路径（在原有思路基础上集成 Allo 前端）**

理由：
1. **保持核心优势**：SystolicDataflow Dialect 是 mlir-systolic 的核心价值
2. **快速获得前端**：复用 Allo 的 PyTorch 支持
3. **最佳实践**：前端复用 + 核心独立 + 灵活代码生成
4. **长期发展**：mlir-systolic 作为独立项目，可以持续发展

### 8.3 实施建议（按优先级排序）

#### 阶段 1：完成 mlir-systolic 核心功能（当前重点）⭐

1. **继续实现 SystolicTransform**：
   - 依赖分析（Polymer/ISL）
   - 空间循环选择（依赖距离 ≤ 1）
   - 循环置换和分块
   - 目标：从 `affine.for` 到分块后的 Affine IR

2. **实现 SystolicDataflowGeneration**：
   - 数组引用分组（IO/PE/Drain）
   - IO 层级分析（L1/L2/L3）
   - 生成 SystolicDataflow Dialect
   - 双缓冲逻辑生成

3. **实现 SystolicDataflowToHLS**：
   - Dialect 降级到 HLS Dialect
   - 双缓冲逻辑转换

4. **实现 EmitHLSCpp**：
   - 从 HLS Dialect 生成 HLS C++
   - Pragma 插入

5. **端到端测试**：
   - MatMul 等测试用例
   - 与 AutoSA 输出对比

#### 阶段 2：集成 Allo 前端（后续工作）

1. **研究 Allo IR 结构**：
   - 了解 Allo 如何保留 Affine Dialect
   - 设计 Affine MLIR 提取接口

2. **实现接口适配层**：
   - Allo IR → 提取 Affine MLIR
   - 处理 Allo 特有的属性（如 `loop_name`, `op_name`）

3. **集成 Allo PyTorch 前端**：
   - 复用 `allo/frontend/pytorch.py`
   - 实现 Allo Python → Allo IR → Affine MLIR 的转换

4. **端到端集成测试**：
   - PyTorch 模型 → mlir-systolic → HLS C++
   - 性能对比和验证

**关键原则**：
- ✅ **先完成核心，再集成前端**：确保 mlir-systolic 核心功能稳定
- ✅ **保持接口标准化**：通过标准的 Affine MLIR 连接，降低耦合
- ✅ **分阶段验证**：每个阶段都有可验证的里程碑

### 8.4 预期收益

1. **用户体验**：用户可以直接从 PyTorch 模型生成脉动阵列 FPGA 代码
2. **自动化**：减少手动编写 schedule 的工作
3. **性能**：自动生成的脉动阵列可能比手动 schedule 更优（基于多面体分析）
4. **可扩展性**：为后续支持更多模型和操作打下基础
5. **独立性**：mlir-systolic 作为独立项目，可以持续发展

---

## 9. Allo Affine 之后环节的深入分析

### 9.1 Allo 的代码生成机制

**关键发现**：Allo **不降低 Affine Dialect**，直接从 Affine MLIR 生成 HLS C++。

**代码生成流程**（`allo/mlir/lib/Translation/EmitVivadoHLS.cpp`）：

1. **ModuleEmitter 类**：主要的代码生成器
   - 使用 Visitor 模式遍历 MLIR 操作
   - 对每种操作类型有对应的 emit 方法

2. **Affine 操作处理**：
   ```cpp
   void emitAffineFor(AffineForOp op);      // 生成 for 循环
   void emitAffineLoad(AffineLoadOp op);    // 生成数组读取
   void emitAffineStore(AffineStoreOp op);  // 生成数组写入
   void emitAffineIf(AffineIfOp op);        // 生成 if 语句
   ```

3. **Pragma 插入**：
   - 根据操作属性插入 HLS pragma
   - 例如：`pipeline_ii` → `#pragma HLS pipeline II=...`
   - 例如：`rewind` → `#pragma HLS rewind`

4. **直接代码生成**：
   - 没有中间的 HLS Dialect
   - 直接从 Affine 操作生成 C++ 代码
   - 代码生成器约 2700 行，处理各种情况

### 9.2 与 mlir-systolic 的对比

| 方面 | Allo | mlir-systolic |
|------|------|---------------|
| **抽象层级** | 低（直接 Affine） | 高（SystolicDataflow Dialect） |
| **代码生成** | 逐句翻译 | 结构化生成 |
| **硬件表达** | 通过 Affine 操作 + 属性 | 通过专用 Dialect |
| **可维护性** | 中等（大文件，复杂逻辑） | 高（分层清晰） |
| **扩展性** | 中等（需要修改代码生成器） | 高（Dialect 系统） |

### 9.3 对集成的影响

**关键洞察**：
1. Allo 保留 Affine Dialect，可以直接传递给 mlir-systolic
2. mlir-systolic 可以生成 HLS Dialect，然后选择：
   - 使用自己的 EmitHLSCpp（结构化生成）
   - 或降级回 Affine，使用 Allo 的 emit_vhls（快速但质量可能较低）

**集成策略**：
- **推荐**：使用 mlir-systolic 的 EmitHLSCpp，因为：
  - 结构化生成，代码质量更高
  - 与 SystolicDataflow Dialect 配合更好
  - 便于维护和扩展
- **备选**：如果需要快速验证，可以降级回 Affine 使用 Allo 的生成器

---

## 10. 参考资料

- [Allo 项目](https://github.com/cornell-zhang/allo)
- [Allo 文档](https://cornell-zhang.github.io/allo)
- [mlir-systolic README](../README.md)
- [AutoSA 分析](./AUTOSA_ANALYSIS.md)

---

**下一步行动**：

**当前阶段（阶段 1）**：
1. ✅ 继续实现 mlir-systolic 核心功能
   - SystolicTransform（依赖分析+分块）
   - SystolicDataflowGeneration（IO 模块生成）
   - SystolicDataflowToHLS（Dialect 降级）
   - EmitHLSCpp（代码生成）
2. ✅ 端到端测试：从 `affine.for` 到 HLS C++ 的完整流程
3. ✅ 与 AutoSA 输出对比验证

**后续阶段（阶段 2）**：
1. 研究 Allo IR 结构，设计 Affine MLIR 提取接口
2. 实现 Allo IR → Affine MLIR 的转换层
3. 集成 Allo 的 PyTorch 前端
4. 端到端集成测试

**关键洞察**：
- Allo **不降低 Affine Dialect**，可以直接提取 Affine MLIR
- mlir-systolic 接受标准的 Affine MLIR，与 Allo 完全兼容
- 两个工具通过标准的 Affine MLIR 接口无缝衔接

