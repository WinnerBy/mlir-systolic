# mlir-systolic 后续工作规划

> **创建时间**: 2026-01-06  
> **范围**: 技术方案、实施计划、里程碑

---

## 一、整体路线图

### 时间线概览

```
2026-01 (本月)
  Week 1: 文档清理 + 代码分析 ✓ (当前)
  Week 2: AutoSA 代码分析 + Spacetime 重构设计
  Week 3-4: Spacetime 参数化实现

2026-02
  Week 1-2: ST0-ST5 全部实现
  Week 3-4: Kernel 通用化

2026-03
  Week 1-2: 多 kernel 支持 (MM, MTTKRP, CNN)
  Week 3-4: 硬件验证和优化

2026-04-05
  集成测试、性能调优、文档完善
```

### 成功标准

| 里程碑 | 目标 | 截止 | 指标 |
|--------|------|------|------|
| M1: Spacetime 参数化 | ST0-ST5 都能编译 | 2026-01-31 | 6/6 配置通过编译 |
| M2: Kernel 通用化 | 支持 N 维循环 | 2026-02-28 | 3+ kernel 支持 |
| M3: 代码生成 | 生成正确的 HLS | 2026-03-31 | PIPELINE 与 AutoSA 匹配 ±2 |
| M4: 硬件验证 | 实际能运行 | 2026-04-30 | 通过板卡测试 |

---

## 二、Phase 1: Spacetime 参数化 (5-7 天)

### 2.1 目标

完全参数化 spacetime 配置，支持 ST0-ST5，同时保持现有 ST3 的正确性。

### 2.2 详细计划

#### Day 1-2: 数据结构设计和重构

**任务 1: 定义参数化 SpaceTime 数据结构**

创建新文件: `include/systolic/Analysis/ParametricSpaceTime.h`

```cpp
namespace mlir::systolic {

/// Space-time 配置的参数化表示
struct ParametricSpaceTime {
  /// 空间循环的维度索引 (如 [0,1] 表示 [i,j])
  SmallVector<unsigned, 3> spaceLoopDims;
  
  /// 时间循环的维度索引 (如 [2] 表示 [k])
  SmallVector<unsigned, 3> timeLoopDims;
  
  /// 每个维度的名称 (用于调试)
  SmallVector<StringRef, 3> dimNames;
  
  /// PE 阵列维度数 (通常等于 spaceLoopDims.size())
  unsigned peArrayDims;
  
  /// 是否包含约化循环
  bool hasReductionLoop;
  
  /// 数据流方向 (operand -> flow direction)
  DenseMap<Value, SystolicFlowDir> operandFlows;
  
  /// 验证此配置是否有效
  bool isValid() const;
  
  /// 转换为字符串表示 (如 "ST3")
  std::string toSpacetimeTypeString() const;
  
  /// 调试输出
  void dump() const;
};

/// 预定义的 spacetime 配置
namespace presets {
  ParametricSpaceTime createST0();  // [i]
  ParametricSpaceTime createST1();  // [j]
  ParametricSpaceTime createST2();  // [k]
  ParametricSpaceTime createST3();  // [i,j]
  ParametricSpaceTime createST4();  // [i,k]
  ParametricSpaceTime createST5();  // [j,k]
}

} // namespace mlir::systolic
```

**任务 2: 更新 SpaceTimeInfo**

修改 `include/systolic/Analysis/SpaceTimeAnalysis.h`:

```cpp
struct SpaceTimeInfo {
  // ... 现有字段 ...
  
  /// 新增: 参数化表示
  ParametricSpaceTime parametric;
  
  /// 方法: 推导参数化 spacetime
  LogicalResult inferParametricSpaceTime();
  
  /// 方法: 推荐 spacetime 配置
  static std::vector<ParametricSpaceTime> recommendConfigurations(
      AffineForOp outerLoop,
      unsigned maxSpaceDims = 2);
};
```

**任务 3: 创建配置管理器**

创建新文件: `include/systolic/Analysis/ConfigurationManager.h`

