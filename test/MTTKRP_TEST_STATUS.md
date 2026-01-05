# MTTKRP 测试状态报告

> **创建时间**: 2026-01-05  
> **状态**: 测试进行中

---

## 测试文件

### 1. `test/mttkrp_affine_only.mlir` ✅
**描述**: 纯 Affine IR 测试文件，包含非线性访问模式
- **数组**: A[16][2][16]
- **非线性模式**: `(l * 8) floordiv 16` 在第三维度
- **用途**: 直接测试 `systolic-dataflow-generation` pass 的分析功能

**关键特征**:
```mlir
%idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%l)
%val = affine.load %A[%i, %k, %idx] : memref<16x2x16xf32>
```

### 2. `test/mttkrp_kernel.mlir` ✅
**描述**: 完整的 MTTKRP kernel
- **计算**: `D(i,j) += A(i,k,l) * B(k,j) * C(j,l)`
- **数组**: A[128][64][128], B[64][64], C[64][128], D[128][64]
- **状态**: 已通过 `systolic-transform` pass

### 3. `test/mttkrp_with_nonlinear.mlir` ⚠️
**描述**: 包含非线性访问的完整 MTTKRP
- **状态**: 需要修复语法错误

---

## 测试结果

### Pass 运行状态

#### ✅ `systolic-transform` Pass
- **状态**: 正常工作
- **测试**: `test/mttkrp_kernel.mlir` 成功转换
- **输出**: 生成了包含 SCoP 语句的 MLIR 代码

#### ⚠️ `systolic-dataflow-generation` Pass
- **状态**: 运行中，需要验证
- **测试**: `test/mttkrp_affine_only.mlir`
- **预期**: 应该检测到非线性访问并生成重排属性

---

## 分析器功能验证

### 预期行为

1. **检测非线性访问**
   - 分析器应该检测到 `(l * 8) floordiv 16` 是非线性表达式
   - 识别非线性维度（第三维度，dim 2）

2. **计算重排方案**
   - 原始布局: `[16][2][16]`
   - 重排后布局: `[2][16][16]` (将非线性维度移到中间)
   - 置换: `[1, 2, 0]`

3. **生成属性**
   - `systolic.reorder.arg0.dims = [2, 16, 16]`
   - `systolic.reorder.arg0.perm = [1, 2, 0]`

### 验证步骤

1. 运行 pass 并检查输出
2. 查找 `systolic.reorder` 属性
3. 验证属性值是否正确
4. 检查分析器日志（如果有）

---

## 发现的问题

### 1. 测试文件状态问题
**现象**: 某些测试文件可能已被之前的 pass 转换过
**解决**: 创建新的纯 Affine IR 文件 (`mttkrp_affine_only.mlir`)

### 2. 数组名称检测
**问题**: 分析器使用 `arg0`, `arg1` 等作为数组名称
**影响**: 重排属性名称会是 `systolic.reorder.arg0.*` 而不是 `systolic.reorder.A.*`
**状态**: 这是预期的行为（如果没有设置 `mlir.name` 属性）

---

## 下一步工作

### 1. 验证分析器功能（优先级：高）
- [ ] 确认分析器能检测到非线性访问
- [ ] 验证重排方案计算正确
- [ ] 检查属性是否正确生成

### 2. 修复测试文件（优先级：中）
- [ ] 修复 `mttkrp_with_nonlinear.mlir` 的语法错误
- [ ] 创建更多测试用例

### 3. 端到端测试（优先级：高）
- [ ] 运行完整的 pass pipeline
- [ ] 生成 HLS 代码
- [ ] 验证重排是否正确应用

---

## 相关文档

- [MTTKRP 测试总结](MTTKRP_TEST_SUMMARY.md)
- [写时重排实现](../docs/WRITE_TIME_REORDERING_IMPLEMENTATION.md)
- [基于多面体模型的实现](../docs/WRITE_TIME_REORDERING_POLYHEDRAL.md)
- [随机读取问题分析](../docs/RANDOM_ACCESS_ANALYSIS.md)

---

## AutoSA 参考

- **Kernel 源码**: https://github.com/UCLA-VAST/AutoSA/blob/master/autosa_tests/large/mttkrp/kernel.c
- **生成的 HLS 代码**: `autosa_hls_output/kernel_mttkrp.cpp`
- **非线性访问模式**:
  - `local_A[c8][c5][8 * c6 / 16]` (第115行)
  - `local_C[c7][8 * c6 / 16]` (第827行)


