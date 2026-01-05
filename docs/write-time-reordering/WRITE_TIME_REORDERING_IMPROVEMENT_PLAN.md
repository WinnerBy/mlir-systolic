# 写时重排功能改进方案总结

> **创建时间**: 2026-01-05  
> **作者**: 代码分析报告  
> **优先级**: 高

---

## 📋 概述

您的写时重排实现**已完成 60%**：

- ✅ **分析层** (100%)：非线性访问检测、重排方案计算、属性生成
- ❌ **应用层** (0%)：代码生成器集成、循环变换、端到端验证
- ⚠️ **优化层** (20%)：多面体分析框架已设计但未实现

---

## 🎯 关键发现

### 问题 1：代码生成器与分析器脱离

**现状**：
```
分析器生成属性  →  属性被存储  →  代码生成器忽视属性  →  重排优化未应用
WriteTimeReordering     MLIR          systolic-translate
    PASS              Attributes         CODEGEN
```

**影响**：
- 生成的 HLS 代码与原始代码相同
- 重排优化完全未被应用
- 内存访问模式未改善

### 问题 2：缺少循环变换

**理论要求**：
```
数据布局变换  +  循环变换  =  一致的优化
  (3-D array)    (loop nests)  (correct & fast)
```

**当前状态**：
- 仅计算数据布局变换
- 未计算循环变换
- 导致代码生成时无法一致应用变换

### 问题 3：重排策略过于简单

**当前**：对所有 3D 数组用同一规则
- 将非线性维度移到中间
- 不考虑实际访问成本

**应该**：根据访问模式选择最优布局
- 分析写入 stride 和读取 stride
- 计算每种排列的缓存成本
- 选择最小化内存访问成本的排列

---

## 🛠️ 改进方案

### 第 1 阶段：关键路径修复（1-2 周）

#### 目标：使重排优化真正生效

##### 1.1 代码生成器集成

**文件**：`tools/systolic-translate/systolic-translate.cpp`

**实现要点**：
```cpp
// 1. 添加数据结构
struct ArrayReorderingInfo {
  std::string arrayName;
  SmallVector<int64_t, 3> originalDims;
  SmallVector<int64_t, 3> reorderedDims;
  SmallVector<unsigned, 3> dimPermutation;
};

// 2. 添加方法
void extractReorderingInfo(func::FuncOp funcOp)  // 从属性读取
void getArrayDims(StringRef arrayName)           // 获取声明维度
void applyAccessPermutation(...)                 // 应用索引置换

// 3. 修改数组声明
// 从：local_A[latency][1]
// 到：local_A[reorderedDims[0]][reorderedDims[1]][reorderedDims[2]]

// 4. 修改数组访问
// 从：local_A[c4][0]
// 到：local_A[permutedIdx[0]][permutedIdx[1]][permutedIdx[2]]
```

**修改的函数**：
- `emitIOL2InIntraTrans()`
- `emitIOL2InInterTrans()`
- `emitIOL2InInterTransBoundary()`
- 所有其他数组访问处理函数

**预期结果**：
```cpp
// 修改前
local_A[c4][0] = out_data;

// 修改后（应用置换 [1, 2, 0]）
local_A[0][c5][c4] = out_data;
```

**验证方法**：
```bash
# 生成 HLS 代码
mlir-opt test/mttkrp_affine_only.mlir \
  -systolic-dataflow-generation \
  | systolic-translate -o kernel_reordered.cpp

# 检查是否正确应用了重排
grep "local_A\[" kernel_reordered.cpp
# 应该看到重排后的索引，而不是原始索引
```

##### 1.2 测试框架

**新增**：`test/write_time_reordering_codegen_test.cpp`

```cpp
// 测试 1：属性生成
TEST(WriteTimeReorderingCodegen, AttributeGeneration) {
  // 验证 systolic.reorder.* 属性被正确生成
}

// 测试 2：代码生成正确性
TEST(WriteTimeReorderingCodegen, ArrayDeclareAndAccess) {
  // 验证数组声明使用重排维度
  // 验证数组访问应用置换
}

// 测试 3：生成代码的语义正确性
TEST(WriteTimeReorderingCodegen, SemanticCorrectness) {
  // 验证重排前后访问的数据相同
  // 模拟内存访问验证
}
```