```cpp
/// 管理和验证 systolic 配置
class ConfigurationManager {
public:
  /// 从 SpaceTimeInfo 创建配置
  SystolicConfiguration createConfiguration(
      const SpaceTimeInfo &info,
      const KernelInfo &kernel);
  
  /// 验证配置
  bool validateConfiguration(const SystolicConfiguration &config);
  
  /// 获取配置推荐
  std::vector<SystolicConfiguration> recommendConfigurations(
      const KernelInfo &kernel);
  
  /// 将配置存储到函数属性
  void storeConfiguration(FuncOp func, const SystolicConfiguration &config);
  
  /// 从函数属性读取配置
  Optional<SystolicConfiguration> loadConfiguration(FuncOp func);
};
```

#### Day 3: SpaceTimeAnalysis 重构

**任务 1: 改进空间循环选择**

修改 `lib/Analysis/SpaceTimeAnalysis.cpp`:

```cpp
/// 通用的空间循环选择算法
/// 参数化地选择指定数量的空间循环
LogicalResult selectSpaceLoopsParametric(
    SpaceTimeInfo &info,
    unsigned numSpaceLoops,  // 1 或 2
    unsigned numTimeLoops);   // 可变
```

**原理**:
1. 遍历所有可能的循环组合
2. 对每个组合进行依赖距离分析
3. 选择满足 space-time 条件的组合 (距离 <= 1)
4. 返回所有可行的配置

**任务 2: 数据流推导**

实现通用的数据流分析:

```cpp
/// 根据循环访问模式推导数据流方向
LogicalResult inferDataFlows(
    AffineForOp outerLoop,
    const SmallVector<unsigned> &spaceLoops,
    const SmallVector<unsigned> &timeLoops,
    DenseMap<Value, SystolicFlowDir> &flows);
```

**原理**:
- 对每个 operand (memref), 分析它使用的循环维度
- 根据使用的维度和 spacetime 类型推导流向
- 例如:
  - Operand 仅使用 spaceLoop[0] → 沿 spaceLoop[1] 传递 (垂直)
  - Operand 仅使用 spaceLoop[1] → 沿 spaceLoop[0] 传递 (水平)
  - 等等

#### Day 4: SystolicTransform 重构

**任务**: 改进 `lib/Transforms/SystolicTransform.cpp`

**具体改动**:

1. **使用参数化 space-time**
   ```cpp
   // 删除硬编码
   - for (unsigned i = 0; i < 2; ++i) {
   + // 动态选择空间循环
   + if (failed(selectSpaceLoopsParametric(info, numSpaceDims, ...))) {
   +   return failure();
   + }
   ```

2. **支持参数化的循环置换**
   ```cpp
   // 根据选择的 spaceLoops 进行循环重排
   // 例如 spaceLoops = [0, 2] 时，需要置换到前两位
   LogicalResult reorderLoops(
       AffineForOp outerLoop,
       const SmallVector<unsigned> &targetOrder);
   ```

3. **参数化的多级分块**
   ```cpp
   // 分块大小由 kernel 大小自动推导
   // 不再硬编码为特定值
   LogicalResult applyMultiLevelTiling(
       AffineForOp outerLoop,
       const SystolicConfiguration &config);
   ```

#### Day 5: SystolicDataflowGeneration 重构

**任务**: 改进 `lib/Transforms/SystolicDataflowGeneration.cpp`

**具体改动**:

1. **参数化的数据流分组**
   ```cpp
   // 根据实际 spacetime 分组数组引用
   // 不再假设 A/B/C 的流向
   LogicalResult groupArrayReferencesParametric(
       func::FuncOp func,
       const ParametricSpaceTime &spacetime,
       std::vector<ArrayRefGroup> &groups);
   ```

2. **参数化的 PE 阵列创建**
   ```cpp
   // PE 阵列维度由 spaceLoopDims.size() 决定
   // 不再硬编码为 2D
   LogicalResult createPEArray(
       OpBuilder &builder,
       const ParametricSpaceTime &spacetime,
       unsigned peCountPerDim);
   ```

3. **参数化的 IO 模块生成**
   ```cpp
   // IO 模块结构由 spacetime 配置决定
   LogicalResult generateIOModules(
       OpBuilder &builder,
       const SystolicConfiguration &config);
   ```

