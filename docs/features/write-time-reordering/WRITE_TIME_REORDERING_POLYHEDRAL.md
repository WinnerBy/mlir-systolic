# 基于多面体模型的写时重排实现

> **创建时间**: 2026-01-05  
> **状态**: 实现中

---

## 概述

基于多面体模型（Polyhedral Model）实现写时重排的自动检测和优化。利用 MLIR 的 Affine 分析和 ISL 库来：

1. **自动检测**：从 MLIR 中分析数组访问模式，检测非线性索引
2. **多面体分析**：使用 ISL 分析访问关系，计算最优数据布局变换
3. **自动应用**：将重排信息传递给代码生成器，自动应用重排

---

## 架构设计

### 1. 分析阶段（MLIR Pass）

**位置**: `lib/Analysis/WriteTimeReorderingAnalysis.cpp`

**功能**:
- 分析 `AffineLoadOp` 和 `AffineStoreOp` 的 `AffineMap`
- 检测非线性索引表达式（包含 `*`, `/`, `%` 的表达式）
- 计算最优的数据布局重排方案
- 将重排信息存储在函数属性中

**集成点**: `SystolicDataflowGeneration` pass

```cpp
// 在 SystolicDataflowGeneration pass 中
WriteTimeReorderingAnalyzer reorderingAnalyzer(func);
reorderingAnalyzer.analyze();

// 存储重排信息到函数属性
func->setAttr("systolic.reorder.A.dims", ...);
func->setAttr("systolic.reorder.A.perm", ...);
```

### 2. 代码生成阶段

**位置**: `tools/systolic-translate/systolic-translate.cpp`

**功能**:
- 从函数属性读取重排信息
- 注册数组布局到 `SystolicHLSEmitter`
- 自动应用重排到代码生成

```cpp
// 在 emit() 函数中
for (auto funcOp : module.getOps<func::FuncOp>()) {
  // 读取重排属性
  auto dimsAttr = funcOp->getAttrOfType<ArrayAttr>("systolic.reorder.A.dims");
  auto permAttr = funcOp->getAttrOfType<ArrayAttr>("systolic.reorder.A.perm");
  
  // 注册布局
  registerArrayLayout("A", dims, writeIndices, readIndices);
}
```

---

## 实现细节

### 1. 非线性索引检测

**方法**: `isNonLinearExpr(AffineExpr expr)`

**检测逻辑**:
- 遍历 AffineExpr 树
- 检测乘法（`Mul`）、除法（`FloorDiv`, `CeilDiv`）、取模（`Mod`）操作
- 递归检查所有子表达式

**示例**:
```cpp
// 检测: 8 * c6 / 16
AffineExpr expr = ...;  // 从 AffineMap 中提取
if (isNonLinearExpr(expr)) {
  // 标记为非线性访问
}
```

### 2. 重排方案计算

**方法**: `computeReordering(ArrayAccessPattern &pattern)`

**策略**（当前实现）:
- 对于 3 维数组，将包含非线性索引的维度移到中间位置
- 原始: `[dim0][dim1][dim2]`
- 如果非线性在 dim0: `[dim1][dim0][dim2]`
- 如果非线性在 dim2: `[dim1][dim2][dim0]`
- 如果非线性在 dim1: 不变（已在中间）

**未来扩展**:
- 使用 ISL 分析访问关系
- 计算最小化非顺序访问的布局变换
- 同时计算循环变换以匹配布局变换

### 3. 多面体分析（未来工作）

**计划**: `computeReorderingWithISL()`

**步骤**:
1. 从 `AffineMap` 构建 ISL 访问关系（`isl_map`）
2. 使用 ISL 分析访问模式
3. 计算布局变换矩阵
4. 计算循环变换以匹配布局变换

**示例**:
```cpp
// 构建访问关系
isl_map *readAccess = buildAccessMapFromAffineMap(loadMap);
isl_map *writeAccess = buildAccessMapFromAffineMap(storeMap);

// 分析访问模式
isl_union_map *accesses = isl_union_map_from_map(readAccess);
isl_union_map *writes = isl_union_map_from_map(writeAccess);

// 计算布局变换
isl_map *layoutTransform = computeLayoutTransform(accesses, writes);

// 应用变换
isl_map *transformedAccess = isl_map_apply_range(accesses, layoutTransform);
```