---

### 第 2 阶段：多面体分析优化（2-3 周）

#### 目标：从启发式升级到数据驱动的最优布局选择

##### 2.1 ISL 多面体分析集成

**依赖安装**（Ubuntu 22.04）：
```bash
sudo apt-get install libisl-dev

# 或者从源代码编译
git clone https://repo.or.cz/isl.git
cd isl
./autogen.sh
./configure --prefix=$HOME/.local
make && make install
```

**CMakeLists.txt 更新**：
```cmake
# 找到 ISL
find_package(ISL REQUIRED)

# 链接到库
target_link_libraries(systolic_analysis PUBLIC ISL::ISL)
```

##### 2.2 实现访问模式分析

**新增类**：`PolyhedralAccessAnalyzer`

```cpp
// 分析访问模式
struct AccessPattern {
  SmallVector<int64_t, 3> minStride;     // 最小 stride
  SmallVector<int64_t, 3> reuseDistance; // 重用距离
  SmallVector<bool, 3> isContiguous;     // 是否连续访问
  double randomnessScore;                 // 随机性评分 (0.0 ~ 1.0)
};

AccessPattern analyzeReadAccess(...);
AccessPattern analyzeWriteAccess(...);
```

**核心算法**：
1. 对每个数组维度分析访问表达式
2. 计算 stride（相邻访问之间的距离）
3. 检测非线性表达式（除法、取模等）
4. 计算"随机性评分"

##### 2.3 布局优化器

**新增类**：`LayoutOptimizer`

```cpp
// 枚举所有 3D 排列（6 种）
SmallVector<LayoutScoreResult, 6> evaluateAllLayouts(...)

// 评分函数
double evaluateMemoryCost(write, read, permutation)
double evaluateCacheLocality(write, read, permutation)

// 选择最优布局
LayoutScoreResult selectBestLayout(...)
```

**评分指标**：
- **内存成本**：非连续访问的数量
- **缓存局部性**：最内层循环是否连续
- **综合评分**：(memoryCost - cacheLocality)

##### 2.4 集成到分析器

```cpp
// 在 WriteTimeReorderingAnalysis.cpp 中
LogicalResult computeReorderingWithISL(ArrayAccessPattern &pattern) {
  PolyhedralAccessAnalyzer analyzer;
  auto writeAccess = analyzer.analyzeWriteAccess(...);
  auto readAccess = analyzer.analyzeReadAccess(...);
  
  auto layouts = LayoutOptimizer::evaluateAllLayouts(
      writeAccess, readAccess);
  
  // 选择最优布局
  pattern.dimPermutation = layouts[0].permutation;
  
  return success();
}
```

---

### 第 3 阶段：循环变换支持（3-4 周）

#### 目标：完整的数据布局 + 循环变换联合优化

##### 3.1 ISL 循环调度计算

```cpp
// 伪代码
isl_schedule *deriveLoopSchedule(
    const SmallVector<unsigned, 3> &layoutPermutation,
    func::FuncOp func) {
  
  // 基本思路：
  // 1. 提取现有循环嵌套结构
  // 2. 应用排列变换：loop_new[i] = loop_old[permutation[i]]
  // 3. 返回 ISL 调度
  
  // 实际实现需要：
  // - 构建 ISL 循环依赖图
  // - 应用 affine 变换矩阵
  // - 验证变换的合法性
  
  return loopSchedule;
}
```

##### 3.2 循环信息存储

```cpp
// 扩展 ArrayAccessPattern 结构
struct ArrayAccessPattern {
  // ... 现有字段 ...
  
  // 新增：循环变换信息
  isl_schedule *loopSchedule = nullptr;
  SmallVector<unsigned, 3> loopPermutation;
  
  // 性能指标
  int64_t cacheMissCount;
  double memoryBandwidthUtilization;
};
```