#### Day 6: 代码生成更新

**任务**: 更新 HLS 代码生成

修改 `tools/systolic-translate/systolic-translate.cpp`:

1. **参数化的 PE 模块生成**
   ```cpp
   // PE 模块循环嵌套根据 spacetime 动态生成
   // ST3 时: 3 层循环 (i, j, k)
   // ST0 时: 2 层循环 (i, k)
   // 等等
   ```

2. **参数化的 IO 模块生成**
   ```cpp
   // IO 模块维度和层次根据 spacetime 确定
   ```

3. **参数化的 serialize 循环**
   ```cpp
   // serialize 循环边界动态计算，而非硬编码
   ```

#### Day 7: 测试和验证

**任务 1: 单元测试**
- 测试每个 spacetime 类型的代码生成
- 验证生成的代码结构正确

**任务 2: 集成测试**
- ST0 生成的代码能编译
- ST1 生成的代码能编译
- ... ST5

**任务 3: 对比测试**
- 与 AutoSA 输出对比
- 验证 PIPELINE 数量相同

### 2.3 预期输出

完成此 phase 后:

```
✅ Spacetime 参数化完成
  - ST0 能够编译生成
  - ST1 能够编译生成
  - ST2 能够编译生成
  - ST3 能够编译生成（保持现有正确性）
  - ST4 能够编译生成
  - ST5 能够编译生成

✅ 代码生成改进
  - PE 模块维度动态确定
  - IO 模块结构动态生成
  - PIPELINE 数量保持一致

✅ 配置管理改进
  - 清晰的配置流
  - 配置验证机制
  - 便于调试的配置输出
```

---

## 三、Phase 2: Kernel 通用化 (5-7 天)

### 3.1 目标

使系统能够处理任意循环层数的 kernel，不仅限于 3 层。

### 3.2 关键设计

#### 概念 1: Kernel 检测

```cpp
/// 自动检测 kernel 的特征
struct KernelInfo {
  unsigned numLoops;                    // 循环层数
  SmallVector<StringRef> loopNames;     // 循环变量名
  SmallVector<Value> inductionVars;     // 归纳变量
  
  // 访问模式分析
  DenseMap<Value, AccessPattern> accessPatterns;  // memref -> 访问方式
  
  // 循环依赖关系
  DenseMap<unsigned, SmallVector<unsigned>> dependenceCarriedBy;
  
  // 自动检测
  static LogicalResult detectFromFunction(
      func::FuncOp func,
      KernelInfo &result);
};
```

#### 概念 2: 访问模式

```cpp
enum class AccessPattern {
  SIMPLE,          // 直接 IV: A[i], A[i,j]
  LINEAR,          // 线性组合: A[i+k], A[i*2+j]
  NONLINEAR,       // 非线性: A[i/2], A[i*j]
  REDUCTION_VAR,   // 约化变量: sum 中的中间变量
  COMPLEX          // 其他复杂情况
};

struct MemrefAccess {
  Value memref;
  AccessPattern pattern;
  SmallVector<unsigned> usedLoopDims;  // 使用的循环维度
  bool isWritten;
  bool isRead;
};
```

### 3.3 实施步骤

#### Step 1: Kernel 分析框架

创建 `lib/Analysis/KernelAnalysis.cpp`:

```cpp
/// 检测循环层数
unsigned detectLoopNesting(AffineForOp outerLoop);

/// 分析访问模式
LogicalResult analyzeAccessPatterns(
    AffineForOp outerLoop,
    DenseMap<Value, AccessPattern> &patterns);

/// 推导循环依赖
LogicalResult analyzeDependences(
    AffineForOp outerLoop,
    DenseMap<unsigned, SmallVector<unsigned>> &carriedBy);

/// 识别循环角色 (时间/空间/其他)
LogicalResult classifyLoopRoles(
    AffineForOp outerLoop,
    SmallVectorImpl<LoopRole> &roles);
```

#### Step 2: 通用的空间循环选择

改进算法以支持 N 维循环:

