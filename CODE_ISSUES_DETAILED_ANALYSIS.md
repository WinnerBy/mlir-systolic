# mlir-systolic 代码问题详细分析

> **生成时间**: 2026-01-06  
> **范围**: 代码设计问题、实现缺陷、技术债务

---

## 一、主要设计问题

### 问题 1: Spacetime 配置硬编码

#### 1.1 问题描述

当前代码假设 spacetime = 3, 即:
- 空间循环: [i, j] (第 0、1 维)
- 时间循环: [k] (第 2 维)
- PE 阵列维度: 2D (行列)

这限制了系统只能支持这种特定的配置。

#### 1.2 具体位置和代码

**文件 1: `lib/Transforms/SystolicTransform.cpp`**

```cpp
// 行 ~185-200: 空间循环硬编码
for (unsigned i = 0; i < 2; ++i) {
  info.spaceLoops.push_back(i);  // 硬编码 [0, 1] = [i, j]
}

// 行 ~210-220: 时间循环硬编码
for (unsigned i = 2; i < loops.size(); ++i) {
  info.timeLoops.push_back(i);  // 硬编码从第 2 维开始
}
```

**问题**: 
- 没有真正的 spacetime 参数输入
- 不支持其他组合如 ST0 ([i]), ST1 ([j]), ST2 ([k]) 等

**文件 2: `lib/Transforms/SystolicDataflowGeneration.cpp`**

```cpp
// 行 ~210-230: 数据流方向硬编码
// A[i,k] -> 水平传递 (因为只使用了 i)
// B[k,j] -> 垂直传递 (因为只使用了 j)
// C[i,j] -> 本地 (因为使用了 i,j)

if (arrayName == "A") {
  flows[memref] = SystolicFlowDir::HORIZONTAL;
} else if (arrayName == "B") {
  flows[memref] = SystolicFlowDir::VERTICAL;
} else if (arrayName == "C") {
  flows[memref] = SystolicFlowDir::NONE;
}
```

**问题**:
- 依赖数组名称识别 (脆弱)
- 不支持其他 kernel 的数据流模式
- 无法自动根据 spacetime 推导数据流

**文件 3: `lib/Transforms/SystolicDataflowToHLS.cpp`**

```cpp
// 行 ~40-60: PE 模块循环结构固定为 3 层
// for (int i = ...) {
//   for (int j = ...) {
//     for (int k = ...) {
//       // 计算
//     }
//   }
// }
```

**问题**:
- 假设总是有 3 个循环
- 不支持 4 层及以上的循环嵌套
- 不支持其他 kernel

**文件 4: `tools/systolic-translate/systolic-translate.cpp`**

```cpp
// 行 ~300-350: 数组维度假设
// 假设所有数据都是 3 维的: [M][N][K]

// 生成 IO_L2_in 模块时
// for (int bb = 0; bb < M; bb++) {
//   for (int cc = 0; cc < N; cc++) {
//     for (int dd = 0; dd < K; dd++) {
```

**问题**:
- 硬编码 3 维数组结构
- serialize 循环的边界依赖 spacetime=3 的具体参数
- 无法适配其他数组大小

#### 1.3 影响范围

| 组件 | 影响 |
|------|------|
| 空间循环选择 | 仅支持 [i,j] |
| 时间循环选择 | 仅支持剩余维度 |
| PE 阵列维度 | 仅支持 2D |
| 数据流方向 | 硬编码 A/B/C 流向 |
| IO 模块生成 | 仅支持 3 维数据 |
| 代码生成 | 输出固定为 spacetime=3 格式 |

#### 1.4 解决方案

参见第三部分 "参数化重构方案"

---

### 问题 2: Kernel 特异性

#### 2.1 问题描述

代码针对矩阵乘法 (MM) kernel 设计，无法支持其他算子。

#### 2.2 具体问题

