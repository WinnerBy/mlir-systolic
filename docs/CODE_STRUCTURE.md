# mlir-systolic 代码结构说明

> **最后更新**: 2026-01-06  
> **用途**: 理解代码组织和已知问题

---

## 目录

1. [核心模块](#核心模块)
2. [代码组织](#代码组织)
3. [Pass 流程](#pass-流程)
4. [已知问题和 FIXME](#已知问题和-fixme)
5. [配置流](#配置流)

---

## 核心模块

### 1. lib/Transforms/ - 转换 Pass

#### SystolicTransform.cpp
**功能**: 主 Pass，执行 space-time 变换

**关键函数**:
- `checkLegality()` - 检查循环是否适合脉动阵列
- `analyzeDependenceDistances()` - 依赖距离分析
- `selectSpaceLoops()` - 选择空间循环
- `applyMultiLevelTiling()` - 多级分块

**输入**: Affine IR (嵌套循环)  
**输出**: 
- 转换后的 Affine IR
- 配置信息存储为函数属性

**✅ 当前状态**:
- 已实现参数化空间循环选择（通过 `selectSpaceLoopsParametric()`）
- 支持 ST0-ST5 配置（通过 `ParametricSpaceTime::createFromMode()`）
- 向后兼容传统模式（`selectSpaceLoops()`）

#### SystolicDataflowGeneration.cpp
**功能**: 生成 SystolicDataflow Dialect 操作

**关键函数**:
- `analyzeArrayReferences()` - 分组数组引用
- `classifyArrayGroups()` - 分类为 IO/PE/Drain
- `generateIOModules()` - 生成 IO 模块
- `generatePEArray()` - 生成 PE 阵列
- `generateDrainModules()` - 生成 Drain 模块

**输入**: 
- 转换后的 Affine IR
- 函数属性中的配置

**输出**: SystolicDataflow Dialect 操作

**✅ 当前状态**:
- 使用 ParametricSpaceTime 框架进行参数化分析
- 数据流方向通过 `analyzeOperandFlowsParametric()` 自动推导
- 支持不同 spacetime 配置的 IO 层级分析

#### SystolicDataflowToHLS.cpp
**功能**: 降低 SystolicDataflow 到 HLS Dialect

**状态**: 框架完成，具体实现未完整

**⚠️ 已知问题**:
- 循环结构假设固定为 3 层
- 双缓冲逻辑未实现
- 循环体迁移未完成

---

### 2. lib/Analysis/ - 分析模块

#### SpaceTimeAnalysis.cpp
**功能**: Space-time 映射分析

**关键数据结构**:
```cpp
struct SpaceTimeInfo {
  SmallVector<unsigned> spaceLoops;    // 空间循环索引
  SmallVector<unsigned> timeLoops;     // 时间循环索引
  DenseMap<Value, SystolicFlowDir> operandFlows;  // 数据流
};
```

**关键函数**:
- `analyzeSpaceTime()` - 主分析函数
- `computeDependenceDistances()` - 依赖距离计算
- `selectSpaceLoops()` - 空间循环选择
- `analyzeDataFlow()` - 数据流分析

#### SystolicConfig.h
**功能**: 配置参数定义

**关键数据结构**:
```cpp
struct SystolicConfig {
  unsigned numPE_I, numPE_J;            // PE 数量
  SmallVector<int64_t> arrayPart;       // 分块大小
  SmallVector<int64_t> latency;         // 延迟隐藏
  unsigned simdWidth;                    // SIMD 宽度
  // ...
};
```

#### PolymerAnalysis.cpp
**功能**: Polymer/ISL 集成

**用途**: 精确的多面体依赖分析

#### WriteTimeReorderingAnalysis.cpp
**功能**: 写时重排分析

**⚠️ 已知问题**:
- 仅支持 3 维数组
- 重排信息未应用到代码生成

---

### 3. lib/Dialect/ - Dialect 定义

#### SystolicDataflow Dialect
**文件**: `lib/Dialect/SystolicDataflow/`

**关键操作**:
- `systolic.dataflow.io.module` - IO 模块
- `systolic.dataflow.pe.array` - PE 阵列
- `systolic.dataflow.drain.module` - Drain 模块
- `systolic.dataflow.double.buffer` - 双缓冲

**状态**: 操作定义完整，语义部分实现

---

### 4. tools/systolic-translate/ - 代码生成

#### systolic-translate.cpp
**功能**: MLIR 到 HLS C++ 翻译

**主要函数**:
- `emitHLSCpp()` - 主代码生成
- `emitIOModule()` - IO 模块生成
- `emitPEModule()` - PE 模块生成
- `emitDrainModule()` - Drain 模块生成

**⚠️ 已知问题**:
- 数组维度硬编码为 3D
- serialize 循环边界依赖 spacetime=3
- 模块结构固定

---

## 代码组织

```
mlir-systolic/
├── include/systolic/          # 头文件
│   ├── Dialect/              # Dialect 定义
│   │   ├── HLS/             # HLS Dialect (未完成)
│   │   └── SystolicDataflow/ # SystolicDataflow Dialect
│   ├── Analysis/             # 分析接口
│   │   ├── SpaceTimeAnalysis.h
│   │   ├── SystolicConfig.h
│   │   └── ...
│   └── Transforms/           # Pass 声明
│       └── Passes.h
│
├── lib/                       # 实现
│   ├── Dialect/              # Dialect 实现
│   ├── Analysis/             # 分析实现
│   ├── Transforms/           # Pass 实现
│   └── Translation/          # 代码生成
│
├── tools/                     # 工具
│   ├── systolic-opt/         # MLIR 优化工具
│   └── systolic-translate/   # MLIR 翻译工具
│
└── test/                      # 测试
    ├── matmul/               # MatMul 测试
    ├── reference-samples/    # AutoSA 参考
    └── *.sh                  # 测试脚本
```

---

## Pass 流程

### 典型的转换流程

```
输入: Affine IR (nested loops)
  ↓
┌─────────────────────────────────────┐
│ 1. SystolicTransform Pass          │
│   - 合法性检查                      │
│   - 依赖分析 (Polymer)             │
│   - 空间循环选择                    │
│   - 循环置换和分块                  │
│   - 配置存储 (函数属性)            │
└─────────────────────────────────────┘
  ↓
转换后的 Affine IR + 配置属性
  ↓
┌─────────────────────────────────────┐
│ 2. SystolicDataflowGeneration Pass │
│   - 读取配置                        │
│   - 数组引用分析                    │
│   - 生成 Dataflow 操作              │
│   - IO/PE/Drain 模块创建           │
└─────────────────────────────────────┘
  ↓
SystolicDataflow Dialect IR
  ↓
┌─────────────────────────────────────┐
│ 3. SystolicDataflowToHLS Pass      │
│   (可选, 未完成)                    │
│   - 降低到 HLS Dialect             │
└─────────────────────────────────────┘
  ↓
HLS Dialect IR (或直接到下一步)
  ↓
┌─────────────────────────────────────┐
│ 4. systolic-translate               │
│   - EmitHLSCpp translation          │
│   - 生成 HLS C++ 代码               │
│   - 插入 PIPELINE pragma            │
└─────────────────────────────────────┘
  ↓
输出: HLS C++ 代码 (.cpp)
```

### 命令行用法

```bash
# 完整流程
systolic-opt \
  --systolic-transform \
  --systolic-dataflow-generation \
  input.mlir -o transformed.mlir

systolic-translate \
  --emit-hls \
  transformed.mlir -o output.cpp
```

---

## 已知问题和 FIXME

### ✅ P1 (已完成) - Spacetime 参数化

**状态**: 已通过 ParametricSpaceTime 框架实现参数化支持

**实现位置**:
- `SystolicTransform.cpp`: 使用 `selectSpaceLoopsParametric()` 进行参数化选择
- `SystolicDataflowGeneration.cpp`: 使用 `ParametricSpaceTime::createFromMode()` 创建配置
- `SpaceTimeAnalysis.cpp`: 实现 `analyzeOperandFlowsParametric()` 进行参数化数据流分析

**支持范围**: ST0-ST5 全部 6 种配置

**向后兼容**: 保留传统 `selectSpaceLoops()` 作为回退机制

### 🟡 P2 (中优先级) - Kernel 泛化

#### 当前状态
- ✅ 主要支持 3-loop 矩阵乘法 (MM)
- 🟡 其他 kernel 类型 (MTTKRP, CNN, LU 等) 支持有限
- 🟡 需要实现通用的 loop body migration

**影响**: 其他 kernel 类型可能需要手动适配或等待通用实现

### 🟡 P2 (中优先级) - 配置流

#### 位置: SystolicTransform.cpp (~700-750)
```cpp
// FIXME: [config flow] 配置存储方式临时
// 需要使用 MLIR Attribute 而非字符串属性
func.setAttr("systolic.config", ...);
```

### 🟢 P3 (低优先级) - 功能完善

#### 位置: SystolicDataflowToHLS.cpp
```cpp
// TODO: 实现通用循环体迁移（当前主要支持 MM kernel）
// TODO: 完善双缓冲逻辑
```

#### 位置: WriteTimeReorderingAnalysis.cpp
```cpp
// FIXME: 当前主要支持 3D 数组，其他维度支持有限
if (arrayDims.size() != 3) {
  return failure();
}
```

---

## 配置流

### 当前流程 (有问题)

```
SpaceTimeInfo (分析结果)
  ↓ 序列化
函数属性 (字符串)
  ↓ 反序列化
SystolicConfig (重构)
  ↓ 嵌入
Dataflow 操作属性
  ↓ 读取
代码生成参数
```

**问题**:
- 多次序列化/反序列化
- 类型不安全
- 信息可能丢失
- 难以验证

### 改进方案

**使用 MLIR Attribute**:
```cpp
// 定义自定义属性
class SystolicConfigAttr : public Attribute {
  // 直接存储强类型的配置
  unsigned numPE_I, numPE_J;
  SmallVector<int64_t> arrayPart;
  // ...
};

// 存储
func.setAttr("systolic.config", 
  SystolicConfigAttr::get(context, config));

// 读取
auto configAttr = func.getAttr("systolic.config")
  .dyn_cast<SystolicConfigAttr>();
```

**优势**:
- 类型安全
- 支持验证
- 便于调试
- 标准 MLIR 方式

---

## 参数化实现状态

### 已完成

✅ **Spacetime 参数化**: 通过 ParametricSpaceTime 框架实现，支持 ST0-ST5
- 使用 `ParametricSpaceTime::createFromMode()` 创建配置
- 使用 `selectSpaceLoopsParametric()` 进行参数化选择
- 使用 `analyzeOperandFlowsParametric()` 进行参数化数据流分析

### 待完善

🟡 **Kernel 泛化**: 主要支持 3-loop MM，其他 kernel 类型支持有限
- 需要实现通用的 loop body migration
- 需要支持 N 维循环嵌套 (MTTKRP: 4+, CNN: 5+)

### 已实现的参数化框架

1. **ParametricSpaceTime 数据结构** ✅
   ```cpp
   class ParametricSpaceTime {
     SmallVector<SpaceDimConfig> spaceDimConfigs;  // 空间维度配置
     TimeDimConfig timeDimConfig;                   // 时间维度配置
     ReductionDimConfig reductionDimConfig;         // Reduction 维度配置
     DenseMap<Value, SystolicFlowDir> operandFlows; // 数据流方向
   };
   ```

2. **参数化空间循环选择** ✅
   ```cpp
   LogicalResult selectSpaceLoopsParametric(
       const SmallVectorImpl<LoopDepInfo> &depInfos,
       const ParametricSpaceTime &parametric,
       SmallVectorImpl<unsigned> &spaceLoopIndices,
       SmallVectorImpl<unsigned> &timeLoopIndices);
   ```

3. **参数化数据流分析** ✅
   ```cpp
   LogicalResult analyzeOperandFlowsParametric(
       AffineForOp outerLoop,
       const SmallVector<AffineForOp> &loops,
       const ParametricSpaceTime &parametric,
       DenseMap<Value, SystolicFlowDir> &flows);
   ```

4. **参数化代码生成** ✅
   - PE 模块维度由 `getNumSpaceDims()` 决定
   - IO 模块结构根据 spacetime 配置动态生成
   - HLS 代码根据配置生成

### 待完善的功能

参见 [ROADMAP.md](../status/ROADMAP.md) 了解后续计划

---

## 代码审查清单

提交代码前检查:

- [x] 是否支持参数化 spacetime? ✅ 已通过 ParametricSpaceTime 框架实现
- [ ] 是否支持 3+ 维循环? 🟡 主要支持 3-loop，其他维度支持有限
- [ ] 是否处理了错误情况?
- [ ] 是否有充分的单元测试?
- [ ] 是否与 AutoSA 行为一致?
- [x] 是否避免了新的硬编码? ✅ 已使用参数化框架
- [ ] 是否更新了文档?
- [ ] 是否添加了 FIXME 注释 (如果临时实现)?

---

## 相关文档

- [CODE_ISSUES_DETAILED_ANALYSIS.md](../CODE_ISSUES_DETAILED_ANALYSIS.md) - 问题详细分析
- [NEXT_STEPS_TECHNICAL_ROADMAP.md](../NEXT_STEPS_TECHNICAL_ROADMAP.md) - 技术路线图
- [../docs/project/ARCHITECTURE.md](../docs/project/ARCHITECTURE.md) - 架构设计
- [../docs/project/DEVELOPMENT_GUIDE.md](../docs/project/DEVELOPMENT_GUIDE.md) - 开发指南

