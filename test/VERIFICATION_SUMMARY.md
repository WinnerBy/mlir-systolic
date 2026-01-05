# 写时重排验证总结

> **创建时间**: 2026-01-05  
> **状态**: 验证中

---

## 验证结果

### ✅ 基础功能验证

1. **代码生成器工作正常**
   - `systolic-translate` 能成功生成 HLS C++ 代码
   - 生成的代码包含正确的函数签名和数组声明
   - 测试文件: `test/output/write_time_reordering_test/output.cpp`

2. **测试脚本运行成功**
   - `test/verify_reordering_simple.sh` 成功执行
   - 能够创建测试 MLIR 文件并生成代码

### ⚠️ 待验证功能

1. **非线性访问检测**
   - 需要验证 `WriteTimeReorderingAnalyzer` 能否正确检测非线性索引
   - 测试用例: `test/test_reordering_with_nonlinear.mlir`
   - 状态: 需要运行完整 pipeline 验证

2. **重排信息传递**
   - 需要验证重排信息是否正确存储到函数属性
   - 需要验证代码生成器是否正确读取属性
   - 状态: 需要检查中间 MLIR 文件

3. **重排应用**
   - 需要验证生成的代码是否使用重排后的数组布局
   - 需要验证索引访问是否正确映射
   - 状态: 当前生成的代码仍使用简单布局

---

## 验证步骤

### 步骤 1: 检查分析器集成

```bash
# 检查 pass 是否注册
build/bin/systolic-opt --help | grep dataflow

# 运行分析 pass
build/bin/systolic-opt test.mlir -systolic-dataflow-generation -o output.mlir

# 检查是否有重排属性
grep "systolic.reorder" output.mlir
```

### 步骤 2: 检查代码生成器

```bash
# 生成代码
build/bin/systolic-translate output.mlir -o kernel.cpp

# 检查数组声明
grep "local_A\[" kernel.cpp

# 检查访问模式
grep -A 2 "local_A\[" kernel.cpp
```

### 步骤 3: 对比参考代码

```bash
# 查看 AutoSA 生成的 MTTKRP 代码
grep "local_A\[" autosa_hls_output/kernel_mttkrp.cpp | head -10
```

---

## 发现的问题

### 问题 1: Pass 可能未正确注册

**现象**: `systolic-opt --help` 中没有显示 `systolic-dataflow-generation` pass

**可能原因**:
- Pass 注册名称可能不同
- Pass 可能未正确链接

**解决方法**:
- 检查 `PassRegistration` 的注册名称
- 检查 CMakeLists.txt 中的链接配置

### 问题 2: 生成的代码未应用重排

**现象**: 生成的代码使用 `local_A[8][1]` 而不是 `local_A[2][16][16]`

**可能原因**:
1. 分析器未检测到非线性访问
2. 重排信息未正确存储到属性
3. 代码生成器未正确读取属性
4. 代码生成器未正确应用重排

**解决方法**:
- 添加调试输出检查分析结果
- 检查中间 MLIR 文件中的属性
- 检查代码生成器中的属性读取逻辑

---

## 下一步验证计划

### 1. 调试分析器

```cpp
// 在 WriteTimeReorderingAnalyzer::analyze() 中添加调试输出
LLVM_DEBUG(llvm::dbgs() << "Analyzing pattern for " << pattern.arrayName << "\n");
LLVM_DEBUG(llvm::dbgs() << "Has non-linear access: " << pattern.hasNonLinearAccess << "\n");
```

### 2. 检查中间结果

```bash
# 运行完整 pipeline 并保存中间结果
build/bin/systolic-opt test.mlir \
  -systolic-transform \
  -systolic-dataflow-generation \
  -o intermediate.mlir

# 检查属性
grep -A 5 "systolic.reorder" intermediate.mlir
```

### 3. 验证代码生成器读取逻辑

```cpp
// 在 SystolicHLSEmitter::emit() 中添加调试输出
// 检查是否读取到重排属性
```

---

## 测试文件

- **简单测试**: `test/verify_reordering_simple.sh`
- **非线性访问测试**: `test/test_reordering_with_nonlinear.mlir`
- **完整测试**: `test/test_write_time_reordering.sh`

---

## 相关文档

- [写时重排实现进度](../docs/WRITE_TIME_REORDERING_IMPLEMENTATION.md)
- [基于多面体模型的实现](../docs/WRITE_TIME_REORDERING_POLYHEDRAL.md)
- [手动验证指南](verify_reordering_manual.md)


