# MLIR-Systolic 架构设计

## 1. 设计理念

### 1.1 核心问题

AutoSA 的能力来自 ISL 的完整多面体编译栈：
- `isl_schedule` - 调度树表示
- `isl_flow` - 精确依赖分析
- `isl_ast_build` - 从调度生成代码

MLIR Affine 缺少这些能力，但提供：
- 强大的 IR 变换基础设施
- 丰富的方言生态
- 成熟的代码生成框架

### 1.2 解决方案

**用 Polymer 桥接 ISL 到 MLIR**：
- Polymer 提供 MLIR ↔ ISL 的双向转换
- 在 ISL 层面做依赖分析和调度决策
- 在 MLIR 层面做变换和代码生成

## 2. 编译流程

```
┌─────────────────────────────────────────────────────────────────┐
│                         编译流程                                 │
└─────────────────────────────────────────────────────────────────┘

输入: C 代码 或 MLIR Affine
        │
        ▼
┌───────────────────────────────────────┐
│  Stage 1: 前端解析                     │
│  ┌─────────────────────────────────┐  │
│  │ Polygeist: C → MLIR Affine      │  │
│  │ (cgeist -function=xxx ...)      │  │
│  └─────────────────────────────────┘  │
└───────────────────────────────────────┘
        │
        ▼ MLIR Affine IR
┌───────────────────────────────────────┐
│  Stage 2: 多面体分析 (Polymer)         │
│  ┌─────────────────────────────────┐  │
│  │ 2.1 SCoP 提取                   │  │
│  │     polymer-opt -extract-scop   │  │
│  │                                 │  │
│  │ 2.2 依赖分析                    │  │
│  │     - 计算 isl_flow             │  │
│  │     - 提取依赖距离向量           │  │
│  │                                 │  │
│  │ 2.3 空间循环选择                │  │
│  │     - 依赖距离 ≤ 1 的循环        │  │
│  │     - 枚举合法空间维度组合       │  │
│  │                                 │  │
│  │ 输出: SpaceTimeInfo             │  │
│  │   - space_loops: [0, 1]         │  │
│  │   - time_loops: [2]             │  │
│  │   - dep_distances: [...]        │  │
│  └─────────────────────────────────┘  │
└───────────────────────────────────────┘
        │
        ▼ SpaceTimeInfo + MLIR Affine
┌───────────────────────────────────────┐
│  Stage 3: Space-Time 变换              │
│  ┌─────────────────────────────────┐  │
│  │ 3.1 循环置换                    │  │
│  │     - 空间循环移到外层           │  │
│  │                                 │  │
│  │ 3.2 多级分块                    │  │
│  │     - array_part (PE 阵列大小)  │  │
│  │     - latency (延迟隐藏)        │  │
│  │                                 │  │
│  │ 3.3 SIMD 向量化                 │  │
│  │     - 最内层循环向量化           │  │
│  └─────────────────────────────────┘  │
└───────────────────────────────────────┘
        │
        ▼ Tiled/Permuted MLIR Affine
┌───────────────────────────────────────┐
│  Stage 4: 数据流构造                   │
│  ┌─────────────────────────────────┐  │
│  │ 4.1 Stream 通道生成             │  │
│  │     - hls.dataflow.stream       │  │
│  │                                 │  │
│  │ 4.2 I/O 模块生成                │  │
│  │     - L3: DDR ↔ Global Stream   │  │
│  │     - L2: Relay + Double Buffer │  │
│  │     - L1: PE 内部 Stream         │  │
│  │                                 │  │
│  │ 4.3 PE 模块生成                 │  │
│  │     - hls.dataflow.task         │  │
│  │     - 脉动数据流 (A横向, B纵向)  │  │
│  └─────────────────────────────────┘  │
└───────────────────────────────────────┘
        │
        ▼ HLS Dialect IR
┌───────────────────────────────────────┐
│  Stage 5: HLS C++ 代码生成             │
│  ┌─────────────────────────────────┐  │
│  │ - systolic-translate            │  │
│  │ - EmitHLSCpp (from ScaleHLS)    │  │
│  │                                 │  │
│  │ 输出:                           │  │
│  │   - kernel.cpp (PE 逻辑)        │  │
│  │   - io_modules.cpp (I/O 模块)   │  │
│  │   - top.cpp (顶层调度)          │  │
│  └─────────────────────────────────┘  │
└───────────────────────────────────────┘
        │
        ▼
输出: HLS C++ (Vivado HLS / Vitis 兼容)
```

## 3. 核心数据结构

### 3.1 SpaceTimeInfo

```cpp
/// 空间-时间分析结果，由 Polymer 分析层生成
struct SpaceTimeInfo {
  /// 可以作为空间循环的索引 (依赖距离 ≤ 1)
  SmallVector<unsigned> spaceLoopCandidates;
  
  /// 选定的空间循环索引
  SmallVector<unsigned> spaceLoops;
  
  /// 时间循环索引 (剩余循环)
  SmallVector<unsigned> timeLoops;
  
  /// 每个循环的依赖距离向量
  DenseMap<unsigned, SmallVector<int64_t>> depDistances;
  
  /// 是否是规约循环
  DenseMap<unsigned, bool> isReductionLoop;
  
  /// 数据流方向分析结果
  struct DataFlowInfo {
    SystolicFlowDir flowDir;  // HORIZONTAL, VERTICAL, NONE
    bool isReductionAccumulator;
  };
  DenseMap<Value, DataFlowInfo> operandFlows;
};
```

