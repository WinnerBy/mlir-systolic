# 写时重排测试结果

> **创建时间**: 2026-01-05  
> **状态**: ✅ 重排属性已成功生成

---

## 测试结果

### ✅ 重排属性生成成功

**测试文件**: `test/mttkrp_affine_only.mlir`

**生成的属性**:
```mlir
func.func @kernel_mttkrp_affine(%arg0: memref<16x2x16xf32>) 
  attributes {
    systolic.reorder.arg0.dims = [2, 16, 16],
    systolic.reorder.arg0.perm = [1 : i32, 0 : i32, 2 : i32]
  }
```

**分析**:
- **原始维度**: [16, 2, 16]
- **非线性维度**: 第三维度 (dim 2) - 使用 `(l * 8) floordiv 16`
- **重排后维度**: [2, 16, 16]
- **置换**: [1, 0, 2]

**注意**: 置换值需要验证是否正确。根据代码逻辑，应该是 [1, 2, 0]（将第三维度移到中间）。

---

## 修复的问题

### 1. Yield 操作位置 ✅
- 修复了 `IOModuleYieldOp` 的位置问题
- Pass 现在可以成功运行

### 2. 非线性访问检测 ✅
- 修复了分析器对 `affine.apply` 的检测
- 现在可以正确检测到通过 `affine.apply` 计算的非线性索引

**修复代码**:
```cpp
// 检查索引操作数是否来自 affine.apply
for (Value idx : loadOp.getMapOperands()) {
  if (auto applyOp = idx.getDefiningOp<AffineApplyOp>()) {
    AffineMap applyMap = applyOp.getAffineMap();
    patterns[patternIdx].loadMaps.push_back(applyMap);
  }
}
```

---

## 验证步骤

### 1. 属性生成 ✅
- [x] 分析器检测到非线性访问
- [x] 重排属性已生成
- [ ] 置换值需要验证

### 2. 代码生成 ⚠️
- [ ] 验证代码生成器是否能读取属性
- [ ] 检查数组声明是否正确应用重排
- [ ] 验证索引访问是否正确应用置换

---

## 下一步工作

### 优先级 1: 验证置换逻辑

检查 `computeReordering` 函数的逻辑：
- 原始: [16, 2, 16]，非线性在 dim 2
- 预期: [2, 16, 16]，置换 [1, 2, 0]
- 实际: [2, 16, 16]，置换 [1, 0, 2]

需要验证置换逻辑是否正确。

### 优先级 2: 代码生成验证

1. **检查代码生成器**
   - 验证是否能读取 `systolic.reorder.*` 属性
   - 检查数组声明是否正确应用重排

2. **验证生成的代码**
   - 检查 `local_A` 的声明维度
   - 验证索引访问是否正确应用置换

---

## 相关文件

- **测试文件**: `test/mttkrp_affine_only.mlir`
- **生成的 MLIR**: `test/output/mttkrp_test/affine_dataflow_with_analysis.mlir`
- **生成的 HLS**: `test/output/mttkrp_test/mttkrp_kernel.cpp`
- **分析器代码**: `lib/Analysis/WriteTimeReorderingAnalysis.cpp`

---

## 总结

✅ **主要功能已实现**:
- Pass 可以成功运行
- 分析器能检测到非线性访问
- 重排属性已生成

⚠️ **待验证**:
- 置换逻辑是否正确
- 代码生成器是否能正确应用重排


