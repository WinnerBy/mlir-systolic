# 用 MLIR 复现 AutoSA 逻辑：技术路线与实现方案

> **文档版本**: 1.0  
> **创建时间**: 2026-01-07  
> **范围**: 变换链条（调度、spacetime、数据流、模块生成）  
> **约束**: 当前验证覆盖 ST3+MM，需扩展至 ST0–ST5、多 kernel、多维循环

---

## 目录
1. [总体目标与范围](#总体目标与范围)
2. [背景：AutoSA 架构回顾](#背景autosa-架构回顾)
3. [AutoSA 与 MLIR 实现的核心差异](#autosa-与-mlir-实现的核心差异)
4. [MLIR 复现的核心技术路线](#mlir-复现的核心技术路线)
5. [调度与多面体分析路线](#调度与多面体分析路线)
6. [Spacetime 参数化与数据流推导](#spacetime-参数化与数据流推导)
7. [MLIR Pass 分层与映射](#mlir-pass-分层与映射)
8. [实现细节与改造点](#实现细节与改造点)
9. [渐进式落地计划](#渐进式落地计划)
10. [附录：AutoSA 源码要点](#附录autosa-源码要点)

---

## 总体目标与范围

### 目标
在 MLIR 上重建 AutoSA 的核心编译路径，实现从 Affine 循环嵌套到脉动阵列硬件配置的全自动转换。

**核心变换链条**：
```
Affine IR (via Polygeist)
       ↓
[SpaceTimeAnalysis] → ParametricSpaceTime + 依赖分析
       ↓
[SystolicTransform] → 调度树标注、loop 置换、tile 策略
       ↓
[SystolicDataflowGeneration] → systolic.io / systolic.pe Dialect
       ↓
[SystolicDataflowToHLS] → 缓冲优化、流通道
       ↓
[systolic-translate] → HLS C++ / Vitis 代码
```

### 范围
- **In Scope**（本路线重点）：变换层（Analysis/Transform/Dataflow）。
- **Out of Scope**（保留 MLIR 优势）：代码生成、硬件验证、性能调优。
- **约束**：仅提供有限支持（不同于 AutoSA 的生产级覆盖）；目标是正确性与可维护性。

### 与现状对标
| 特性 | 当前 mlir-systolic | AutoSA | 目标（本路线） |
|------|------------------|--------|------------|
| Spacetime 配置 | 硬编码 ST3 | ST0–ST5 自动选择 | 参数化支持 ST0–ST5 |
| PE 维度 | 固定 2D | 变量（1–3D） | 1–2D 参数化 |
| Loop 维度 | 假设 3 层 | 支持 4+ 层 | N 维支持 |
| 数据流推导 | 依赖数组名 | 依赖访问投影 | 访问投影（去名字） |
| 调度操作 | Affine AST 改写 | ISL schedule tree | Schedule tree（Polymer） |
| 配置持久化 | 无 | isl_union_map | MLIR FuncOp 属性 |
| Kernel 支持 | MM only | MM/CNN/MTTKRP/LU/... | MM/MTTKRP（后续扩展） |

---

## 背景：AutoSA 架构回顾

### AutoSA 编译流程（源码视角）
```
C 输入 (PET parser)
  ↓
SCoP 提取 & 多项式分析 (PPCG/ISL)
  ↓
[Space-Time Transform] (autosa_trans.cpp)
  • 在 schedule tree 上标注 band 成员为 space/time
  • 依赖距离检查与一致性验证
  • 产生多个候选 kernel (不同 space-time 划分)
  ↓
[Array Partitioning / Latency Hiding / SIMD] (compute_management)
  • 根据访问模式决定 complete/cyclic/block
  • 隐藏流水线延迟
  ↓
[硬件模块生成 & 代码] (autosa_codegen.cpp / autosa_xilinx_hls_c.cpp)
  • PE 阵列、IO 模块、双缓冲
  • HLS 指令与 pragma
```

### 关键数据结构与操作
1. **Schedule Tree**（ISL）
   - Band 节点：`isl_schedule_node_band_member_get_space_time(node, i)` 读取成员i的标记。
   - 标注为 `autosa_loop_space` 或 `autosa_loop_time`（或 `autosa_loop_reduce` for reduction）。
   - 支持 band 置换：`loop_interchange_at_node(node, level1, level2)`。

2. **依赖分析**（ISL）
   - `autosa_schedule_tree.cpp`：`get_dep_dis_at_node()` 计算依赖距离向量。
   - `is_dep_uniform()` 检查依赖是否为常量（必要条件）。
   - 距离 ≤ 1 的维度可作为 space 循环。

3. **Space-Time 变换**（autosa_trans.h）
   - `sa_space_time_transform_at_dim()` 在指定维度进行 space-time 变换。
   - `sa_space_time_transform()` 枚举所有可行配置，返回多个候选 kernel。
   - 智能/手工选择最优 kernel：`sa_candidates_smart_pick() / sa_candidates_manual_pick()`。

4. **配置读取**（autosa_common.cpp）
   - `extract_sa_sizes(union_map, type)` 从 ISL union_map 中按"type"（space_time/array_part/latency）提取参数。
   - `read_*_tile_sizes()` 系列函数回填到 `autosa_kernel` 结构体。

5. **数据流推导**（访问投影）
   - 无硬编码数组名；通过访问映射与 space 迭代器求交判定流向。
   - 例：memref 访问仅依赖第 0 个 space 维 → HORIZONTAL；仅依赖第 1 个 → VERTICAL；全依赖 → NONE (PE-local)。

---

## AutoSA 与 MLIR 实现的核心差异

### 1. 调度操作层次
| 方面 | AutoSA | 当前 mlir-systolic | 问题 |
|------|--------|------------------|------|
| **调度编辑** | ISL schedule tree band 标注/置换 | Affine loop AST 改写 | 改 AST 时维度耦合强，难扩展 |
| **空间循环选择** | `selectSpaceLoops()` 依赖距离过滤 | 硬编码 `[0,1]` | 无法支持 ST0/ST2/ST4 等 |
| **时间循环** | 剩余非 space 维 | 硬编码 `[2..]` | 3+ 维循环时失效 |
| **Loop 置换** | 在 schedule tree 完成 | 在 Affine 层手工改循环体 | 易出错，难复用 |

**根本原因**：现实现在 **Affine IR 层** 做转换，而 Affine 不天然支持多维参数化；AutoSA 在 **schedule tree（多面体）层** 操作，参数化能力强。

### 2. Spacetime 与数据流推导
| 方面 | AutoSA | 当前 mlir-systolic | 问题 |
|------|--------|------------------|------|
| **数据流判定** | 访问投影 vs space 迭代器 | 硬编码数组名分支（A/B/C） | 新 kernel 需改代码 |
| **访问分析** | 动态计算 `operandFlows` | 静态写死 `HORIZONTAL/VERTICAL` | 无法支持异构访问模式 |
| **PE 维度** | 参数化（`n_sa_dim`） | 假设 2D | 1D/3D 配置无法生成 |
| **IO 结构** | 按 space 维数与数组秩动态生成 L1/L2/L3 | 假设 3 层（L3/L2/L1） | 1D PE 时结构不合理 |

**根本原因**：缺少"访问投影分析"的通用框架；依赖数组名是脆弱的黑盒判断。

### 3. 配置管理与持久化
| 方面 | AutoSA | 当前 mlir-systolic | 问题 |
|------|--------|------------------|------|
| **配置存储** | `isl_union_map` sizes | 多处重复序列化，无统一载体 | pass 间耦合，难以重入 |
| **参数传递** | `extract_sa_sizes()` 系列 | 属性/内联写死 | 扩展困难 |
| **复用性** | 支持多 kernel 与调优网格搜索 | 每改一个配置需改代码 | 不适合参数扫描 |

### 4. 循环维度与 Kernel 多样性
| 方面 | AutoSA | 当前 mlir-systolic | 问题 |
|------|--------|------------------|------|
| **循环维度** | 4+ 维（MTTKRP、LU） | 假设 3 维 | 4+ 维循环无法生成 |
| **Kernel 支持** | MM、CNN、MTTKRP、LU、稀疏等 | 仅 MM | 需大改才能支持其他 kernel |
| **约化维** | 显式标记、单独处理 | 混在 time 维 | 延迟隐藏、流通道不准确 |

---

## MLIR 复现的核心技术路线

### 整体思路
1. **引入参数化数据结构**：`ParametricSpaceTime` 统一表达所有 spacetime 配置。
2. **分离顾虑**：将"合法性检查"与"配置生成"与"IR 转换"分离到独立 pass。
3. **基于多面体分析**：依托 Polymer（ISL）的调度树与依赖信息，而不是 Affine AST 硬编码。
4. **维度无关的生成**：所有循环/缓冲/流向生成都参数化，支持 1D/2D/多维。
5. **配置持久化**：通过 MLIR FuncOp 属性保存配置，支持 pass 链共享与重入。

### 三层变换架构（推荐）
```
┌─────────────────────────────────────────────────────────────┐
│ Layer 1: Analysis (多面体分析 + 合法性检查)                  │
├─────────────────────────────────────────────────────────────┤
│ • SpaceTimeAnalysis                                         │
│   - 依赖距离计算（Polymer）                               │
│   - 候选空间维枚举与过滤                                  │
│   - 产出 ParametricSpaceTime + operandFlows               │
└─────────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────────┐
│ Layer 2: Transform (调度编辑 + IR 注解)                     │
├─────────────────────────────────────────────────────────────┤
│ • SystolicTransform                                        │
│   - Loop 置换/标注（优先在 schedule tree）               │
│   - Tile 大小与并行度决策                                │
│   - 设置 FuncOp 属性 (SystolicConfiguration)            │
│                                                           │
│ • (可选) LoopOptimization                                │
│   - 额外的循环变换（strip-mine、skew 等）              │
└─────────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────────┐
│ Layer 3: Lowering (Dialect 生成 + 代码合成)                │
├─────────────────────────────────────────────────────────────┤
│ • SystolicDataflowGeneration                              │
│   - 数组引用分组（IO/PE/Drain）                         │
│   - 生成 systolic.io.module / systolic.pe.array          │
│   - IO 层级与双缓冲决策                                  │
│                                                           │
│ • SystolicDataflowToHLS                                  │
│   - 缓冲合并与流优化                                     │
│                                                           │
│ • systolic-translate (to C++/Vitis)                      │
│   - 维度无关的循环展开与序列化                          │
└─────────────────────────────────────────────────────────────┘
```

---

## 调度与多面体分析路线

### 4.1 多面体基础（Polymer 替代 PPCG）

#### 为什么需要 Polymer？
- **AutoSA 依赖 ISL schedule tree**：所有变换（space/time 标注、依赖检查、loop 置换）都在 mupa（多元部分仿射）与 band 节点上进行。
- **MLIR Affine 不足**：Affine dialect 不直接暴露调度信息，难以参数化操作。
- **Polymer 提供 ISL 绑定**：可获得 schedule tree、dependence map、并支持 band 操作。

#### Polymer 集成点
| 任务 | Polymer API | 用途 |
|------|------------|------|
| 获取调度树 | `isl_schedule`（从 Polygeist 提取或 Polymer 内生） | 初始化变换基础 |
| 依赖距离 | `isl_union_map` (flow/rar deps) | space 循环合法性检查 |
| Band 操作 | `isl_schedule_node_band_*` | space/time 标注与置换 |
| 配置读写 | `isl_union_map` sizes 映射 | 参数序列化 |

#### 轻量调度 API 设计
在 `include/systolic/Analysis/ScheduleTreeUtils.h` 中定义：

```cpp
namespace mlir::systolic::schedule {

/// Band 成员标签（类似 AutoSA 的 autosa_loop_space/time/reduce）
enum LoopType {
  SPACE = 0,
  TIME = 1,
  REDUCE = 2,      // 约化维，由 time 单独处理
  SEQUENTIAL = 3   // 顺序维（不并行）
};

/// 在 band 节点的成员 idx 上设置标签
isl_schedule_node *bandMemberSetLoopType(
    isl_schedule_node *node, int idx, LoopType type);

/// 交换 band 节点的两个成员位置（对应 loop 置换）
isl_schedule_node *bandMembersInterchange(
    isl_schedule_node *node, int i, int j);

/// 获取 band 节点的部分调度（mupa）
isl_multi_union_pw_aff *bandGetPartialSchedule(
    isl_schedule_node *node);

/// 计算给定 band 下的依赖距离向量（针对具体依赖）
isl_vec *computeDependenceDistanceAtBand(
    isl_basic_map *dep,
    isl_schedule_node *band);

/// 检验是否所有 flow/rar 依赖在此 band 均为常数距离（合法 space 循环必要条件）
bool allDependencesUniformAtBand(
    isl_schedule *schedule,
    struct ppcg_scop *scop,
    isl_schedule_node *band,
    int memberIdx);

} // namespace mlir::systolic::schedule
```

### 4.2 依赖分析与候选空间维搜索

#### 核心流程
```
输入: Affine 循环嵌套 + 依赖信息（来自 Polymer）
  ↓
1. 提取所有 loop 维度与依赖关系
2. 对每个非约化维，计算其与所有依赖的距离
3. 过滤：距离必须是常数且 ≤ 1
4. 候选空间维组合（1–2 个，取决于 PE 维度需求）
  ↓
输出: spaceLoopCandidates[], timeLoops[], reduction flag
```

#### 实现位置：`SpaceTimeAnalysis`
```cpp
// 新增方法
LogicalResult SpaceTimeAnalysis::identifySpaceLoopCandidates(
    AffineForOp outerLoop,
    const DependenceInfo &deps,
    SmallVectorImpl<unsigned> &candidates);

LogicalResult SpaceTimeAnalysis::validateSpaceLoops(
    const SmallVectorImpl<unsigned> &spaceLoopIndices,
    const DependenceInfo &deps);

LogicalResult SpaceTimeAnalysis::inferParametricSpaceTime(
    AffineForOp outerLoop,
    const DependenceInfo &deps,
    unsigned preferredSpaceDims = 2);
```

#### 依赖分析集成
- **输入**：来自 Polymer 的 `isl_union_map` (dep_flow / dep_rar)。
- **处理**：对每个依赖，在目标 band 下计算距离向量；检查是否全为常数。
- **输出**：boolean flag 或距离向量数组（供后续分析）。

### 4.3 调度树标注与配置反馈

#### 标注阶段
在 `SystolicTransform` 中，对所有 band 成员执行：
```cpp
for (int i = 0; i < spaceLoopIndices.size(); ++i) {
  bandMemberSetLoopType(bandNode, spaceLoopIndices[i], SPACE);
}
for (int j = 0; j < timeLoopIndices.size(); ++j) {
  bandMemberSetLoopType(bandNode, timeLoopIndices[j], TIME);
}
if (hasReductionLoop) {
  bandMemberSetLoopType(bandNode, reductionIdx, REDUCE);
}
```

#### 配置持久化
将 `ParametricSpaceTime` 与其他超参数（tile 大小、array_part、latency）打包为 MLIR 属性：
```cpp
struct SystolicConfiguration {
  SmallVector<unsigned, 3> spaceLoopDims;
  SmallVector<unsigned, 3> timeLoopDims;
  SmallVector<int64_t> tileSizes;      // 对应各维
  SmallVector<int64_t> arrayPartFactors;
  int latencyHidingLength;
  bool doubleBuffer;
  // ...
};

// 保存到 FuncOp
func.setAttr("systolic.config", ConfigurationAttr::get(ctx, config));
```

---

## Spacetime 参数化与数据流推导

### 5.1 ParametricSpaceTime 数据结构

#### 定义（`include/systolic/Analysis/ParametricSpaceTime.h`）
```cpp
namespace mlir::systolic {

/// Space-Time 配置的参数化表示
/// 支持从 ST0（1D PE）到 ST5（2D PE，不同方向）的所有配置
struct ParametricSpaceTime {
  // 空间循环维度索引（如 [0, 1] 表示 i, j 维，PE 阵列为 2D）
  SmallVector<unsigned, 3> spaceLoopDims;
  
  // 时间循环维度索引（如 [2] 表示 k 维）
  SmallVector<unsigned, 3> timeLoopDims;
  
  // 约化维度索引（如 [3] 表示某些卷积/张量操作的约化维，单独处理）
  SmallVector<unsigned, 2> reductionDims;
  
  // 循环名（用于调试与代码生成）
  SmallVector<StringRef, 4> loopNames;
  
  // PE 阵列维数（= spaceLoopDims.size()，通常为 1–2）
  unsigned peArrayDims;
  
  // 每个空间维的优化参数（并行度、管道级、SIMD 宽度）
  struct SpaceDimConfig {
    unsigned parallelism;    // PE 数量
    unsigned pipelineII;     // 初始间隔
    unsigned simdWidth;      // 向量化宽度
  };
  SmallVector<SpaceDimConfig, 3> spaceDimConfigs;
  
  // 时间维的优化参数
  struct TimeDimConfig {
    unsigned pipelineDepth;  // 流水线深度
    int latencyHiding;       // 延迟隐藏长度（-1 为自动）
  };
  TimeDimConfig timeDimConfig;
  
  // 数据流方向映射（Value/operand → flow direction）
  // 由 SpaceTimeAnalysis::analyzeDataFlow 产生
  DenseMap<Value, enum SystolicFlowDir> operandFlows;
  
  // Spacetime 类型字符串（如 "ST3"）
  std::string spacetimeTypeString;
  
  /// 验证此配置是否有效
  bool isValid() const;
  
  /// 转换为人类可读的字符串
  std::string toString() const;
  
  /// Dump 调试信息
  void dump() const;
};

/// 预定义的 spacetime 配置（ST0–ST5）
namespace presets {
  /// ST0: [i] 空间，[j, k] 时间（1D PE 行方向）
  ParametricSpaceTime createST0();
  
  /// ST1: [j] 空间，[i, k] 时间（1D PE 列方向）
  ParametricSpaceTime createST1();
  
  /// ST2: [k] 空间，[i, j] 时间（1D PE 深度方向，少见）
  ParametricSpaceTime createST2();
  
  /// ST3: [i, j] 空间，[k] 时间（标准 2D PE，输出静止）
  ParametricSpaceTime createST3();
  
  /// ST4: [i, k] 空间，[j] 时间（2D PE，权重静止）
  ParametricSpaceTime createST4();
  
  /// ST5: [j, k] 空间，[i] 时间（2D PE，激活静止）
  ParametricSpaceTime createST5();
}

} // namespace mlir::systolic
```

#### 与 AutoSA 对应关系
| ST | mlir-systolic | AutoSA 逻辑 | PE 配置 |
|----|--------------|-----------|--------|
| ST0 | `[0]`, `[1, 2]` | i 固定，j/k 流 | 1D 行 |
| ST1 | `[1]`, `[0, 2]` | j 固定，i/k 流 | 1D 列 |
| ST2 | `[2]`, `[0, 1]` | k 固定，i/j 流 | 1D 深（罕见） |
| ST3 | `[0, 1]`, `[2]` | i/j 固定，k 流 | 2D（输出静止） |
| ST4 | `[0, 2]`, `[1]` | i/k 固定，j 流 | 2D（权重静止） |
| ST5 | `[1, 2]`, `[0]` | j/k 固定，i 流 | 2D（激活静止） |

### 5.2 数据流推导（去数组名硬编码）

#### 核心原理
对于每个数组/memref 的访问，计算其对空间迭代器的**投影**：

```
访问：A[i, k]
空间维：[i, j]（ST3）
投影：A 使用 i（第 0 个空间维），不使用 j
→ 流向 = HORIZONTAL（沿 i 传递）

访问：B[k, j]
投影：B 使用 j（第 1 个空间维），不使用 i
→ 流向 = VERTICAL（沿 j 传递）

访问：C[i, j]
投影：C 使用 i 和 j（两个空间维）
→ 流向 = NONE（PE-local，无跨 PE 传输）
```

#### 实现流程（`SpaceTimeAnalysis::analyzeDataFlow`）
1. 获取当前 loop 的所有 memref 访问。
2. 对每个 memref，获其访问映射（访问坐标与循环变量关系）。
3. 计算该访问对每个空间维的**投影**（是否依赖该空间维变量）。
4. 根据投影结果分配流向：
   - 仅用 space[0] → HORIZONTAL
   - 仅用 space[1] → VERTICAL
   - 用全部 space dims → NONE
   - 不用 space dims → NONE（标量/broadcast）
5. 存入 `operandFlows: map<Value, FlowDir>`。

#### 伪代码
```cpp
LogicalResult SpaceTimeAnalysis::analyzeDataFlow(
    AffineForOp outerLoop,
    const ParametricSpaceTime &spacetime,
    DenseMap<Value, SystolicFlowDir> &operandFlows) {
  
  // 遍历 loop 内所有访问
  SmallVector<AffineReadOpInterface, 16> reads;
  SmallVector<AffineWriteOpInterface, 16> writes;
  gatherMemoryOperations(outerLoop, reads, writes);
  
  for (auto readOp : reads) {
    Value memref = readOp.getMemref();
    AffineMap accessMap = readOp.getAffineMap();
    
    // 计算访问对空间维的投影
    bool usesSpace0 = accessMapDependsOn(accessMap, spacetime.spaceLoopDims[0]);
    bool usesSpace1 = (spacetime.peArrayDims >= 2) && 
                      accessMapDependsOn(accessMap, spacetime.spaceLoopDims[1]);
    
    SystolicFlowDir flowDir;
    if (usesSpace0 && usesSpace1) {
      flowDir = SystolicFlowDir::NONE;  // PE-local
    } else if (usesSpace0 && !usesSpace1) {
      flowDir = SystolicFlowDir::HORIZONTAL;
    } else if (!usesSpace0 && usesSpace1) {
      flowDir = SystolicFlowDir::VERTICAL;
    } else {
      flowDir = SystolicFlowDir::NONE;  // 标量
    }
    
    operandFlows[memref] = flowDir;
  }
  
  return success();
}
```

### 5.3 自动 Spacetime 推荐

#### 推荐策略
1. **候选空间维枚举**：从合法候选中选择 1 或 2 个（按依赖距离）。
2. **阵列秩匹配**：PE 维数应与数组秩兼容（至少 PE 维 ≤ 数组秩）。
3. **对称性打破**：偏好覆盖"不对称"访问模式的配置（如 A[i,k], B[k,j] 提示 ST3）。
4. **启发式评分**：综合考虑流方向平衡、缓冲大小、通信开销。

#### 预设选择与用户覆盖
```cpp
std::vector<ParametricSpaceTime> SpaceTimeAnalysis::recommendConfigurations(
    AffineForOp outerLoop,
    const DependenceInfo &deps,
    unsigned maxSpaceDims = 2) {
  
  SmallVector<ParametricSpaceTime> candidates;
  
  // 枚举可行空间维组合
  for (unsigned numSpaceDims = 1; numSpaceDims <= maxSpaceDims; ++numSpaceDims) {
    // 生成所有 numSpaceDims 的组合
    for (auto spaceDims : combinations(spaceLoopCandidates, numSpaceDims)) {
      auto spacetime = ParametricSpaceTime{spaceDims, ...};
      if (spacetime.isValid()) {
        candidates.push_back(spacetime);
      }
    }
  }
  
  // 排序（启发式评分）
  std::sort(candidates.begin(), candidates.end(),
            [this, deps](const auto &a, const auto &b) {
              return scoreSpaceTime(a, deps) > scoreSpaceTime(b, deps);
            });
  
  return candidates;
}
```

---

## MLIR Pass 分层与映射

### 6.1 Analysis 层（多面体分析 + 合法性检查）

#### SpaceTimeAnalysis Pass
**职责**：
- 从 Affine loop 提取循环信息、依赖约束。
- 调用 Polymer 进行依赖距离分析。
- 产生 `ParametricSpaceTime` 与 `operandFlows`。

**输入**：Affine 循环嵌套 + Polymer 调度树/依赖信息。  
**输出**：  
- `SpaceTimeInfo` 结构体（包含 `parametric`、`operandFlows` 等）。
- 打印/日志：候选配置列表及评分。

**关键方法**：
```cpp
class SpaceTimeAnalysisPass : public PassBase<...> {
  LogicalResult analyze(AffineForOp loop, SpaceTimeInfo &result);
  
  // 内部助手
  LogicalResult identifySpaceLoopCandidates(...);
  LogicalResult validateSpaceLoops(...);
  LogicalResult inferParametricSpaceTime(...);
  LogicalResult analyzeDataFlow(...);
};
```

#### 可选：LoopStructureAnalysis（前置）
分析循环的秩、归纳变量、边界等，为后续 pass 服务。

### 6.2 Transform 层（调度编辑 + IR 注解）

#### SystolicTransform Pass
**职责**：
- 消费 `SpaceTimeAnalysis` 的结果（`ParametricSpaceTime`）。
- 在循环上执行标注与置换（优先在 schedule tree，备选 Affine 层）。
- 设置 tile 大小、并行度、array_part 等配置。
- 存储 `SystolicConfiguration` 到 FuncOp 属性。

**输入**：标注过的 Affine loop（含 `systolic.spacetime` 属性）。  
**输出**：  
- 置换并标注后的 Affine loop。
- FuncOp 属性：`systolic.configuration`。

**关键方法**：
```cpp
class SystolicTransformPass : public PassBase<...> {
  LogicalResult performLoopInterchange(AffineForOp loop,
                                       const ParametricSpaceTime &config);
  
  LogicalResult annotateLoopsWithSpaceTime(AffineForOp loop,
                                           const ParametricSpaceTime &config);
  
  LogicalResult computeTileSizes(AffineForOp loop,
                                 const DependenceInfo &deps,
                                 SmallVectorImpl<int64_t> &tileSizes);
  
  LogicalResult persistConfiguration(FuncOp func,
                                     const SystolicConfiguration &config);
};
```

#### 可选：LoopOptimizationPass
额外的循环变换（strip-mine、skew、fusion 等），为 systolic 优化服务。

### 6.3 Lowering 层（Dialect 生成 + 代码合成）

#### SystolicDataflowGeneration Pass
**职责**：
- 从 `SystolicConfiguration` 读取 space/time 维度与数据流信息。
- 分组数组引用（IO 缓冲、PE 计算、drain）。
- 决策 IO 层级（L1/L2/L3）与双缓冲。
- 生成 `systolic.io.module`、`systolic.pe.array` 等高层 Dialect。

**输入**：Affine loop + 配置属性。  
**输出**：systolic Dialect IR（结构化表示硬件模块）。

**关键变更**（去硬编码）：
```cpp
// 旧：硬编码数组名
if (arrayName == "A") flows[memref] = HORIZONTAL;

// 新：使用 operandFlows
auto flowIt = config.operandFlows.find(memref);
if (flowIt != config.operandFlows.end()) {
  flows[memref] = flowIt->second;
}
```

```cpp
// 旧：假设 3 维、2D PE
for (int i = 0; i < 2; ++i) {
  createIOModule(/* L3/L2 */, arrayPartVec[i]);
}

// 新：按 peArrayDims 生成
for (int i = 0; i < spacetime.peArrayDims; ++i) {
  createIOModule(/* level based on rank/config */, ...);
}
```

#### SystolicDataflowToHLS Pass
**职责**：
- 缓冲合并与流通道优化。
- 生成双缓冲与流同步逻辑。

**输入**：systolic Dialect IR。  
**输出**：优化后的 systolic Dialect IR（可选）。

#### systolic-translate / Serialization
**职责**：
- 将 systolic Dialect 转换为 HLS C++/Vitis HDL。
- 循环展开与序列化（维度无关）。

**关键变更**：
```cpp
// 旧：假设 3D 嵌套
for (int i = 0; i < M; ++i) {
  for (int j = 0; j < N; ++j) {
    for (int k = 0; k < K; ++k) {
      // compute
    }
  }
}

// 新：动态生成循环
for (unsigned d = 0; d < spaceLoops.size(); ++d) {
  emitLoop(spaceLoops[d], bounds[d]);
}
for (unsigned d = 0; d < timeLoops.size(); ++d) {
  emitLoop(timeLoops[d], bounds[d]);
  emitPipelineDirective(...);
}
```

---

## 实现细节与改造点

### 7.1 新增代码文件

#### 1. `include/systolic/Analysis/ParametricSpaceTime.h`
- 定义 `ParametricSpaceTime`、`SystolicFlowDir`、`SpaceDimConfig` 等数据结构。
- 声明预设函数（`createST0`–`createST5`）。
- 验证与序列化方法。

#### 2. `include/systolic/Analysis/ScheduleTreeUtils.h`（可选但推荐）
- 轻量调度操作 API。
- 依赖距离计算。
- Band 标注与置换。

#### 3. `lib/Analysis/ParametricSpaceTime.cpp`
- 预设实现。
- 验证逻辑。
- toString/dump 等工具函数。

### 7.2 修改现有代码文件

#### 1. `include/systolic/Analysis/SpaceTimeAnalysis.h`
**改动**：
- 添加 `ParametricSpaceTime parametric;` 字段到 `SpaceTimeInfo`。
- 新增方法声明：
  ```cpp
  LogicalResult inferParametricSpaceTime(...);
  LogicalResult identifySpaceLoopCandidates(...);
  LogicalResult analyzeDataFlow(...);
  std::vector<ParametricSpaceTime> recommendConfigurations(...);
  ```

#### 2. `lib/Analysis/SpaceTimeAnalysis.cpp`
**改动**：
- **去约束**：删除 `if (loops.size() < 3 || spaceLoops.size() < 2)` 的硬校验。
- **新增依赖分析**：调用 Polymer API 获取 `isl_union_map` deps，计算距离。
- **空间维候选**：不再硬编码 `[0, 1]`，而是枚举满足"距离常量 ≤1"的维组合。
- **数据流推导**：用访问投影替换数组名分支。
  ```cpp
  // 旧：
  if (arrayName == "A") flows[memref] = HORIZONTAL;
  
  // 新：
  bool usesSpace0 = accessMapDependsOn(accessMap, spaceLoopDims[0]);
  bool usesSpace1 = accessMapDependsOn(accessMap, spaceLoopDims[1]);
  if (usesSpace0 && !usesSpace1) flows[memref] = HORIZONTAL;
  // ...
  ```

#### 3. `lib/Transforms/SystolicTransform.cpp`
**改动**：
- **消除硬编码 spacetime**：使用 `info.parametric.spaceLoopDims` 而不是 `[0, 1]`。
  ```cpp
  // 旧：
  for (unsigned i = 0; i < 2; ++i) {
    info.spaceLoops.push_back(i);
  }
  
  // 新：
  for (unsigned dim : spacetime.spaceLoopDims) {
    info.spaceLoops.push_back(dim);
  }
  ```
- **Loop 置换**：优先调用 schedule tree API（如可用），否则在 Affine 层置换（需避免写死维度）。
- **配置持久化**：调用新增方法存储 `SystolicConfiguration` 属性。

#### 4. `lib/Transforms/SystolicDataflowGeneration.cpp`
**改动**：
- **去数组名分支**：消除 `if (arrayName == "A/B/C")` 硬编码。
  ```cpp
  // 新流程
  Value memref = ...;
  auto flowIt = config.operandFlows.find(memref);
  SystolicFlowDir flow = (flowIt != config.operandFlows.end())
                             ? flowIt->second
                             : SystolicFlowDir::NONE;
  ```
- **支持 1D PE**：IO 生成逻辑按 `peArrayDims` 适配：
  ```cpp
  // 旧（假设 2D）：
  createIOL3Module(...);
  createIOL2Module(...);
  
  // 新（参数化）：
  for (unsigned d = 0; d < config.peArrayDims; ++d) {
    createIOModule(/* level based on d and array rank */, d);
  }
  ```
- **允许 N 维循环**：移除"3 重循环"假设，动态生成 IO 模块树。

#### 5. `lib/Transforms/SystolicDataflowToHLS.cpp`
**改动**：
- **动态循环生成**：
  ```cpp
  // 旧：
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      for (int k = 0; k < K; ++k) { /* PE body */ }
    }
  }
  
  // 新：
  for (unsigned s = 0; s < config.spaceLoopDims.size(); ++s) {
    emitLoopNest(config.spaceLoopDims[s], config.tileSizes[...]);
  }
  for (unsigned t = 0; t < config.timeLoopDims.size(); ++t) {
    emitLoopNest(config.timeLoopDims[t], config.tileSizes[...]);
    emitPipelineDirective(config.timeDimConfig.pipelineII);
  }
  ```
- **PE 模块模板**：按 `peArrayDims` 配置循环范围，不再假设 2D。

#### 6. `tools/systolic-translate/systolic-translate.cpp`
**改动**：
- **序列化维度无关**：
  ```cpp
  // 旧（假设 3D）：
  for (int bb = 0; bb < M; bb++) {
    for (int cc = 0; cc < N; cc++) {
      for (int dd = 0; dd < K; dd++) { /* ... */ }
    }
  }
  
  // 新（动态）：
  for (unsigned d = 0; d < config.spaceLoopDims.size(); ++d) {
    emitSerializedLoops(/* based on bounds and config */);
  }
  ```
- **IO 模块生成**：按 `tileSizes` 与 `arrayPartFactors` 而不是固定 3D 大小。

### 7.3 配置属性定义（`include/systolic/Dialect/SystolicOps.td`）

新增 Attr 定义（如使用 ODS）：
```td
def SystolicConfiguration : MLIR_Attr<...> {
  let mnemonic = "systolic.config";
  let parameters = (ins
    ArrayRefParameter<"unsigned">:$spaceLoopDims,
    ArrayRefParameter<"unsigned">:$timeLoopDims,
    ArrayRefParameter<"int64_t">:$tileSizes,
    "unsigned":$peArrayDims,
    ...
  );
}
```

或在 C++ 中使用 `DictionaryAttr` + helper 序列化。

### 7.4 测试与验证

#### 现有测试保持
- `test/matmul_test.mlir`（ST3 MM）继续通过。

#### 新增测试
1. **ST0 测试**（1D PE，行向）：
   - 输入：3D 循环（i, j, k）。
   - 配置：`spaceLoops=[0], timeLoops=[1,2]`。
   - 验证：PE 阵列为 1D；数据流正确。

2. **ST4 测试**（2D PE，权重静止）：
   - 输入：3D MM (i, j, k)。
   - 配置：`spaceLoops=[0,2], timeLoops=[1]`。
   - 验证：space=[i,k], time=[j]；B 为 PE-local。

3. **4D 循环测试**（MTTKRP 片段）：
   - 输入：4D 循环（i, j, k, l）。
   - 配置：选择 2D space，2D time（或含 reduction）。
   - 验证：IO/PE 生成正确；无硬编码假设失败。

---

## 渐进式落地计划

### 第一阶段：ParametricSpaceTime 基础（Week 1–2）
**目标**：引入参数化数据结构，保持 ST3 现有行为不变。

**任务**：
1. 创建 `ParametricSpaceTime.h/cpp`，定义数据结构与预设。
2. 修改 `SpaceTimeAnalysis.h/cpp`：
   - 添加 `parametric` 字段。
   - 实现 `inferParametricSpaceTime()`（初版可简化为选 ST3）。
   - 实现 `analyzeDataFlow()`（访问投影，去数组名）。
3. 修改 `SystolicTransform.cpp`：
   - 消费 `parametric`（初版仍选 ST3）。
   - 在 FuncOp 属性中存储 `SystolicConfiguration`。
4. 修改 `SystolicDataflowGeneration.cpp`：
   - 消除数组名分支，改用 `operandFlows`。
5. 测试：现有 ST3 MM 测试通过；新增数据流推导验证。

**预期产出**：
- `ParametricSpaceTime` 支持表达所有 spacetime 配置。
- 数据流推导去硬编码，支持任意数组集合。
- ST3 MM 测试仍通过（无回归）。

### 第二阶段：全 ST0–ST5 支持（Week 3–4）
**目标**：实现所有 spacetime 配置的自动选择与生成。

**任务**：
1. 扩展 `SpaceTimeAnalysis`：
   - 实现 `identifySpaceLoopCandidates()`：枚举合法空间维。
   - 实现 `recommendConfigurations()`：生成 ST0–ST5 候选。
2. 扩展 `SystolicTransform`：
   - 支持用户指定 spacetime 类型（CLI/属性）。
   - 根据选定配置执行 loop 置换（调度树优先）。
3. 适配 1D PE：
   - `SystolicDataflowGeneration`：支持 `peArrayDims=1` 的 IO 生成。
   - `SystolicDataflowToHLS`：1D PE 的循环/双缓冲模板。
   - `systolic-translate`：1D 序列化与循环展开。
4. 测试：
   - 新增 ST0 MM 测试。
   - 新增 ST4 MM 测试。
   - 验证所有 ST 配置编译通过与数据正确性。

**预期产出**：
- 支持 ST0–ST5 的全参数化管线。
- 1D/2D PE 通用生成。
- 3 个以上 spacetime 配置通过编译与测试。

### 第三阶段：多 Kernel 与高维循环（Week 5）
**目标**：验证通用性，扩展到 MTTKRP 等多维/多 kernel 场景。

**任务**：
1. 扩展测试：
   - 4D 循环片段（MTTKRP）。
   - 验证约化维处理（独立于 time 维）。
   - 测试不同流向的混合（HORIZONTAL + VERTICAL）。
2. 调度树优化（如时间允许）：
   - 迁移 loop 置换到调度树层操作。
   - 完善配置的多 pass 复用与重入。
3. 代码生成优化：
   - SIMD 向量化配置。
   - 精细化流通道与缓冲大小。

**预期产出**：
- 通过 4+ 维循环测试。
- MTTKRP/CNN 片段生成正确 HLS。
- 文档与示例完善。

### 第四阶段：调度树与配置系统（可选）
**目标**：完善调度操作与配置管理系统。

**任务**：
1. 实现 `ScheduleTreeUtils`：轻量 band 操作 API。
2. 将 loop 置换彻底迁移到调度树层。
3. 完善配置持久化与 CLI 集成。
4. 性能调优与文档。

---

## 附录：AutoSA 源码要点

### A.1 关键文件与函数映射

| AutoSA 模块 | 文件 | 核心函数 | MLIR 对应 |
|-----------|------|--------|---------|
| **Schedule** | `autosa_schedule_tree.cpp` | `extract_node_band_prop`, `loop_interchange_at_node`, `get_dep_dis_at_node` | `SpaceTimeAnalysis` + 调度 API |
| **Space-Time** | `autosa_trans.cpp/.h` | `sa_space_time_transform_at_dim`, `sa_space_time_loop_setup` | `SystolicTransform` |
| **Config** | `autosa_common.cpp` | `extract_sa_sizes`, `read_*_tile_sizes` | `SystolicConfiguration` + 属性 |
| **Dataflow** | `autosa_comm.cpp` | (隐含逻辑) | 访问投影分析 + `SystolicDataflowGeneration` |
| **Codegen** | `autosa_xilinx_hls_c.cpp` | (HLS 生成) | `systolic-translate` |

### A.2 依赖距离检查的 AutoSA 代码示例

```c
// 来自 autosa_schedule_tree.cpp: get_dep_dis_at_node
isl_vec *get_dep_dis_at_node(__isl_keep isl_basic_map *dep, 
                             __isl_keep isl_schedule_node *band)
{
  // 对 band 的每个成员维度，计算依赖在该维的距离值
  int band_w = isl_schedule_node_band_n_member(band);
  isl_vec *dep_dis = isl_vec_zero(isl_basic_map_get_ctx(dep), band_w);
  
  for (int i = 0; i < band_w; i++) {
    // 获取部分调度（该维的调度函数）
    isl_union_pw_aff *p_sc_hyp = isl_multi_union_pw_aff_get_union_pw_aff(p_sc, i);
    
    // 分别获取依赖源与目标在该维的调度值
    isl_pw_aff *src_sc = ..., *dest_sc = ...;
    
    // 计算距离 = dest_sc - src_sc
    isl_pw_aff *dis_sc = isl_pw_aff_sub(dest_sc, src_sc);
    
    // 求值（期望为常数）
    isl_val *val = isl_pw_aff_eval(dis_sc, ...);
    dep_dis = isl_vec_set_element_val(dep_dis, i, val);
  }
  
  return dep_dis;
}
```

**MLIR 对应实现**：在 `SpaceTimeAnalysis::computeDependenceDistance()` 中调用 Polymer ISL API，获取依赖距离向量。

### A.3 数据流推导的隐含逻辑

虽然 AutoSA 代码中未显式实现"数据流推导"函数，但逻辑散布在：
- `autosa_comm.cpp`：数组分组与 IO 生成时，隐含使用访问模式判定流向。
- `autosa_codegen.cpp`：生成 PE 与 IO 连接时，根据流向决定 FIFO/寄存器类型。

**推荐的 MLIR 提炼**：
```cpp
// 伪代码
SystolicFlowDir inferFlowDirection(
    const AffineMap &accessMap,
    const SmallVectorImpl<unsigned> &spaceLoopDims) {
  
  // 检查 accessMap 是否依赖各 space 维
  bool usesSpace0 = accessMap dependsOnIterator spaceLoopDims[0];
  bool usesSpace1 = (spaceLoopDims.size() > 1) && 
                    accessMap dependsOnIterator spaceLoopDims[1];
  
  if (usesSpace0 && !usesSpace1) return HORIZONTAL;
  if (!usesSpace0 && usesSpace1) return VERTICAL;
  if (usesSpace0 && usesSpace1) return NONE;   // PE-local
  return NONE;  // broadcast/scalar
}
```

### A.4 配置参数化示例（AutoSA union_map）

AutoSA 使用 ISL 的 union_map 存储配置，例如：
```
sizes = { kernel[0] -> space_time[3] : 0 <= 3 < 6 };
sizes = { kernel[0] -> array_part[4, 2] : ... };
sizes = { kernel[0] -> latency[8] : ... };
```

MLIR 对应实现可用 `DictionaryAttr` 或自定义 `SystolicConfigurationAttr`：
```cpp
// 示例：使用 DictionaryAttr
auto config = DictionaryAttr::get(ctx, {
  {StringAttr::get(ctx, "spaceLoopDims"), 
   ArrayAttr::get(ctx, {IntegerAttr::get(i32, 0), IntegerAttr::get(i32, 1)})},
  {StringAttr::get(ctx, "timeLoopDims"),
   ArrayAttr::get(ctx, {IntegerAttr::get(i32, 2)})},
  {StringAttr::get(ctx, "tileSizes"),
   ArrayAttr::get(ctx, {/* tile sizes */})},
  ...
});
func.setAttr("systolic.configuration", config);
```

---

## 总结

本技术路线通过以下关键改进，使 mlir-systolic 从"ST3+MM 硬编码"升级为"通用多 spacetime + 多 kernel 参数化"：

1. **引入 `ParametricSpaceTime`**：统一表示所有 spacetime 配置，打通 Analysis/Transform/Codegen 管线。
2. **多面体分析驱动**：利用 Polymer/ISL 进行依赖检查与候选空间维搜索，而非硬编码假设。
3. **访问投影数据流**：去数组名依赖，支持任意 kernel 与访问模式。
4. **维度无关的代码生成**：Dataflow/HLS/Translate 逻辑参数化，支持 1D/2D/N 维。
5. **渐进式落地**：阶段化实现，第一阶段保持 ST3 回归，后续逐步扩展至全 spacetime 与多 kernel。

该路线既遵循 AutoSA 的核心思路（多面体 + spacetime 变换 + 数据流），又充分利用 MLIR 的中间表示与属性系统优势，适合在开源项目中长期维护与扩展。

