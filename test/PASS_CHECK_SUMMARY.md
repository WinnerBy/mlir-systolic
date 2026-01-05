# Pass 注册检查总结

> **创建时间**: 2026-01-05  
> **状态**: 编译错误修复中

---

## 问题总结

### 1. Pass 未在帮助信息中显示

**现象**: `systolic-opt --help` 中只显示 `--systolic-transform`，没有 `--systolic-dataflow-generation`

**可能原因**:
- Pass 使用 `static PassRegistration`，但链接器移除了未使用的符号
- Pass 未在 `registerSystolicPasses()` 中显式引用

### 2. 编译错误

**错误类型**:
1. `func.getArguments()` 调用错误 - 已修复（添加 `#include "mlir/Dialect/Func/IR/FuncOps.h"`）
2. `ArrayAccessPattern` 中使用 `AffineLoadOp`/`AffineStoreOp` 导致不完整类型错误 - 已修复（改为 `Operation*`）
3. `ArrayRefGroup` 未声明 - 已修复（移除未使用的方法声明）

---

## 修复措施

### 1. Pass 注册

**文件**: `lib/Transforms/SystolicTransform.cpp`

```cpp
void registerSystolicPasses() {
  PassRegistration<SystolicTransformPass>();
  // 显式引用确保链接
  (void)createSystolicDataflowGenerationPass();
}
```

### 2. 头文件包含

**文件**: `include/systolic/Analysis/WriteTimeReorderingAnalysis.h`

```cpp
#include "mlir/Dialect/Func/IR/FuncOps.h"  // 添加
```

### 3. 类型定义修复

**文件**: `include/systolic/Analysis/WriteTimeReorderingAnalysis.h`

```cpp
// 修改前
SmallVector<affine::AffineLoadOp, 8> loads;
SmallVector<affine::AffineStoreOp, 8> stores;

// 修改后
SmallVector<Operation *, 8> loadOps;
SmallVector<Operation *, 8> storeOps;
```

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

### 3. 功能测试

```bash
# 运行 pass
build/bin/systolic-opt test.mlir -systolic-dataflow-generation -o output.mlir

# 检查重排属性
grep "systolic.reorder" output.mlir
```

---

## 当前状态

- ✅ 头文件包含已修复
- ✅ 类型定义已修复
- ⚠️ 编译中（需要验证是否成功）
- ⚠️ Pass 注册待验证

---

## 下一步

1. 完成编译
2. 验证 pass 是否出现在帮助信息中
3. 运行功能测试