**假设 1: 三维循环结构**
- 矩阵乘法: C[i,j] += A[i,k] * B[k,j] -> 3 个循环
- MTTKRP: Y[i,j] += X[i,k,l] * A[k] * B[l,j] -> 4 个循环
- 卷积: 可能有 5+ 个循环

**假设 2: 特定的访问模式**
- MM: 两个输入一个输出
- 某些算子可能有多个输入或输出
- 访问模式复杂度可能不同

**假设 3: 循环迭代关系**
- 代码假设循环独立或有简单的依赖关系
- 某些算子可能有更复杂的重用模式

#### 2.3 具体位置

| 文件 | 问题 | 行号 |
|------|------|------|
| SystolicTransform.cpp | 假设 3 个循环 | ~140, ~270 |
| SystolicDataflowGeneration.cpp | 假设 MM 的数据流 | ~210-240 |
| SystolicDataflowToHLS.cpp | 假设 MM 的循环结构 | ~40-80 |
| systolic-translate.cpp | 假设 MM 的数组大小 | ~300-350 |

#### 2.4 解决方案

实现通用的:
1. 循环结构分析
2. 访问模式识别
3. 空间循环选择算法
4. 代码生成框架

---

### 问题 3: 配置信息流混乱

#### 3.1 问题描述

配置参数从分析到代码生成的传递方式不清晰，导致维护困难。

#### 3.2 当前流程

```
AffineForOp (输入)
    ↓
SystolicTransform Pass
    ├─ Polymer 分析
    ├─ SpaceTimeInfo 创建
    └─ 函数属性存储
         ↓
        (存储到 function attributes)
         ↓
SystolicDataflowGeneration Pass
    ├─ 从属性读取 SpaceTimeInfo
    ├─ 创建 SystolicConfig
    └─ 存储到 dataflow operations
         ↓
        (嵌入到 SystolicDataflow Dialect)
         ↓
SystolicDataflowToHLS / EmitHLSCpp
    ├─ 从 dataflow operations 读取
    └─ 生成 HLS 代码
```

#### 3.3 问题详解

**问题 A: 属性存储不规范**

```cpp
// 当前方式: 存储为字符串属性 (脆弱)
func.setAttr("systolic.spacetime.spaceLoops", 
             builder.getI32ArrayAttr(spaceLoops));

// 问题:
// - 属性名字符串容易出错
// - 类型转换手动进行，容易出 bug
// - 难以检验属性的有效性
```

**问题 B: 类型不匹配**

```
SpaceTimeInfo (分析结果)
    → 转换 → 函数属性 (字符串化)
        → 转换 → SystolicConfig (重新构造)
            → 转换 → dataflow 操作参数
                → 转换 → HLS 代码生成参数
```

每一步转换都可能丢失信息或引入错误。

**问题 C: 缺乏验证**

```cpp
// 无法验证:
// - 配置是否有效
// - 配置是否与输入 kernel 兼容
// - 配置参数是否在合理范围内
```

#### 3.4 具体位置

| 文件 | 问题 | 行号 |
|------|------|------|
| SystolicTransform.cpp | 配置存储 | ~700-750 |
| SystolicDataflowGeneration.cpp | 配置读取 | ~340-380 |
| EmitHLSCpp | 配置使用 | ~100-150 |

#### 3.5 改进方案

1. **定义正式的配置格式**
   ```cpp
   struct SystolicConfigV2 {
     // 验证方法
     bool validate() const;
     // 序列化/反序列化
     std::string serialize() const;
     static SystolicConfigV2 deserialize(const std::string &);
   };
   ```

2. **创建配置管理器**
   ```cpp
   class ConfigManager {
     bool setConfig(FuncOp func, const SystolicConfigV2 &);
     SystolicConfigV2 getConfig(FuncOp func) const;
     bool isValidFor(FuncOp func) const;
   };
   ```

3. **使用 MLIR 属性**
   - 定义自定义 MLIR 属性类型
   - 利用 MLIR 的类型系统进行验证

---

## 二、具体实现缺陷

### 缺陷 1: 写时重排分析仅支持 3 维