```cpp
/// 从 N 维循环中选择 numSpaceLoops 个作为空间循环
/// 考虑依赖距离约束
LogicalResult selectSpaceLoopsFromN(
    const KernelInfo &kernel,
    const SmallVector<DependenceDistance> &distances,
    unsigned numSpaceLoops,
    SmallVectorImpl<unsigned> &selectedDims);
```

**算法**:
1. 候选: 所有距离 <= 1 的循环
2. 约束: 
   - 空间循环必须可以置换到前 numSpaceLoops 位
   - 不能选择约化循环作为空间循环 (通常)
3. 评分: 
   - 偏好提前出现的循环（cache locality）
   - 偏好循环大小较小的循环（PE 数量）
4. 贪心选择: 选择评分最高的组合

#### Step 3: 通用的代码生成

创建代码生成器基类:

```cpp
/// 基础代码生成器，支持 N 维循环
class CodeGeneratorBase {
protected:
  const KernelInfo &kernel;
  const SystolicConfiguration &config;
  
  /// 生成 PE 模块 (N 维)
  virtual LogicalResult generatePEModule(
      OpBuilder &builder,
      const SmallVector<unsigned> &spaceLoops) = 0;
  
  /// 生成 IO 模块 (N 维)
  virtual LogicalResult generateIOModules(
      OpBuilder &builder) = 0;
  
  /// 处理循环嵌套 (支持任意层数)
  LogicalResult walkLoopNest(
      AffineForOp outerLoop,
      std::function<void(SmallVector<AffineForOp> &)> callback);
};
```

#### Step 4: 特化实现

为不同 kernel 类型提供特化:

```cpp
/// MatMul 特化 (现有代码基础)
class MatMulCodeGenerator : public CodeGeneratorBase {
  LogicalResult generatePEModule(...) override;
  LogicalResult generateIOModules(...) override;
};

/// MTTKRP 特化
class MTTKRPCodeGenerator : public CodeGeneratorBase {
  LogicalResult generatePEModule(...) override;
  LogicalResult generateIOModules(...) override;
  LogicalResult handleReductionVar(...);
};

/// CNN 特化
class CNNCodeGenerator : public CodeGeneratorBase {
  LogicalResult generatePEModule(...) override;
  LogicalResult generateIOModules(...) override;
};
```

### 3.4 预期里程碑

```
✅ Kernel 分析框架
  - 自动检测循环层数
  - 分析访问模式
  - 识别循环依赖

✅ 通用空间循环选择
  - 支持 3+ 维循环
  - 支持多种 kernel

✅ 参数化代码生成
  - PE 模块适配任意维度
  - IO 模块动态生成
  - HLS 代码正确生成
```

---

## 四、Phase 3: 多 Kernel 支持 (3-5 天)

### 4.1 目标

支持至少 MM, MTTKRP, CNN 三种 kernel。

### 4.2 Kernel 特点分析

#### MM (Matrix Multiply)
- 3 个循环: i, j, k
- 3 个输入: A[i,k], B[k,j], C[i,j]
- 访问模式: 简单线性
- 依赖: k 循环内有约化

#### MTTKRP (Matricized Tensor Times Kronecker Root)
- 4+ 个循环: i, j, l, k (等)
- 多个输入: Y[i,j], X[i,l,k], A[l], B[k,j]
- 访问模式: 可能有非线性 (如 X[...] 中的下标)
- 依赖: 复杂

#### CNN (Convolutional Neural Network)
- 5-7 个循环: batch, channel_out, h_out, w_out, channel_in, h_in, w_in
- 访问模式: 带有 stride 和 padding 的非线性
- 特殊性: 可能有多个独立的卷积操作

### 4.3 实施方案

#### 支持 MTTKRP

修改 `lib/Analysis/KernelAnalysis.cpp`:

```cpp
/// 识别约化变量
LogicalResult detectReductionVariables(
    AffineForOp outerLoop,
    SmallVectorImpl<Value> &reductionVars);

/// 分析多输入多输出
LogicalResult analyzeIOPattern(
    AffineForOp outerLoop,
    SmallVectorImpl<Value> &inputs,
    SmallVectorImpl<Value> &outputs,
    SmallVectorImpl<Value> &intermediates);
```

#### 支持 CNN

创建专化处理:

