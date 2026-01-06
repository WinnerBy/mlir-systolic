# Phase 2 多面体分析优化实现总结

> **实现时间**: 2026-01-05  
> **状态**: ✅ Phase 2 核心功能已完成

---

## ✅ 已完成的工作

### 1. 访问模式分析器（PolyhedralAccessAnalyzer）✅

**文件**: 
- `include/systolic/Analysis/PolyhedralAccessAnalyzer.h`
- `lib/Analysis/PolyhedralAccessAnalyzer.cpp`

**功能**:
- 分析每个维度的访问模式（stride、连续性、非线性）
- 计算随机性评分
- 分别分析读取和写入访问模式

**核心方法**:
```cpp
AccessPattern analyzeWriteAccess(...);  // 分析写入访问
AccessPattern analyzeReadAccess(...);    // 分析读取访问
AccessPattern analyzeCombinedAccess(...); // 分析组合访问
```

**分析指标**:
- `minStride`: 最小 stride（相邻访问之间的距离）
- `isContiguous`: 是否是连续访问（stride == 1）
- `hasNonLinear`: 是否有非线性访问（除法、取模等）
- `randomnessScore`: 随机性评分（0.0 = 完全连续，1.0 = 完全随机）

### 2. 布局优化器（LayoutOptimizer）✅

**文件**:
- `include/systolic/Analysis/LayoutOptimizer.h`
- `lib/Analysis/LayoutOptimizer.cpp`

**功能**:
- 枚举所有 3D 排列（6 种）
- 评估每种排列的内存成本和缓存局部性
- 选择最优布局

**核心方法**:
```cpp
SmallVector<LayoutScoreResult, 6> evaluateAllLayouts(...);  // 评估所有排列
LayoutScoreResult selectBestLayout(...);                     // 选择最优布局
double evaluateMemoryCost(...);                              // 评估内存成本
double evaluateCacheLocality(...);                           // 评估缓存局部性
```

**评分指标**:
- `memoryCost`: 内存访问成本（越低越好）
  - 非线性访问成本高
  - 非连续访问成本高
  - 外层维度成本更高
- `cacheLocality`: 缓存局部性（越高越好）
  - 最内层维度连续性最重要
  - 连续且无非线性访问得分最高
- `totalScore`: 综合评分 = memoryCost - cacheLocality（越低越好）

### 3. 集成到分析器 ✅

**文件**: `lib/Analysis/WriteTimeReorderingAnalysis.cpp`

**修改**:
- 更新 `computeReorderingWithISL()` 方法，使用多面体分析
- 在 `analyzePattern()` 中优先尝试多面体分析
- 如果多面体分析失败，回退到简单启发式

**工作流程**:
```
1. 检测到非线性访问
   ↓
2. 调用 computeReorderingWithISL()
   ↓
3. 使用 PolyhedralAccessAnalyzer 分析访问模式
   ↓
4. 使用 LayoutOptimizer 评估所有排列
   ↓
5. 选择最优布局并应用
```

---

## 📊 改进效果

### 之前（Phase 1）
- 使用固定规则：将非线性维度移到中间
- 不考虑实际访问模式
- 对所有 3D 数组用同一策略

### 现在（Phase 2）
- 分析实际访问模式（stride、连续性、非线性）
- 评估所有可能的排列（6 种）
- 基于成本和局部性选择最优布局
- 数据驱动的优化决策

---

## 🔍 实现细节

### 访问模式分析

**维度分析**:
```cpp
DimensionAccessPattern analyzeDimension(
    unsigned dimIdx,
    const SmallVector<AffineMap, 8> &maps,
    const SmallVector<int64_t, 3> &arrayDims);
```

**Stride 计算**:
- 检查表达式是否直接使用维度（stride = 1）
- 检查常数乘法（stride = 系数）
- 处理加法表达式

**随机性评分**:
- 非线性访问：+0.5
- 非连续访问：+0.3 × (1 - 1/stride)
- 综合评分：所有维度的平均值

### 布局评估

**所有 3D 排列**:
```cpp
{0, 1, 2}, {0, 2, 1}, {1, 0, 2},
{1, 2, 0}, {2, 0, 1}, {2, 1, 0}
```

**成本评估**:
- 非线性访问：10.0 × (3 - i)（外层权重更高）
- 非连续访问：2.0 × (stride - 1) × (3 - i)

**局部性评估**:
- 权重：内层维度权重更高（i+1)/3.0
- 连续且无非线性：+1.0
- 连续但有非线性：+0.5

---

## 🧪 测试验证

### 测试用例
- **文件**: `test/mttkrp_affine_only.mlir`
- **数组**: `memref<16x2x16xf32>`
- **非线性访问**: 第三维度使用 `(l * 8) floordiv 16`

### 预期结果
- 多面体分析应该检测到非线性访问在第三维度
- 评估所有 6 种排列
- 选择最优布局（可能不是简单的移到中间）

### 验证方法
```bash
# 运行 pass 并查看调试输出
build/bin/systolic-opt test/mttkrp_affine_only.mlir \
  -systolic-dataflow-generation \
  -debug-only=write-time-reordering-analysis

# 检查生成的属性
grep "systolic.reorder" test/output/mttkrp_phase2_final.mlir
```

---

## 📝 新增文件

1. **include/systolic/Analysis/PolyhedralAccessAnalyzer.h**
   - 访问模式分析器头文件

2. **lib/Analysis/PolyhedralAccessAnalyzer.cpp**
   - 访问模式分析器实现

3. **include/systolic/Analysis/LayoutOptimizer.h**
   - 布局优化器头文件

4. **lib/Analysis/LayoutOptimizer.cpp**
   - 布局优化器实现

---

## 🔄 修改的文件

1. **lib/Analysis/WriteTimeReorderingAnalysis.cpp**
   - 更新 `computeReorderingWithISL()` 方法
   - 集成多面体分析

2. **lib/Analysis/CMakeLists.txt**
   - 添加新源文件到构建系统

---

## ⚠️ 注意事项

### 当前限制

1. **仅支持 3D 数组**
   - 2D、4D 及更高维数组会回退到简单启发式

2. **Stride 计算简化**
   - 当前实现只处理简单情况
   - 复杂表达式可能无法准确计算 stride

3. **未使用真正的 ISL**
   - 当前实现基于 AffineMap 分析
   - 未来可以集成 ISL 进行更精确的分析

### 未来改进

1. **支持更高维数组**
   - 扩展排列生成逻辑

2. **更精确的 stride 计算**
   - 使用 ISL 进行精确的访问关系分析

3. **考虑循环嵌套**
   - 分析循环结构对访问模式的影响

4. **重用距离分析**
   - 计算数据重用距离，优化缓存性能

---

## 🎯 下一步工作

### 优先级 1: 测试和验证
1. 创建更多测试用例
2. 验证布局选择的正确性
3. 性能对比测试

### 优先级 2: 改进分析精度
1. 改进 stride 计算
2. 添加重用距离分析
3. 考虑循环嵌套影响

### 优先级 3: 扩展支持
1. 支持 2D、4D 数组
2. 集成真正的 ISL 分析
3. 添加循环变换计算

---

## 📚 相关文档

- **改进方案**: `WRITE_TIME_REORDERING_IMPROVEMENT_PLAN.md`
- **ISL 实现参考**: `WRITE_TIME_REORDERING_ISL_IMPLEMENTATION.cpp`
- **Phase 1 总结**: `IMPLEMENTATION_IMPROVEMENTS.md`

---

**✅ Phase 2 多面体分析优化核心功能已完成！现在重排策略基于数据驱动的访问模式分析，而不是固定的启发式规则。**

