# Spacetime 动态枚举功能

> **最后更新**: 2026-01-06  
> **状态**: ✅ 已实现并测试通过

---

## 概述

mlir-systolic 现在支持动态枚举所有可能的 spacetime 配置，类似于 AutoSA 的行为。系统会根据循环数量自动生成所有可能的配置，并允许用户选择或自动选择。

### 核心功能

- ✅ **动态枚举**: 根据循环数量自动生成所有可能的配置
- ✅ **自动选择**: 默认优先选择 ST3（3-loop MM）
- ✅ **配置索引**: 每个配置分配递增的 ID（从 0 开始）
- ✅ **循环置换**: 自动将 space loops 移到最外层

---

## 快速开始

### 基本使用

```bash
# 自动选择配置（默认）
systolic-opt input.mlir --systolic-transform
```

**输出示例**:
```
[Systolic] Spacetime enumeration: Found 6 configurations, selected [3] ST3
```

### 配置数量

不同循环数量的配置数量：

| 循环数 | 1D | 2D | 3D | 总计 |
|--------|----|----|----|------|
| 3 (MM) | 3 | 3 | - | 6 |
| 4 (MTTKRP) | 4 | 6 | 4 | 14 |
| 5 (CNN) | 5 | 10 | 10 | 25 |

---

## 实现细节

### 枚举算法

```cpp
// 1D: C(N,1) = N 种
// 2D: C(N,2) = N*(N-1)/2 种
// 3D: C(N,3) = N*(N-1)*(N-2)/6 种
```

### 代码位置

- **枚举函数**: `lib/Transforms/SystolicTransform.cpp::enumerateSpaceTimeConfigs()`
- **配置框架**: `include/systolic/Analysis/ParametricSpaceTime.h`
- **选择逻辑**: `lib/Transforms/SystolicTransform.cpp::runOnOperation()`

---

## 测试结果

✅ **测试通过** (2026-01-06)

- 3 循环 kernel: 成功枚举 6 种配置
- 默认选择: ST3（符合预期）
- 配置信息: 正确存储到函数属性

详见: [测试结果](../../SPACETIME_TEST_RESULTS.md)

---

## 相关文档

- **[对比分析](../SPACETIME_COMPARISON.md)** - 与 AutoSA 的详细对比
- **[使用指南](../SPACETIME_USAGE.md)** - 详细使用说明
- **[实现状态](../SPACETIME_IMPLEMENTATION_STATUS.md)** - 实现状态和代码变更

---

## 下一步

1. **扩展测试**: 4 循环和 5 循环 kernel 测试
2. **命令行选项**: 添加用户友好的命令行选项
3. **性能优化**: 优化枚举算法的性能
