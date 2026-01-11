# Space-Time 数据流架构

> **目录**: `docs/features/spacetime/`  
> **状态**: ✅ 动态枚举功能已实现

---

## 📚 文档导航

### 核心文档

1. **[SPACETIME_DYNAMIC_ENUMERATION.md](SPACETIME_DYNAMIC_ENUMERATION.md)** ⭐ **最新**
   - 动态枚举功能概述
   - 快速开始和使用示例
   - 实现细节和测试结果

2. **[SPACETIME_IMPLEMENTATION_PLAN.md](SPACETIME_IMPLEMENTATION_PLAN.md)**
   - 基于 AutoSA 的详细实现计划
   - 实现步骤和注意事项

### 详细分析文档

3. **[../SPACETIME_COMPARISON.md](../SPACETIME_COMPARISON.md)**
   - AutoSA vs mlir-systolic 的详细对比
   - 关键差异和调整方案

4. **[../SPACETIME_ANALYSIS.md](../SPACETIME_ANALYSIS.md)**
   - 不同循环数量的映射规则
   - Spacetime 映射分析

5. **[../SPACETIME_USAGE.md](../SPACETIME_USAGE.md)**
   - 详细使用指南
   - 配置选项说明

### 测试文档

6. **[../SPACETIME_TEST_RESULTS.md](../SPACETIME_TEST_RESULTS.md)**
   - 测试结果和验证检查点
   - 性能指标

7. **[../SPACETIME_TESTING.md](../SPACETIME_TESTING.md)**
   - 测试用例和验证方法
   - 测试脚本说明

### 实现状态

8. **[../SPACETIME_IMPLEMENTATION_STATUS.md](../SPACETIME_IMPLEMENTATION_STATUS.md)**
   - 实现状态和代码变更总结
   - 待完成工作

---

## 🎯 快速开始

### 了解功能

1. 阅读 [SPACETIME_DYNAMIC_ENUMERATION.md](SPACETIME_DYNAMIC_ENUMERATION.md) 了解核心功能
2. 查看 [SPACETIME_TEST_RESULTS.md](../SPACETIME_TEST_RESULTS.md) 了解测试结果

### 使用功能

1. 参考 [SPACETIME_USAGE.md](../SPACETIME_USAGE.md) 了解如何使用
2. 查看 [SPACETIME_ANALYSIS.md](../SPACETIME_ANALYSIS.md) 了解不同配置

### 深入理解

1. 阅读 [SPACETIME_COMPARISON.md](../SPACETIME_COMPARISON.md) 了解与 AutoSA 的差异
2. 查看 [SPACETIME_IMPLEMENTATION_STATUS.md](../SPACETIME_IMPLEMENTATION_STATUS.md) 了解实现细节

---

## 📊 功能状态

| 功能 | 状态 | 说明 |
|------|------|------|
| 动态枚举 | ✅ | 支持任意循环数量 |
| 自动选择 | ✅ | 默认选择 ST3 |
| 循环置换 | ✅ | Space loops 移到最外层 |
| 测试验证 | ✅ | 3 循环测试通过 |
| 命令行选项 | 🟡 | 部分支持 |

---

## 🔗 相关文档

- **项目状态**: [../status/PROJECT_STATUS.md](../status/PROJECT_STATUS.md)
- **下一步工作**: [../NEXT_STEPS_ROADMAP.md](../NEXT_STEPS_ROADMAP.md)
- **实现状态**: [../IMPLEMENTATION_STATUS.md](../IMPLEMENTATION_STATUS.md)

---

**👉 推荐开始**: [SPACETIME_DYNAMIC_ENUMERATION.md](SPACETIME_DYNAMIC_ENUMERATION.md)
