# Spacetime 功能文档索引

> **最后更新**: 2026-01-06  
> **状态**: ✅ 核心功能已实现并测试通过

---

## 📋 文档分类

### 🚀 快速开始

- **[features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md](features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md)** ⭐ **推荐**
  - 功能概述和快速开始
  - 基本使用示例

### 📖 使用指南

- **[SPACETIME_USAGE.md](SPACETIME_USAGE.md)**
  - 详细使用说明
  - 配置选项和示例

- **[SPACETIME_ANALYSIS.md](SPACETIME_ANALYSIS.md)**
  - 不同循环数量的映射规则
  - Spacetime 映射分析

### 🔍 技术分析

- **[SPACETIME_COMPARISON.md](SPACETIME_COMPARISON.md)**
  - AutoSA vs mlir-systolic 详细对比
  - 关键差异和调整方案

- **[SPACETIME_IMPLEMENTATION_STATUS.md](SPACETIME_IMPLEMENTATION_STATUS.md)**
  - 实现状态和代码变更
  - 待完成工作

- **[features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md)**
  - 详细实现计划
  - 实现步骤和代码示例

### ✅ 测试验证

- **[SPACETIME_TEST_RESULTS.md](SPACETIME_TEST_RESULTS.md)** ⭐ **最新**
  - 测试结果和验证检查点
  - 性能指标

- **[SPACETIME_TESTING.md](SPACETIME_TESTING.md)**
  - 测试用例和验证方法
  - 测试脚本说明

---

## 🎯 推荐阅读路径

### 第一次了解功能

1. [features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md](features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md) - 功能概述
2. [SPACETIME_TEST_RESULTS.md](SPACETIME_TEST_RESULTS.md) - 测试结果

### 想要使用功能

1. [SPACETIME_USAGE.md](SPACETIME_USAGE.md) - 使用指南
2. [SPACETIME_ANALYSIS.md](SPACETIME_ANALYSIS.md) - 配置分析

### 深入理解实现

1. [SPACETIME_COMPARISON.md](SPACETIME_COMPARISON.md) - 与 AutoSA 对比
2. [SPACETIME_IMPLEMENTATION_STATUS.md](SPACETIME_IMPLEMENTATION_STATUS.md) - 实现状态
3. [features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md) - 实现计划

---

## 📊 功能状态总结

✅ **已完成**:
- 动态枚举所有可能的配置
- 自动选择默认配置（ST3 for MM）
- 循环置换功能
- 3 循环 kernel 测试通过

🟡 **部分完成**:
- 命令行选项支持（可通过代码设置）

⏳ **待完成**:
- 4 循环和 5 循环 kernel 测试
- 完整的命令行选项支持

---

## 🔗 相关文档

- **项目状态**: [status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)
- **下一步工作**: [NEXT_STEPS_ROADMAP.md](NEXT_STEPS_ROADMAP.md)
- **实现状态**: [IMPLEMENTATION_STATUS.md](IMPLEMENTATION_STATUS.md)