### 3.2 SystolicConfig

```cpp
/// 脉动阵列配置参数
struct SystolicConfig {
  /// PE 阵列维度
  unsigned numPE_I;  // 行数
  unsigned numPE_J;  // 列数
  
  /// 分块参数
  SmallVector<int64_t> arrayPart;  // 第一级分块
  SmallVector<int64_t> latency;    // 第二级分块 (延迟隐藏)
  
  /// SIMD 宽度
  unsigned simdWidth;
  
  /// I/O 配置
  unsigned ddrBitWidth;  // DDR 位宽 (32, 64, 128, 256, 512)
  bool enableDoubleBuffer;
  
  /// 从 SpaceTimeInfo 计算得出
  static SystolicConfig fromSpaceTimeInfo(
      const SpaceTimeInfo &info,
      ArrayRef<int64_t> problemSize,
      ArrayRef<int64_t> tileHints);
};
```

## 4. 模块职责

### 4.1 lib/Analysis/ - Polymer 分析层

| 文件 | 职责 |
|------|------|
| `DependenceAnalysis.cpp` | 调用 Polymer/ISL 计算依赖距离 |
| `SpaceLoopSelection.cpp` | 选择合法的空间循环 |
| `DataFlowAnalysis.cpp` | 分析操作数的数据流方向 |

**核心接口**：
```cpp
/// 分析给定 affine 循环带的空间-时间属性
LogicalResult analyzeSpaceTime(
    AffineForOp outerLoop,
    SpaceTimeInfo &result);

/// 验证给定的空间循环选择是否合法
bool verifySpaceLoopSelection(
    AffineForOp outerLoop,
    ArrayRef<unsigned> spaceLoops);
```

### 4.2 lib/Transforms/ - 变换层

| 文件 | 职责 | 来源 |
|------|------|------|
| `SpaceTimeTransform.cpp` | 循环置换 + 分块 | 新实现，参考 AutoSAMap |
| `StreamGeneration.cpp` | 生成 Stream 通道 | 移植自 ScaleHLS |
| `IOModuleGeneration.cpp` | 生成 I/O 模块 | 移植自 ScaleHLS |
| `SIMDVectorization.cpp` | SIMD 向量化 | 移植自 ScaleHLS |

### 4.3 lib/Dialect/HLS/ - HLS 方言

直接从 ScaleHLS 移植：
- `StreamOp`, `StreamReadOp`, `StreamWriteOp`
- `TaskOp`, `DispatchOp`, `YieldOp`
- `BufferOp`, `AxiOp` 等

### 4.4 lib/Translation/ - 代码生成

| 文件 | 职责 | 来源 |
|------|------|------|
| `EmitHLSCpp.cpp` | 生成 HLS C++ | 移植自 ScaleHLS |
| `EmitIOModules.cpp` | 生成 I/O 模块代码 | 新实现，模板化 |

## 5. 与原 AutoSA 的映射

| AutoSA 函数 | 本项目对应 | 说明 |
|------------|-----------|------|
| `sa_legality_check()` | `Analysis/SpaceLoopSelection.cpp` | 检查循环嵌套合法性 |
| `sa_space_time_transform()` | `Transforms/SpaceTimeTransform.cpp` | 空间-时间变换 |
| `compute_management()` | `Transforms/` 多个文件 | array_part, latency, simd |
| `comm_management()` | `Transforms/IOModuleGeneration.cpp` | I/O 构造 |
| `generate_hw_modules()` | `Transforms/StreamGeneration.cpp` | 模块生成 |
| `print_hw()` | `Translation/EmitHLSCpp.cpp` | HLS C++ 输出 |

## 6. 开发优先级

### Phase 1: 最小可行产品 (MVP)
目标：MatMul 端到端工作

1. **移植 HLS Dialect** (1-2 天)
   - 复制 ScaleHLS 的 Ops.td, Types.td
   - 调整命名空间

2. **移植 EmitHLSCpp** (1-2 天)
   - 复制核心代码生成逻辑
   - 简化为模板输出

3. **实现 Space-Time 变换** (2-3 天)
   - 先硬编码 MatMul 的配置
   - 后续接入 Polymer 分析

4. **端到端测试** (1 天)

### Phase 2: Polymer 集成
目标：自动空间循环选择

1. **依赖分析 Pass** (3-5 天)
   - 调用 Polymer 的 ISL 接口
   - 提取依赖距离

2. **空间循环选择** (2-3 天)
   - 实现 AutoSA 的选择逻辑

### Phase 3: 完整功能
- 多种内核支持 (Conv, GEMV, ...)
- I/O 优化 (带宽打包, 链式 L2)
- 性能调优

## 7. 与 ScaleHLS 代码的关系

```
ScaleHLS (LLVM 16)              mlir-systolic (LLVM 18)
─────────────────               ─────────────────────────
Dialect/HLS/                    复制 → Dialect/HLS/
  - Ops.td                            - Ops.td (调整命名空间)
  - Types.td                          - Types.td
  - HLS.cpp                           - HLS.cpp
                                
Translation/                    复制 → Translation/
  - EmitHLSCpp.cpp                    - EmitHLSCpp.cpp (简化)

Transforms/Dataflow/            参考 → Transforms/
  - AutoSAMap.cpp                     - SpaceTimeTransform.cpp
                                      - StreamGeneration.cpp
                                
                                新增 → Analysis/
                                      - DependenceAnalysis.cpp
                                      - SpaceLoopSelection.cpp
```