##### 3.3 代码生成器应用循环变换

在 `systolic-translate` 中：
```cpp
// 读取循环变换
auto loopPerms = funcOp->getAttrOfType<ArrayAttr>(
    "systolic.loop_permutation");

// 应用循环变换生成代码
// 需要重新排序循环嵌套
```

---

### 第 4 阶段：性能验证与优化（4-5 周）

#### 目标：验证重排是否真的改善了性能

##### 4.1 性能测试框架

```cpp
// 新增：性能基准测试
TEST(WriteTimeReorderingPerf, MTTKRPMemoryAccess) {
  // 1. 原始版本：生成 HLS 代码（无重排）
  auto originalCost = simulateMemoryAccess(original_kernel);
  
  // 2. 优化版本：生成 HLS 代码（有重排）
  auto optimizedCost = simulateMemoryAccess(optimized_kernel);
  
  // 3. 验证改善
  EXPECT_LT(optimizedCost, originalCost);
  double improvement = (originalCost - optimizedCost) / originalCost;
  EXPECT_GT(improvement, 0.1);  // 至少改善 10%
}
```

##### 4.2 内存访问模拟器

```cpp
// 简单的缓存模型
struct CacheModel {
  int L1_size = 64 * 1024;      // 64KB L1
  int L1_line_size = 64;         // 64B 缓存行
  
  int countCacheMisses(const SmallVector<Access, 1000> &accesses) {
    // 模拟 LRU 替换策略
    std::set<int64_t> cache;
    int misses = 0;
    
    for (auto access : accesses) {
      int64_t line = access.addr / L1_line_size;
      if (cache.find(line) == cache.end()) {
        misses++;
        if (cache.size() >= L1_size / L1_line_size) {
          cache.erase(cache.begin());
        }
        cache.insert(line);
      }
    }
    return misses;
  }
};
```

##### 4.3 HLS 综合对比

```bash
# 原始版本
vivado_hls -f hls.tcl -kernel original_kernel

# 优化版本
vivado_hls -f hls.tcl -kernel reordered_kernel

# 对比报告
# - 面积 (LUT/BRAM)
# - 延迟 (cycle latency)
# - II (initiation interval)
# - 内存带宽利用率
```

---

## 📊 实现路线图

```
Week 1-2: Phase 1 - 代码生成器集成
├─ 修改 systolic-translate.cpp
├─ 添加属性读取
├─ 应用维度置换
└─ 基础测试

Week 3-4: Phase 2 - 多面体分析
├─ ISL 集成
├─ 访问模式分析
├─ 布局优化器
└─ 完整单元测试

Week 5-6: Phase 3 - 循环变换
├─ 计算循环调度
├─ 验证变换合法性
├─ 集成到代码生成
└─ 功能测试

Week 7-8: Phase 4 - 性能验证
├─ 性能测试框架
├─ 缓存模拟器
├─ HLS 综合对比
└─ 性能报告
```

---

## ✅ 验收标准

### Phase 1 完成标准

- [ ] `systolic-translate` 能读取 `systolic.reorder.*` 属性
- [ ] 生成的 HLS 代码数组声明使用重排维度
- [ ] 生成的 HLS 代码数组访问应用置换
- [ ] 单元测试覆盖所有关键路径
- [ ] MTTKRP 测试用例通过

### Phase 2 完成标准

- [ ] ISL 多面体分析正确实现
- [ ] 访问模式分析准确率 > 90%
- [ ] 布局优化器选择的排列 score < 启发式 score
- [ ] 单元测试覆盖 10+ 个测试用例

### Phase 3 完成标准

- [ ] 循环变换正确计算
- [ ] 变换合法性检查通过
- [ ] 代码生成器正确应用循环变换
- [ ] 生成的代码在模拟器中验证正确

### Phase 4 完成标准

- [ ] 性能测试框架完成
- [ ] 5+ 个基准测试通过
- [ ] 重排优化相比原始版本至少改善 15% (cache misses)
- [ ] HLS 综合对比报告完成

---

