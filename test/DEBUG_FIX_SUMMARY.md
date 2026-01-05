# 调试修复总结

> **创建时间**: 2026-01-05  
> **问题**: Pass 执行时出现 `systolic.dataflow.io.module.yield` 错误

---

## 问题描述

运行 `systolic-dataflow-generation` pass 时出现错误：
```
error: 'systolic.dataflow.io.module.yield' op must be the last operation in the parent block
```

## 根本原因

Pass 在创建 `IOModuleOp` 的 body block 时，yield 操作被放在了 block 的开头而不是末尾。这是因为代码使用了 `setInsertionPointToStart` 而不是 `setInsertionPointToEnd`。

## 修复方案

修改 `lib/Transforms/SystolicDataflowGeneration.cpp`:

**修复前**:
```cpp
builder.setInsertionPointToStart(bodyBlock);
builder.create<dataflow::IOModuleYieldOp>(loc);
```

**修复后**:
```cpp
builder.setInsertionPointToEnd(bodyBlock);
builder.create<dataflow::IOModuleYieldOp>(loc);
```

## 修复位置

1. **第530行**: Double buffer 分支的 yield 操作
2. **第542行**: 简单 IO module 分支的 yield 操作

## 验证结果

✅ Pass 现在可以成功运行
✅ IO module 正确创建
✅ 没有 yield 操作错误

## 下一步

1. 验证分析器是否能检测到非线性访问
2. 检查重排属性是否正确生成
3. 测试完整的 pipeline

---

## 相关文件

- `lib/Transforms/SystolicDataflowGeneration.cpp` - 修复的代码
- `test/mttkrp_affine_only.mlir` - 测试文件
- `test/output/mttkrp_test/affine_dataflow_fixed.mlir` - 生成的输出