**文件**: `lib/Analysis/WriteTimeReorderingAnalysis.cpp`

**代码**:
```cpp
if (arrayDims.size() != 3) {
  LLVM_DEBUG(llvm::dbgs() << "Only 3D arrays are supported\n");
  return failure();
}
```

**问题**:
- MTTKRP 使用 4D 数组
- CNN 可能使用更高维数组
- 不支持 1D, 2D 数组

**影响**: 无法对多维度的 kernel 进行写时重排优化

**解决方案**:
- 通用的 N 维数组重排分析
- 泛化重排模式检测
- 支持可变维度数组

### 缺陷 2: 依赖距离分析不完整

**文件**: `lib/Analysis/SpaceTimeAnalysis.cpp`

**问题**:
- 仅分析简单的依赖关系
- 对复杂的非线性访问分析不充分
- 没有处理条件依赖

**具体代码** (~200-300 行):
```cpp
// 简化的依赖分析，不够完整
for (auto storeOp : stores) {
  for (auto loadOp : loads) {
    // 简单地检查是否访问同一 memref
    // 没有精确的依赖距离计算
  }
}
```

**影响**: 
- 无法准确判断哪些循环可作为空间循环
- 可能生成不正确的代码

**解决方案**:
- 使用 Polymer/ISL 的完整依赖分析
- 支持精确的依赖距离向量计算

### 缺陷 3: 循环体迁移代码缺失

**文件**: `lib/Transforms/SystolicDataflowGeneration.cpp`

**问题**:
- 框架存在，具体实现为空
- 无法将原始循环体迁移到 PE 模块
- 代码生成不完整

**代码** (~450-500 行):
```cpp
// TODO: 实现循环体迁移
LogicalResult migrateLoopBody(...) {
  // 未实现
  return success();
}
```

**影响**: 
- 最终生成的代码缺少实际计算
- 无法验证功能正确性

**优先级**: 高

### 缺陷 4: 双缓冲逻辑不完整

**文件**: `lib/Transforms/SystolicDataflowToHLS.cpp`

**问题**:
- 双缓冲框架定义完整，但生成逻辑为空
- 无法生成双缓冲 pragma
- 无法生成 buffer 管理代码

**影响**: 
- 性能优化功能无法使用
- 与 AutoSA 生成的代码不一致

---

## 三、设计问题总结表

| 问题 | 严重性 | 影响范围 | 修复难度 | 优先级 |
|------|--------|---------|---------|--------|
| Spacetime 硬编码 | 🔴 高 | 所有 Pass | 高 | 🔴 P1 |
| Kernel 特异性 | 🔴 高 | 全系统 | 高 | 🔴 P1 |
| 配置流混乱 | 🟡 中 | 多个 Pass | 中 | 🟡 P2 |
| 写时重排限制 | 🟡 中 | 分析 | 中 | 🟡 P2 |
| 循环体迁移缺失 | 🔴 高 | 代码生成 | 高 | 🔴 P1 |
| 双缓冲逻辑缺失 | 🟡 中 | 性能优化 | 中 | 🟡 P2 |
| 依赖分析不完整 | 🟡 中 | 正确性 | 中 | 🟡 P2 |

---

## 四、参数化重构方案

### 4.1 Spacetime 参数化

#### 目标
支持 ST0-ST5 的所有配置:
- ST0: [i] (1D, 水平)
- ST1: [j] (1D, 垂直)  
- ST2: [k] (1D, 约化)
- ST3: [i,j] (2D, 标准)
- ST4: [i,k] (2D, 约化)
- ST5: [j,k] (2D, 约化)

#### 设计

**数据结构**:
```cpp
enum class SpacetimeType {
  ST0, ST1, ST2, ST3, ST4, ST5, CUSTOM
};

struct ParametricSpaceTime {
  SpacetimeType type;
  SmallVector<unsigned, 3> spaceLoops;      // 动态空间循环
  SmallVector<unsigned, 3> timeLoops;       // 动态时间循环
  SmallVector<SystolicFlowDir, 3> flows;    // 数据流 (与 operands 对应)
  
  bool isValid() const;
  std::string serialize() const;
};
```