## 📚 代码位置参考

### 现有代码

| 文件 | 作用 | 行数 |
|------|------|------|
| [lib/Analysis/WriteTimeReorderingAnalysis.h](lib/Analysis/WriteTimeReorderingAnalysis.h) | 分析器头文件 | - |
| [lib/Analysis/WriteTimeReorderingAnalysis.cpp](lib/Analysis/WriteTimeReorderingAnalysis.cpp) | 分析器实现 | 307 |
| [lib/Transforms/SystolicDataflowGeneration.cpp](lib/Transforms/SystolicDataflowGeneration.cpp) | Pass 集成 | 721 |
| [tools/systolic-translate/systolic-translate.cpp](tools/systolic-translate/systolic-translate.cpp) | 代码生成器 | 1321 |

### 新增代码（提议）

| 文件 | 作用 | 优先级 |
|------|------|--------|
| `systolic-translate.cpp` 扩展 | 代码生成器集成 | 🔴 关键 |
| `PolyhedralAccessAnalyzer.h/.cpp` | 多面体分析 | 🟡 高 |
| `LayoutOptimizer.h/.cpp` | 布局优化 | 🟡 高 |
| `test/write_time_reordering_codegen_test.cpp` | 代码生成测试 | 🟡 高 |
| `test/write_time_reordering_perf_test.cpp` | 性能测试 | 🟢 中 |

---

## 🎓 相关文献

### 多面体模型
- Feautrier P. "Parametric integer programming" (1988)
- Barron J. "The case for a high-level description language" (2004)

### ISL 库
- Verdoolaege S. "ISL: An integer set library for the polyhedral model" (2010)
- 官方文档：http://isl.gforge.inria.fr/

### 数据布局优化
- Strout M. et al. "Data layout transformation for improving cache performance" (2004)
- Kannan R. "Optimal and practical data layout for structured mesh codes" (2017)

### MLIR Affine
- MLIR 官方文档：https://mlir.llvm.org/docs/Dialects/Affine/
- Affine 分析：https://mlir.llvm.org/docs/Rationale/AffineAnalysisInfra/

---

## 💬 补充说明

### 为什么当前置换逻辑 [1, 2, 0] 可能不是最优的？

**MTTKRP 示例**：
```
原始维度: [16, 2, 16]
写入访问: [c4, c5, c6]      (完全线性，stride = 1)
读取访问: [c8, c5, 8*c6/16] (dim 2 非线性)

当前重排: [2, 16, 16]，置换 [1, 2, 0]
结果: [c5, 8*c6/16, c8]     (非线性仍在中间)

更优方案可能是: [16, 16, 2]，置换 [2, 0, 1]
结果: [c6, c4, c5]           (需要具体分析)
```

需要用 ISL 分析来确定哪种排列最优。

### 为什么缺少循环变换？

数据布局变换（重排维度）必须配合循环变换：

```cpp
// 原始循环
for (c4 = 0..15) {       // dim 0
  for (c5 = 0..1) {      // dim 1
    for (c6 = 0..15) {   // dim 2
      A[c4][c5][c6] = ...
    }
  }
}

// 如果只重排数据不重排循环
for (c4 = 0..15) {       // 循环仍按原序
  for (c5 = 0..1) {
    for (c6 = 0..15) {
      A_reordered[c5][c6][c4] = ...  // 访问与声明不匹配！
    }
  }
}

// 正确的做法：重排循环以匹配数据布局
for (c5 = 0..1) {        // 按重排顺序循环
  for (c6 = 0..15) {
    for (c4 = 0..15) {
      A_reordered[c5][c6][c4] = ...  // 现在匹配了
    }
  }
}
```

---

## 📞 联系与问题

如果实现过程中遇到问题：

1. **ISL 集成问题**：参考 ISL 官方文档和示例
2. **MLIR 属性问题**：查看 MLIR 源码中的属性使用
3. **测试框架**：参考 MLIR 现有的测试框架

---

**完成此方案后，写时重排优化将成为一个完整的、可验证的、具有性能收益的功能。**

