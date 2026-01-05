# AutoSA 详细分析

> **最后更新**: 2025-12-15  
> **目的**: 全面分析 AutoSA 的逻辑、架构和实现细节，为 mlir-systolic 提供参考

---

## 目录

1. [AutoSA 概述](#1-autosa-概述)
2. [整体架构](#2-整体架构)
3. [核心算法流程](#3-核心算法流程)
4. [多面体分析与变换](#4-多面体分析与变换)
5. [参数对脉动阵列结构的影响](#5-参数对脉动阵列结构的影响)
6. [代码生成流程](#6-代码生成流程)
7. [与 mlir-systolic 的对应关系](#7-与-mlir-systolic-的对应关系)

---

## 1. AutoSA 概述

### 1.1 什么是 AutoSA

AutoSA 是一个基于 PPCG（Polyhedral Parallel Code Generator）的编译器，专门用于将 C 语言中的嵌套循环（SCoP 区域）转换为 FPGA 上的脉动阵列 HLS C++ 代码。

**核心特点**：
- 基于多面体模型（Polyhedral Model）进行依赖分析和循环变换
- 使用 ISL（Integer Set Library）进行调度树（Schedule Tree）计算
- 支持多种脉动阵列架构（1D/2D，不同空间-时间映射）
- 自动生成多层 IO 模块和双缓冲结构
- 支持 SIMD 向量化优化

### 1.2 入口脚本

AutoSA 的入口是 `autosa.py`（安装后重命名为 `autosa`），主要功能：

1. **参数解析**：解析命令行参数（target、output-dir、sa-sizes 等）
2. **调用核心程序**：执行 `./src/autosa` 进行核心编译
3. **后处理**：生成 top module、合并代码、清理临时文件
4. **代码生成**：根据 target 调用 `codegen.py` 生成最终代码

**关键参数**：
- `--target`: `autosa_hls_c`（Xilinx HLS）、`autosa_opencl`（Intel OpenCL）、`autosa_tapa`（TAPA）、`autosa_catapult_c`（Catapult HLS）
- `--sa-sizes`: 脉动阵列参数（space_time、array_part、latency、simd）
- `--output-dir`: 输出目录
- `--host-serialize`: 启用 Host 端序列化（用于 SIMD 打包）

---

## 2. 整体架构

### 2.1 两部分架构

AutoSA 包含两个主要部分：

#### 2.1.1 FPGA Kernel 生成（核心）

**输入**：被 `#pragma scop` / `#pragma endscop` 包裹的嵌套循环

**处理流程**：
1. SCoP 提取（使用 PET）
2. 多面体分析和调度（使用 ISL）
3. 空间-时间变换
4. 多级分块（array_part + latency）
5. IO 模块生成（L1/L2/L3）
6. PE 阵列生成
7. HLS C++ 代码生成

**输出**：FPGA Kernel HLS C++ 代码

#### 2.1.2 Host 端代码生成

**根据 target 生成**：
- `autosa_hls_c` → HLS C Testbench 或 OpenCL Host
- `autosa_opencl` → OpenCL Host
- `autosa_tapa` → TAPA Host
- `autosa_catapult_c` → Catapult Host

**当前状态**：mlir-systolic 已预留接口，暂不实现

### 2.2 核心数据结构

#### 2.2.1 `struct autosa_kernel`

脉动阵列 kernel 的核心数据结构：

```c
struct autosa_kernel {
  isl_ctx *ctx;
  isl_schedule *schedule;           // ISL 调度树
  struct ppcg_scop *scop;           // SCoP 信息
  
  // 空间-时间映射
  int n_sa_dim;                     // 脉动阵列维度数（1 或 2）
  int sa_dim[3];                    // 空间维度索引
  int space_parallel[3];            // 空间并行维度
  int space_time_id;                // 空间-时间模式（0-5）
  
  // 分块参数
  int array_part_w;                 // array_part 宽度
  int space_w;                      // 空间维度宽度
  int time_w;                       // 时间维度宽度
  int simd_w;                       // SIMD 宽度
  int lat_hide_len;                 // 延迟隐藏长度
  
  // 数组信息
  int n_array;
  struct autosa_local_array_info *array;
  
  // 循环信息
  struct autosa_loop *loop;
  int n_loop;
  
  // 硬件模块
  struct autosa_hw_module **modules;
  int n_modules;
};
```

#### 2.2.2 `struct autosa_hw_module`

硬件模块（IO、PE、Drain）的抽象：

```c
struct autosa_hw_module {
  enum autosa_module_type type;     // PE_MODULE, IO_MODULE, DRAIN_MODULE
  int level;                         // IO 层级（L1/L2/L3）
  enum autosa_io_dir dir;            // IO 方向（IN/OUT）
  struct autosa_array_ref_group *group;  // 关联的数组引用组
  // ...
};
```

#### 2.2.3 `struct autosa_array_ref_group`

数组引用分组（IO/PE/Drain）：

```c
struct autosa_array_ref_group {
  enum autosa_group_type type;       // IO_GROUP, PE_GROUP, DRAIN_GROUP
  struct autosa_array_info *array;   // 关联的数组
  // 访问关系
  isl_union_map *access;             // 访问映射
  isl_union_map *write;              // 写映射
  isl_union_map *read;               // 读映射
  // 分块信息
  struct autosa_array_tile *tile;    // 数组分块
  // ...
};
```

---

## 3. 核心算法流程

### 3.1 主流程（`generate_sa`）

```c
int generate_sa(isl_ctx *ctx, const char *input, FILE *out,
                struct ppcg_options *options, ...) {
  // 1. 提取 SCoP（使用 PET）
  struct ppcg_scop *scop = ppcg_scop_extract(ctx, input, options);
  
  // 2. 计算调度树（使用 ISL）
  isl_schedule *schedule = isl_schedule_compute_schedule(scop);
  
  // 3. 合法性检查
  if (!sa_legality_check(schedule, scop))
    return -1;
  
  // 4. 空间-时间变换
  struct autosa_kernel **sa_list;
  isl_size num_sa;
  sa_list = sa_space_time_transform(schedule, scop, &num_sa);
  
  // 5. 选择最佳 kernel（如果有多个候选）
  struct autosa_kernel *sa = sa_candidates_smart_pick(sa_list, num_sa);
  
  // 6. 计算管理（分块、置换）
  compute_management(gen, sa, pass_en, pass_mode);
  
  // 7. 通信管理（IO 模块生成）
  sa_io_construct_optimize(sa, gen);
  
  // 8. 生成硬件模块
  generate_hw_modules(sa, gen);
  
  // 9. 代码生成
  print_hw(sa, gen, ...);
  
  return 0;
}
```

### 3.2 空间-时间变换（`sa_space_time_transform`）

**目的**：将循环维度映射到空间维度（PE 索引）和时间维度（执行顺序）

**关键函数**：
- `sa_space_time_transform_at_dim_async`: 异步脉动阵列变换
- `sa_space_time_transform_at_dim_sync`: 同步脉动阵列变换

**步骤**：
1. **依赖距离分析**：计算每个循环维度的依赖距离（`get_dep_dis_at_node`）
2. **空间循环选择**：选择依赖距离 ≤ 1 的循环作为空间循环
3. **空间-时间模式**：根据用户指定的 `space_time` 参数确定映射

**空间-时间模式（0-5）**：

| 模式 | 空间维度 | 阵列类型 | 说明 |
|------|---------|---------|------|
| 0: [i] | i | 1D Row | 行脉动阵列 |
| 1: [j] | j | 1D Column | 列脉动阵列 |
| 2: [k] | k | 1D Reduction | 归约脉动阵列 |
| 3: [i,j] | i, j | 2D Output-Stationary | 输出驻留 2D 阵列（默认） |
| 4: [i,k] | i, k | 2D with H-Reduce | 水平归约 2D 阵列 |
| 5: [j,k] | j, k | 2D with V-Reduce | 垂直归约 2D 阵列 |

### 3.3 计算管理（`compute_management`）

**目的**：应用多级分块和循环置换

**步骤**：
1. **Array Partitioning**（`sa_array_partitioning_optimize`）：
   - 第一级分块：创建 PE 阵列的 tile 循环
   - 分块大小：`array_part[i, j, k]`
   
2. **Latency Hiding**（`sa_latency_hiding_optimize`）：
   - 第二级分块：创建点循环（point loops）
   - 分块大小：`latency[i, j]`
   - 目的：隐藏数据加载延迟

3. **SIMD Vectorization**（`sa_simd_vectorization_optimize`）：
   - 向量化最内层循环
   - SIMD 因子：`simd`

4. **循环置换**：
   - 将空间循环移到外层
   - 将归约循环移到最内层

**分块后的循环结构**（以 3 层循环为例）：

```
原始循环: for i, for j, for k

Array Partition 后:
  for i0 (tile), for j0 (tile), for k0 (tile)
    for i1 (point), for j1 (point), for k1 (point)

Latency Hiding 后:
  for i0, for j0, for k0
    for i1 (latency), for j1 (latency), for k1
      for i2 (point), for j2 (point), for k2 (point)

最终置换后（AutoSA 特定）:
  for i0, for j0, for k0
    for i1, for j1, for k1
      for j2, for k2, for i2  // 归约循环 k2 最内层
```

### 3.4 通信管理（`sa_io_construct_optimize`）

**目的**：生成多层 IO 模块和双缓冲结构

**步骤**：
1. **数组引用分组**：
   - 分析所有数组访问
   - 分组为 IO_GROUP、PE_GROUP、DRAIN_GROUP

2. **IO 层级分析**：
   - **L3**：Global Memory ↔ L3 Stream（最外层 tile 循环）
   - **L2**：L3 Stream ↔ L2 Stream（双缓冲，latency tile 循环）
   - **L1**：L2 Stream ↔ PE（最内层 point 循环）

3. **双缓冲生成**：
   - 为每个 L2 IO 模块生成 ping-pong 缓冲区
   - Inter-transfer：从 L3 加载到非活动缓冲区
   - Intra-transfer：从活动缓冲区发送到 L1

4. **PE 阵列生成**：
   - 根据 `array_part` 和 `latency` 计算 PE 阵列大小
   - 生成每个 PE 的实例
   - 生成 PE 之间的脉动数据流

---

## 4. 多面体分析与变换

### 4.1 SCoP 提取

AutoSA 使用 **PET**（Polyhedral Extraction Tool）从 C 代码中提取 SCoP：

1. 识别 `#pragma scop` / `#pragma endscop` 区域
2. 提取循环嵌套和数组访问
3. 构建 ISL 域（domain）和访问关系（access relations）

### 4.2 依赖分析

**关键函数**：`get_dep_dis_at_node`（在 `autosa_utils.cpp` 中）

**步骤**：
1. 计算数据依赖（RAW、WAR、WAW）
2. 计算依赖距离向量（dependence distance vector）
3. 判断是否为均匀依赖（uniform dependence）

**依赖距离判断**：
- 依赖距离 ≤ 1：可以作为空间循环（PE 索引）
- 依赖距离 > 1：只能作为时间循环（执行顺序）

### 4.3 调度树计算

AutoSA 使用 **ISL Schedule Tree** 进行调度：

1. **调度计算**：`isl_schedule_compute_schedule(scop)`
   - 输入：SCoP（域、访问关系、依赖）
   - 输出：ISL Schedule Tree

2. **调度树结构**：
   ```
   domain
   ├── band (循环带)
   │   ├── filter (过滤条件)
   │   └── schedule (调度映射)
   └── ...
   ```

3. **调度树遍历**：
   - AutoSA 使用 `isl_schedule_node` API 遍历调度树
   - 识别 kernel 节点、IO 标记、PE 节点等

### 4.4 循环变换

AutoSA 在调度树上应用变换：

1. **循环置换**：`loop_interchange_at_node`
2. **循环分块**：`isl_schedule_node_band_tile`
3. **循环展开**：用于 SIMD 向量化

---

## 5. 参数对脉动阵列结构的影响

> 本节内容基于 scalehls 文档中的实际测试和分析

### 5.1 核心参数

| 参数 | 说明 | 示例值 |
|------|------|--------|
| `space_time` | 空间-时间映射模式 | 3（2D output-stationary） |
| `array_part` | 第一级分块大小 | [16, 16, 16] |
| `latency` | 第二级分块大小（延迟隐藏） | [8, 8] |
| `simd` | SIMD 向量化因子 | 2 |

### 5.2 PE 阵列大小计算

**公式**：
```
numPE_i = array_part_i / latency_i
numPE_j = array_part_j / latency_j
```

**示例**（已验证）：
- `array_part=[8,8,8]`, `latency=[4,4]` → `8/4 = 2` → **2×2 PE 阵列** ✓
  - 验证：`mm_st3_I32_J32_K32_ap8_lat4_simd1` → PE_wrapper(idx: 0-1, idy: 0-1) = 4 个 PE
- `array_part=[16,16,16]`, `latency=[8,8]` → `16/8 = 2` → **2×2 PE 阵列** ✓
  - 验证：`mm_st3_I64_J64_K64_ap16_lat8_simd1` → PE_wrapper(idx: 0-1, idy: 0-1) = 4 个 PE
- `array_part=[32,32,32]`, `latency=[8,8]` → `32/8 = 4` → **4×4 PE 阵列** ✓
  - 验证：`mm_st3_I64_J64_K64_ap32_lat8_simd1` → PE_wrapper(idx: 0-3, idy: 0-3) = 16 个 PE
- `array_part=[16,16,16]`, `latency=[4,4]` → `16/4 = 4` → **4×4 PE 阵列** ✓
  - 验证：`mm_st3_I64_J64_K64_ap16_lat4_simd1` → PE_wrapper(idx: 0-3, idy: 0-3) = 16 个 PE

**验证方法**：
- 在生成的 HLS C++ 代码中，`PE_wrapper` 的调用次数 = `numPE_i × numPE_j`
- `idx` 参数范围：`0` 到 `numPE_i - 1`
- `idy` 参数范围：`0` 到 `numPE_j - 1`
- L2 IO 模块的 `c3` 循环范围也对应 `numPE_i`（或 `numPE_j`）

**注意**：PE 阵列大小**不直接依赖问题规模**（如 64×64），而是由 `array_part` 和 `latency` 决定。

### 5.3 循环结构影响

#### 5.3.1 Array Partition 分块

**作用**：创建 PE 阵列的 tile 循环

**示例**（64×64 矩阵，`array_part=[16,16,16]`）：
```
原始: for i = 0 to 64, for j = 0 to 64, for k = 0 to 64

Array Partition 后:
  for i0 = 0 to 4 (64/16)      // Tile i
    for j0 = 0 to 4 (64/16)    // Tile j
      for k0 = 0 to 4 (64/16)  // Tile k
        for i1 = 0 to 16        // Point i
          for j1 = 0 to 16      // Point j
            for k1 = 0 to 16    // Point k
```

#### 5.3.2 Latency Hiding 分块

**作用**：创建点循环，隐藏数据加载延迟

**示例**（`latency=[8,8]`，已验证）：
```
在 Point 循环上应用 Latency Hiding:
  for i0, j0, k0 (Tile loops)
    for i1 = 0 to 2 (16/8)     // Latency tile i
      for j1 = 0 to 2 (16/8)   // Latency tile j
        for k1 = 0 to 16        // Latency tile k (不变)
          for i2 = 0 to 8       // Point i
            for j2 = 0 to 8     // Point j
              for k2 = 0 to 16    // Point k
```

**验证**（`mm_st3_I64_J64_K64_ap16_lat8_simd1`）：
- `c0, c1, c2`: 0 到 3（array tile，64/16=4 tiles）
- `c3`: 0 到 1（latency tile，对应 L2 IO 模块的 `idx`，16/8=2）
- `c4`: 0 到 7（latency tile，对应 L2 IO 模块的 `c4`，16/8=2？不对，应该是 8）
- `c5`: 0 到 7（latency，对应 PE 内部的 `c5`）
- `c6, c7`: 0 到 7（point，对应 `local_C[8][8]` 的索引）

**最终循环结构**（9 层循环）：
```
0: c0 (array tile i)
1: c1 (array tile j)
2: c2 (array tile k)
3: c3 (latency tile i, 对应 L2 IO idx)
4: c4 (latency tile j, 对应 L2 IO c4)
5: c5 (latency tile k, 对应 PE 内部 c5)
6: c6 (point i, 对应 local_C[c7][c6] 的 c6)
7: c7 (point j, 对应 local_C[c7][c6] 的 c7)
8: (point k 被合并到 c5 中)
```

**PE 内部累加缓冲区大小**：
- `local_C[latency_i][latency_j]` = `local_C[8][8]`（当 `latency=[8,8]`）
- `local_C[4][4]`（当 `latency=[4,4]`）
- 验证：`mm_st3_I32_J32_K32_ap8_lat4_simd1` → `local_C[4][4]` ✓
- 验证：`mm_st3_I64_J64_K64_ap16_lat8_simd1` → `local_C[8][8]` ✓

### 5.4 SIMD 影响

**作用**：向量化最内层循环，减少迭代次数

**示例**（`simd=2`）：
```
原始最内层循环: for k2 = 0 to 16

SIMD=2 后:
  for k2 = 0 to 8 (16/2)       // 迭代次数减半
    // 每次处理 2 个元素（展开）
    local_C += A[0] * B[0] + A[1] * B[1]
```

**类型映射**：
- `simd=1` → `float`
- `simd=2` → `ap_uint<64>`（打包 2 个 float）
- `simd=4` → `ap_uint<128>`（打包 4 个 float）

### 5.5 空间-时间模式影响

不同 `space_time` 模式影响数据流方向：

| 模式 | A 流 | B 流 | C 流 |
|------|------|------|------|
| 0: [i] | Direct | Horizontal | Local |
| 1: [j] | Horizontal | Direct | Local |
| 2: [k] | Broadcast | Broadcast | Reduce Horizontal |
| 3: [i,j] | Horizontal | Vertical | Local |
| 4: [i,k] | Direct | Vertical | Reduce Horizontal |
| 5: [j,k] | Horizontal | Direct | Reduce Vertical |

**示例**（space_time=3，2D output-stationary）：
- Matrix A：水平流动（沿 j 轴）
- Matrix B：垂直流动（沿 i 轴）
- Matrix C：本地累加（无流动）

---

## 6. 代码生成流程

### 6.1 硬件模块生成

AutoSA 生成以下硬件模块：

#### 6.1.1 IO 模块（L3）

**功能**：Global Memory ↔ L3 Stream

**命名**：`{array}_IO_L3_{dir}`

**示例**：
- `A_IO_L3_in`: 从 Global Memory 读取 A，写入 L3 Stream
- `A_IO_L3_out`: 从 L3 Stream 读取，写入 Global Memory

#### 6.1.2 IO 模块（L2，双缓冲）

**功能**：L3 Stream ↔ L2 Stream（带双缓冲）

**命名**：`{array}_IO_L2_{dir}`

**结构**：
- Ping-Pong 缓冲区
- Inter-transfer：从 L3 加载到非活动缓冲区
- Intra-transfer：从活动缓冲区发送到 L2

#### 6.1.3 IO 模块（L1）

**功能**：L2 Stream ↔ PE

**命名**：`{array}_IO_L1_{dir}` 或直接集成到 PE

#### 6.1.4 PE 模块

**功能**：执行计算（MAC 操作）

**命名**：`PE_{i}_{j}` 或 `PE_wrapper`（函数调用模式）

**结构**：
- 本地累加缓冲区：`local_C[latency_i][latency_j]`
- SIMD 缓冲区：`local_A[1][simd]`, `local_B[1][simd]`
- 脉动数据流：从相邻 PE 读取，转发到下一个 PE

#### 6.1.5 Drain 模块

**功能**：收集 PE 输出，写回 Global Memory

**命名**：`{array}_drain_IO_L{level}_{dir}`

### 6.2 HLS C++ 代码生成

AutoSA 使用自定义的 C printer 生成 HLS C++ 代码：

1. **遍历调度树**：使用 `isl_ast_build` 生成 AST
2. **模块生成**：为每个硬件模块生成函数
3. **Stream 生成**：生成 `hls::stream<T>` 声明
4. **Pragma 插入**：插入 HLS pragma（pipeline、array_partition 等）

**关键 Pragma**：
- `#pragma HLS PIPELINE II=1`: 内层循环流水线
- `#pragma HLS ARRAY_PARTITION`: 数组分区（用于 SIMD 缓冲区）
- `#pragma HLS RESOURCE`: 资源约束（BRAM、DSP 等）
- `#pragma HLS DATAFLOW`: 数据流优化（顶层）

### 6.3 Host 端代码生成

根据 `target` 参数生成不同的 Host 代码：

- **HLS Testbench**：用于 Vivado HLS 仿真
- **OpenCL Host**：用于 Xilinx/Intel OpenCL 运行时
- **TAPA Host**：用于 TAPA 平台
- **Catapult Host**：用于 Catapult HLS

---

## 7. 与 mlir-systolic 的对应关系

### 7.1 整体流程对应

| AutoSA 阶段 | mlir-systolic Pass | 说明 |
|------------|-------------------|------|
| SCoP 提取 | Polygeist | 使用 Polygeist 从 C 提取 MLIR |
| 调度树计算 | Polymer | 使用 Polymer 获取 ISL Schedule Tree |
| 空间-时间变换 | `SystolicTransform` | 依赖分析、空间循环选择、循环置换 |
| 多级分块 | `SystolicTransform` | Array Partition + Latency Hiding |
| IO 模块生成 | `SystolicDataflowGeneration` | 生成 SystolicDataflow Dialect |
| 代码生成 | `EmitHLSCpp` | 生成 HLS C++ |

### 7.2 数据结构对应

| AutoSA 结构 | mlir-systolic 结构 | 说明 |
|------------|-------------------|------|
| `autosa_kernel` | `func::FuncOp` | Kernel 函数 |
| `autosa_hw_module` | `SystolicDataflow` Dialect Op | IO/PE/Drain 模块 |
| `autosa_array_ref_group` | `ArrayRefGroup` | 数组引用分组 |
| ISL Schedule Tree | `ScheduleTreeInfo` | 调度树信息（通过 Polymer） |

### 7.3 关键算法对应

| AutoSA 函数 | mlir-systolic 函数 | 状态 |
|------------|-------------------|------|
| `sa_legality_check` | `checkLegality` | ✅ 已实现 |
| `get_dep_dis_at_node` | `analyzeDependenceDistances` | ⚠️ 部分实现（需要 Polymer） |
| `sa_space_time_transform` | `selectSpaceLoops` + `permuteLoopsForSpaceTime` | ✅ 已实现 |
| `sa_array_partitioning_optimize` | `applyMultiLevelTiling` (Level 1) | ✅ 已实现 |
| `sa_latency_hiding_optimize` | `applyMultiLevelTiling` (Level 2) | ✅ 已实现 |
| `sa_io_construct_optimize` | `SystolicDataflowGeneration` | ⚠️ 部分实现 |
| `generate_hw_modules` | `SystolicDataflowGeneration` | ⚠️ 部分实现 |

### 7.4 参数对应

| AutoSA 参数 | mlir-systolic 参数 | 说明 |
|------------|-------------------|------|
| `space_time` | `spaceTimeMode` | 空间-时间模式（0-5） |
| `array_part` | `arrayPart` | 第一级分块大小 |
| `latency` | `latency` | 第二级分块大小 |
| `simd` | `simdWidth` | SIMD 向量化因子 |

### 7.5 主要差异

1. **多面体分析**：
   - AutoSA：直接使用 ISL
   - mlir-systolic：通过 Polymer 使用 ISL（更灵活）

2. **中间表示**：
   - AutoSA：ISL AST + 自定义 C printer
   - mlir-systolic：MLIR Dialect（SystolicDataflow → HLS）

3. **模块化**：
   - AutoSA：直接生成 C++ 函数
   - mlir-systolic：使用 MLIR Dialect 抽象，便于优化

4. **扩展性**：
   - AutoSA：修改 C printer 较困难
   - mlir-systolic：利用 MLIR Pass 系统，易于扩展

---

## 8. 关键实现细节

### 8.1 依赖距离计算

AutoSA 使用 ISL 计算依赖距离：

```c
// 伪代码
isl_union_map *deps = compute_dependences(scop);
isl_union_map *distances = compute_dependence_distances(deps, schedule);

// 对每个循环维度
for (int dim = 0; dim < n_dims; dim++) {
  isl_val *min_dist = get_min_distance(distances, dim);
  isl_val *max_dist = get_max_distance(distances, dim);
  
  if (isl_val_get_num_si(max_dist) <= 1) {
    can_be_space_loop[dim] = true;
  }
}
```

### 8.2 双缓冲实现

AutoSA 的双缓冲逻辑：

```c
// 伪代码
bool arbiter = false;  // Ping-Pong 选择器
bool intra_en = false; // Intra-transfer 使能

for (tile loops) {
  if (arbiter) {
    // Inter: 加载到 ping 缓冲区
    load_to_buffer(ping_buffer, L3_stream);
    // Intra: 从 pong 缓冲区发送（如果使能）
    if (intra_en) {
      send_from_buffer(pong_buffer, L2_stream);
    }
  } else {
    // Inter: 加载到 pong 缓冲区
    load_to_buffer(pong_buffer, L3_stream);
    // Intra: 从 ping 缓冲区发送（如果使能）
    if (intra_en) {
      send_from_buffer(ping_buffer, L2_stream);
    }
  }
  
  intra_en = true;        // 第一个 tile 后使能
  arbiter = !arbiter;     // 切换缓冲区
}

// 最后 drain：发送最后一个缓冲区
send_remaining_buffer(arbiter ? pong_buffer : ping_buffer);
```

### 8.3 PE 脉动数据流

AutoSA 的 PE 数据流逻辑：

```c
// 伪代码（Matrix A，水平流动）
void PE(int pe_i, int pe_j, ...) {
  if (pe_j == 0) {
    // 边界 PE：从 feeder 读取
    data = read_from_feeder();
  } else {
    // 内部 PE：从左侧 PE 读取
    data = read_from_left_neighbor();
  }
  
  // 转发到右侧 PE
  write_to_right_neighbor(data);
  
  // 计算
  local_C += data * other_data;
}
```

---

## 9. 参考资源

### 9.1 AutoSA 源代码

- **核心变换**：`autosa_trans.cpp` / `autosa_trans.h`
- **通信管理**：`autosa_comm.cpp` / `autosa_comm.h`
- **代码生成**：`autosa_codegen.cpp` / `autosa_codegen.h`
- **调度树操作**：`autosa_schedule_tree.cpp` / `autosa_schedule_tree.h`
- **工具函数**：`autosa_utils.cpp` / `autosa_utils.h`
- **通用定义**：`autosa_common.h`

### 9.2 ScaleHLS 文档

- **详细实现**：`scalehls/docs/MLIR_AutoSA_Implementation.md`
- **快速参考**：`scalehls/docs/AutoSA_Quick_Reference.md`
- **输出对比**：`scalehls/docs/HLS_Output_Comparison.md`

### 9.3 测试用例

- **AutoSA 参考输出**：`mlir-systolic/test/reference-samples/`
- **MLIR 测试用例**：`mlir-systolic/test/matmul/matmul.mlir`

---

## 10. 总结

AutoSA 是一个功能完整的脉动阵列编译器，其核心特点：

1. **基于多面体模型**：使用 ISL 进行精确的依赖分析和调度
2. **灵活的架构**：支持多种空间-时间映射模式
3. **优化的硬件结构**：自动生成多层 IO 模块和双缓冲
4. **完整的代码生成**：生成 FPGA Kernel 和 Host 端代码

mlir-systolic 的目标是在 MLIR 框架内重新实现 AutoSA 的核心功能，同时利用 MLIR 的优势（如 Dialect 系统、Pass 组合）提供更好的扩展性和灵活性。

---

**文档维护**：本文档应随着 mlir-systolic 的实现进展持续更新，确保与 AutoSA 的对应关系准确。