---

## 使用流程

### 1. MLIR 转换流程

```bash
# 1. 运行 SystolicTransform pass（循环变换、分块等）
mlir-opt input.mlir \
  -systolic-transform \
  -systolic-dataflow-generation \  # 包含写时重排分析
  -o output.mlir

# 2. 生成 HLS C++ 代码（自动应用重排）
systolic-translate output.mlir -o kernel.cpp
```

### 2. 重排信息传递

**MLIR 属性格式**:
```mlir
func.func @kernel0(...) attributes {
  // 重排后的维度
  systolic.reorder.A.dims = [2, 16, 16]
  // 维度置换: new_dim[i] = originalDims[perm[i]]
  systolic.reorder.A.perm = [1, 2, 0]
} {
  // ... 函数体
}
```

**代码生成器读取**:
```cpp
// 在 SystolicHLSEmitter::emit() 中
auto dimsAttr = funcOp->getAttrOfType<ArrayAttr>("systolic.reorder.A.dims");
auto permAttr = funcOp->getAttrOfType<ArrayAttr>("systolic.reorder.A.perm");

// 注册布局
registerArrayLayout("A", dims, writeIndices, readIndices);
```

---

## 示例：MTTKRP Kernel

### 原始访问模式

```mlir
// 写入: local_A[c4][c5][c6] = ...
affine.store %value, %local_A[%c4, %c5, %c6] : memref<16x2x16xf32>

// 读取: local_A[c8][c5][8 * c6 / 16] = ...
// AffineMap: (d0, d1, d2, d3) -> (d0, d1, (d2 * 8) floordiv 16)
affine.load %local_A[%c8, %c5, %expr] : memref<16x2x16xf32>
```

### 分析结果

**检测**:
- 读取索引包含非线性表达式: `8 * c6 / 16`
- 非线性维度: dim2 (第三个维度)

**重排方案**:
- 原始维度: `[16, 2, 16]`
- 重排后: `[2, 16, 16]` (将 dim2 移到中间)
- 置换: `[1, 2, 0]`

### 生成的代码

```cpp
// 重排后的数组声明
void A_IO_L2_in_intra_trans(..., A_t16 local_A[2][16][16], ...);

// 写入时应用重排
void A_IO_L2_in_inter_trans(...) {
  local_A[c5][c6][c4] = out_data;  // 重排后的索引
}

// 读取时使用重排后的布局
void A_IO_L2_in_intra_trans(...) {
  in_data = local_A[c5][8 * c6 / 16][c8];  // 非线性索引移到中间
}
```

---

## 当前状态

### ✅ 已完成

1. **基础框架**
   - `WriteTimeReorderingAnalyzer` 类
   - 非线性索引检测
   - 简单重排策略（3 维数组）

2. **MLIR 集成**
   - 在 `SystolicDataflowGeneration` pass 中集成
   - 将重排信息存储到函数属性

3. **代码生成器集成**
   - 从函数属性读取重排信息
   - 自动注册和应用重排

### ⚠️ 待完成

1. **多面体分析**
   - 使用 ISL 分析访问关系
   - 计算最优布局变换
   - 计算循环变换

2. **扩展支持**
   - 支持 2 维和 4+ 维数组
   - 支持更复杂的重排策略

3. **测试验证**
   - 测试 MTTKRP kernel
   - 验证重排后的代码正确性
   - 对比 HLS 综合结果

---

## 相关文档

- [写时重排实现进度](WRITE_TIME_REORDERING_IMPLEMENTATION.md) - 实现细节
- [写时重排使用指南](WRITE_TIME_REORDERING_USAGE.md) - 使用说明
- [随机读取问题分析](RANDOM_ACCESS_ANALYSIS.md) - 问题背景

---

## 更新日志

- **2026-01-05**: 创建基于多面体模型的实现文档
  - 设计分析架构
  - 实现非线性索引检测
  - 集成到 MLIR pass 和代码生成器