```cpp
/// 检测卷积模式
bool isConvolutionKernel(const KernelInfo &kernel);

/// 提取卷积参数
struct ConvolutionParams {
  unsigned batchSize;
  unsigned inChannels, outChannels;
  unsigned inHeight, inWidth;
  unsigned outHeight, outWidth;
  unsigned kernelSize, stride, padding;
};

LogicalResult extractConvolutionParams(
    AffineForOp outerLoop,
    ConvolutionParams &params);
```

### 4.4 预期结果

完成此 phase 后，系统应能处理:

```
✅ MM kernel: 任意 spacetime 配置
✅ MTTKRP kernel: 基本 spacetime 配置
✅ CNN kernel: 基本 spacetime 配置
✅ 通用 N 维循环 kernel: 基本支持
```

---

## 五、Phase 4: 硬件验证与优化 (待后续)

### 5.1 验证清单

- [ ] 代码在 Vitis HLS 中编译通过
- [ ] 综合后时序能够满足
- [ ] 上板后功能正确
- [ ] 性能指标达到预期

### 5.2 优化方向

1. **写时重排验证**: 在 U250 上测试性能收益
2. **数据复用优化**: L2 层复用实现
3. **自动调优**: 参数空间搜索
4. **与 Allo 融合**: 探索集成方案

---

## 六、技术决策

### 决策 1: AutoSA 代码参考

**行动**: 获取 AutoSA 源码，深入分析:
- `sa.cpp` 的主处理流程
- `autosa_utils.cpp` 的工具函数
- `autosa_*.cpp` 的具体实现

**预期收获**:
- 理解 spacetime 选择的精确算法
- 学习如何处理不同 kernel
- 参考参数推荐策略

### 决策 2: 配置系统

**选择**: 使用 MLIR 属性存储配置，而非函数属性字符串

**优势**:
- 类型安全
- 支持验证
- 便于调试

**实施**: 定义自定义 MLIR Attribute 类

### 决策 3: 代码生成策略

**选择**: 模板 + 参数化，而非 codegen 库

**优势**:
- 不引入额外依赖
- 便于调试 (可视化生成过程)
- 易于定制

**实施**: 在 EmitHLSCpp 中增加参数驱动的代码生成

---

## 七、风险和缓解措施

| 风险 | 概率 | 影响 | 缓解措施 |
|------|------|------|---------|
| AutoSA 代码难以理解 | 中 | 中 | 前期深入学习，联系 AutoSA 作者 |
| 参数化重构引入 bug | 中 | 高 | 充分测试，保持向后兼容 |
| Kernel 检测不准确 | 低 | 中 | 支持手动标注，提供调试模式 |
| 硬件验证失败 | 低 | 高 | 提前与硬件团队沟通，准备备选方案 |

---

## 八、成功指标和验收

### 代码指标

- [ ] 所有 6 种 spacetime (ST0-ST5) 都能生成代码
- [ ] 生成的代码能通过 Vitis HLS 编译
- [ ] PIPELINE 数量与 AutoSA 一致 (误差 < 5%)
- [ ] 代码行数差异 (考虑注释) 在 ±10% 以内

### 功能指标

- [ ] 支持 3 种 kernel (MM, MTTKRP, CNN)
- [ ] 支持 3+ 维循环嵌套
- [ ] 自动 kernel 检测准确率 > 90%

### 性能指标

- [ ] 与 AutoSA 性能相当 (误差 < 15%)
- [ ] 写时重排实现并通过硬件验证

### 质量指标

- [ ] 代码覆盖率 > 80%
- [ ] 无编译警告
- [ ] 文档完整度 > 90%

---

## 九、最终总结

### 关键成果

1. **从特例到通用**
   - 从 spacetime=3, MM kernel
   - 到 spacetime 参数化，多 kernel 支持

2. **从硬编码到数据驱动**
   - 从固定的参数和假设
   - 到配置驱动的代码生成

3. **从原型到生产**
   - 从 proof-of-concept 实现
   - 到可维护、可扩展的系统

### 后续可能的工作

1. 自动调优框架 (AutoSA 的功能)
2. 与 Allo 的集成
3. 支持更多 AI/HPC kernel
4. 多芯片验证