**重构步骤**:

1. **修改 SpaceTimeInfo**
   ```cpp
   struct SpaceTimeInfo {
     // 新增字段
     unsigned numSpaceLoops;
     unsigned numTimeLoops;
     bool hasReduction;
     
     // 分析函数返回具体配置
     ParametricSpaceTime toParametric() const;
   };
   ```

2. **修改 SystolicTransform**
   ```cpp
   // 算法改为通用的空间循环选择
   LogicalResult selectSpaceLoops(
       SpaceTimeInfo &info,
       unsigned numSpaceLoops);  // 参数化
   ```

3. **修改 SystolicDataflowGeneration**
   ```cpp
   // 数据流推导基于实际的循环使用模式
   LogicalResult analyzeDataFlows(
       const SpaceTimeInfo &info,
       DenseMap<Value, SystolicFlowDir> &flows);
   ```

4. **参数化代码生成**
   - PE 模块维度由 numSpaceLoops 决定
   - IO 模块结构由 spaceLoops 和 timeLoops 决定
   - HLS 代码由 ParametricSpaceTime 驱动

### 4.2 Kernel 通用化

#### 目标
支持任意 kernel (循环数 ≥ 3)

#### 设计

**Kernel 检测**:
```cpp
struct KernelInfo {
  unsigned numLoops;
  std::vector<std::string> loopNames;
  std::map<Value, AccessPattern> accessPatterns;
  
  static KernelInfo detectFromFunc(FuncOp func);
};
```

**通用的循环嵌套处理**:
```cpp
// 替代硬编码的 3 层循环假设
std::vector<AffineForOp> extractLoopBand(FuncOp func);

// 通用的遍历 N 层循环
void walkNestedLoops(
    AffineForOp outerLoop,
    std::function<void(std::vector<AffineForOp> &)> callback);
```

**访问模式分析**:
```cpp
enum class AccessPattern {
  SIMPLE,         // 简单的 IV 访问
  REDUCTION,      // 约化变量
  COMPLEX         // 非线性或复杂
};
```

### 4.3 配置管理改进

**统一配置对象**:
```cpp
class SystolicConfiguration {
private:
  ParametricSpaceTime spacetime;
  KernelInfo kernel;
  std::map<std::string, int64_t> parameters;
  
public:
  // 验证和查询
  bool isValid() const;
  bool supportsSpaceTime(SpacetimeType st) const;
  
  // 生成派生信息
  int getPEDimension() const;
  std::vector<int64_t> getIOShape() const;
  
  // 序列化
  MLIR属性 toAttribute() const;
  static SystolicConfiguration fromAttribute(Attribute);
};
```

---

## 五、修复优先级和工作量估算

### 优先级 1 (立即修复 - P1)
- [ ] Spacetime 参数化重构 (3-4 天)
- [ ] 循环体迁移实现 (2-3 天)

### 优先级 2 (短期修复 - P2)
- [ ] Kernel 通用化 (3-4 天)
- [ ] 配置管理改进 (1-2 天)
- [ ] 写时重排扩展 (1-2 天)

### 优先级 3 (后续优化 - P3)
- [ ] 双缓冲逻辑完善 (2-3 天)
- [ ] 依赖分析增强 (2-3 天)

---

## 附录: 代码审视清单

使用此清单审视新增或修改的代码:

- [ ] 是否支持参数化的 spacetime?
- [ ] 是否支持 3+ 维循环?
- [ ] 是否处理了错误情况?
- [ ] 是否有充分的单元测试?
- [ ] 是否与 AutoSA 行为一致?
- [ ] 是否能独立于具体 kernel?
- [ ] 是否能独立于数组维度?
- [ ] 配置参数是否清晰传递?
- [ ] 是否有代码注释说明设计?
- [ ] 是否避免了新的硬编码?

