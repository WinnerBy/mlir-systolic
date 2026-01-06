# 调试状态

## 当前问题

### 运行时崩溃
- **位置**: `polymer::IslScop::initializeSymbolTable` 中调用 `cst->getNumDimAndSymbolVars()`
- **堆栈**: `PresburgerSpace::getNumDimAndSymbolVars()` 崩溃
- **症状**: 即使创建了只有 symbols 的 context，传递给它时仍然崩溃

### 已尝试的修复
1. ✅ 修复编译错误（删除不存在的头文件）
2. ✅ 按照 Polymer 的标准流程构建 context（先收集 symbols，再合并 domains）
3. ✅ 在调用 `initializeSymbolTable` 之前不合并 domains，只传递 symbols
4. ✅ 添加验证检查，确保 symbols 和 context 都是有效的

### 可能的原因
1. **Context 对象生命周期问题**: `contextForSymbolTable` 可能在传递时被移动或复制，导致内部状态不一致
2. **MLIR API 版本不匹配**: 我们使用的 MLIR API 可能与 Polymer 期望的版本不匹配
3. **Context 构建方式问题**: 即使按照 Polymer 的模式，我们的 context 构建可能仍有细微差别

### 下一步
1. 检查 `initializeSymbolTable` 的实现，看看它期望什么样的 context
2. 尝试使用 Polymer 的 `IslScopBuilder` 来构建 SCoP，而不是手动构建
3. 检查 MLIR 版本兼容性
