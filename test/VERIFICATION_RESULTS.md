# 写时重排验证结果

> **创建时间**: 2026-01-05  
> **状态**: 验证中

---

## 验证总结

### ✅ 基础功能验证通过

1. **Pass 注册**: ✅ 成功
   - `--systolic-dataflow-generation` 出现在 `systolic-opt --help` 中

2. **编译**: ✅ 成功
   - 所有代码编译通过
   - 分析器和代码生成器都已集成

3. **代码生成**: ✅ 正常工作
   - `systolic-translate` 能成功生成 HLS C++ 代码
   - 生成的代码包含正确的函数签名和数组声明

### ⚠️ 待验证功能

1. **非线性访问检测**
   - 需要验证 `WriteTimeReorderingAnalyzer` 能否正确检测非线性索引
   - 测试用例: `test/test_reordering_detection.mlir`

2. **重排属性生成**
   - 需要验证 pass 是否能生成 `systolic.reorder.*` 属性
   - 可能需要先运行 `systolic-transform` pass

3. **重排应用**
   - 需要验证代码生成器是否正确读取和应用重排属性

---

## 测试用例

### 1. 简单矩阵乘法（无重排）

**文件**: `test/mm_32x32.mlir`

**结果**: ✅ 代码生成成功
- 数组声明: `local_A[4][1]` (简单布局，无重排)

### 2. 非线性访问测试（应有重排）

**文件**: `test/test_reordering_detection.mlir`

**特征**:
- 写入: `local_A[%i, %j, %k]` (线性)
- 读取: `local_A[%i, %j, %idx]` 其中 `%idx = (k * 8) floordiv 16` (非线性)

**预期**:
- 检测到非线性访问
- 生成重排属性: `systolic.reorder.A.dims = [2, 16, 16]`
- 生成重排属性: `systolic.reorder.A.perm = [1, 2, 0]`

**状态**: ⚠️ 待验证（需要运行 pass 并检查属性）

---

## 验证步骤

### 步骤 1: 运行分析 Pass

```bash
build/bin/systolic-opt test/test_reordering_detection.mlir \
  -systolic-dataflow-generation \
  -o output.mlir
```

### 步骤 2: 检查重排属性

```bash
grep "systolic.reorder" output.mlir
```

**预期输出**:
```mlir
func.func @kernel0(...) attributes {
  systolic.reorder.A.dims = [2, 16, 16]
  systolic.reorder.A.perm = [1, 2, 0]
} {
  ...
}
```

### 步骤 3: 生成 HLS 代码

```bash
build/bin/systolic-translate output.mlir \
  -array-part=16 \
  -latency=8 \
  -simd=1 \
  -size=16 \
  -o kernel.cpp
```

### 步骤 4: 检查重排后的数组声明

```bash
grep "local_A\[" kernel.cpp
```

**预期输出**:
```cpp
void A_IO_L2_in_intra_trans(..., A_t16 local_A[2][16][16], ...);
```

---

## 当前状态

- ✅ Pass 注册成功
- ✅ 编译成功
- ✅ 代码生成器集成成功
- ⚠️ 分析器功能待验证（需要运行 pass）
- ⚠️ 重排属性生成待验证
- ⚠️ 重排应用待验证

---

## 发现的问题

### 1. Dialect 注册问题

**现象**: 运行 `systolic-dataflow-generation` pass 时出现错误：
```
LLVM ERROR: Building op `systolic.dataflow.io.module` but it isn't registered
```

**可能原因**:
- Pass 需要先运行 `systolic-transform` 来设置必要的上下文
- 或者需要确保 dialect 在运行 pass 前已加载

**解决方法**:
- 先运行 `systolic-transform`，再运行 `systolic-dataflow-generation`
- 或者修改 pass 使其不依赖 `systolic-transform` 的输出

---

## 下一步

1. **修复 Dialect 注册问题**
   - 检查 pass 的依赖关系
   - 确保 dialect 正确注册

2. **验证分析器功能**
   - 运行 pass 并检查是否检测到非线性访问
   - 验证重排属性是否正确生成

3. **验证代码生成**
   - 检查代码生成器是否正确读取属性
   - 验证重排后的数组声明和访问模式

---

## 相关文档

- [Pass 注册状态总结](PASS_STATUS_SUMMARY.md)
- [验证总结](VERIFICATION_SUMMARY.md)
- [写时重排实现进度](../docs/WRITE_TIME_REORDERING_IMPLEMENTATION.md)


