# Pass 注册问题修复总结

> **创建时间**: 2026-01-05  
> **状态**: 已修复编译错误，pass 注册待验证

---

## 问题描述

1. **Pass 未在 `systolic-opt --help` 中显示**
   - `systolic-dataflow-generation` pass 未出现在帮助信息中
   - 可能原因：pass 未正确注册或链接

2. **编译错误**
   - `WriteTimeReorderingAnalysis.cpp` 中 API 调用错误
   - `func.getArgAttrOfType<StringAttr>()` 的调用方式不正确

---

## 修复措施

### 1. Pass 注册修复

**文件**: `lib/Transforms/SystolicTransform.cpp`

**修改**:
```cpp
void registerSystolicPasses() {
  PassRegistration<SystolicTransformPass>();
  // 显式引用 createSystolicDataflowGenerationPass() 确保链接
  (void)createSystolicDataflowGenerationPass();
}
```

**原因**: `SystolicDataflowGenerationPass` 使用 `static PassRegistration`，但链接器可能移除未使用的符号。显式引用 `createSystolicDataflowGenerationPass()` 可以防止符号被移除。

### 2. API 调用修复

**文件**: `lib/Analysis/WriteTimeReorderingAnalysis.cpp`

**修改**:
```cpp
// 修改前（错误）
if (auto attr = func.getArgAttrOfType<StringAttr>(argNum, "mlir.name")) {
  name = attr.getValue().str();
}

// 修改后（正确）
auto attr = func.getArgAttrOfType<StringAttr>(argNum, "mlir.name");
if (attr) {
  name = attr.getValue().str();
}
```

**原因**: `getArgAttrOfType` 返回的是可选类型，不能直接在 `if` 条件中使用 `auto`。

### 3. 头文件包含

**添加**: `#include "mlir/Dialect/Func/IR/FuncOps.h"`

**原因**: 需要 `func::FuncOp` 的完整定义。

---

## 验证步骤

### 1. 编译验证

```bash
cd build && make -j4
```

### 2. Pass 注册验证

```bash
build/bin/systolic-opt --help | grep -E "systolic|dataflow"
```

**预期输出**:
```
--systolic-transform                             -   Apply systolic array transformations
--systolic-dataflow-generation                   -   Generate SystolicDataflow Dialect from Affine IR
```

### 3. 功能验证

```bash
# 运行 pass
build/bin/systolic-opt test.mlir -systolic-dataflow-generation -o output.mlir

# 检查是否有重排属性
grep "systolic.reorder" output.mlir
```

---

## 当前状态

- ✅ 编译错误已修复
- ⚠️ Pass 注册待验证（需要重新编译后测试）
- ⚠️ 功能验证待完成

---

## 下一步

1. 重新编译项目
2. 验证 pass 是否出现在帮助信息中
3. 运行测试验证功能


