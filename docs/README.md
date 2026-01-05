# mlir-systolic 文档索引

> **最后更新**: 2026-01-05  
> **文档已按事项/功能分类整理**

---

## 📚 文档目录结构

所有文档已按事项/功能分类整理到以下目录：

```
docs/
├── README.md (本文件)
├── DOCUMENT_ORGANIZATION.md (文档组织说明)
├── write-time-reordering/ (写时重排功能)
│   ├── README.md ⭐
│   ├── RANDOM_ACCESS_ANALYSIS.md (随机访问问题)
│   ├── DOCUMENTATION_CHECKLIST.md (文档清单)
│   └── ... (其他写时重排相关文档)
├── spacetime/ (Space-time 实现)
│   ├── README.md
│   ├── SPACETIME_IMPLEMENTATION_PLAN.md ⭐
│   └── ... (Space-time 相关文档)
├── autosa/ (AutoSA 参考)
│   ├── README.md
│   ├── AUTOSA_ANALYSIS.md ⭐
│   └── ... (AutoSA 相关文档)
├── project/ (项目基础)
│   ├── README.md
│   ├── BUILD_STEPS.md ⭐
│   ├── ARCHITECTURE.md
│   ├── DEVELOPMENT_GUIDE.md
│   ├── PROJECT_STATUS.md ⭐
│   └── ... (架构、构建、开发、规划、分析总结)
├── testing/ (测试验证)
│   ├── README.md
│   ├── TEST_RESULTS.md ⭐
│   └── REFERENCE_SAMPLES.md
└── other-issues/ (其他问题)
    ├── README.md
    ├── SIMD2_ISSUE_ANALYSIS.md
    └── DATA_REUSE_STRATEGY.md
```

⭐ = 推荐优先阅读

---

## 🚀 快速导航

### 新用户快速开始

1. **[快速参考](project/QUICK_REFERENCE.md)** ⭐ - 常用命令和参数配置
2. **[构建指南](project/BUILD_STEPS.md)** ⭐ - 详细的构建步骤
3. **[项目状态](project/PROJECT_STATUS.md)** ⭐ - 了解项目当前状态

### 按事项查找

- **写时重排** → [write-time-reordering/](write-time-reordering/) - 包括随机访问问题、实现方案等
- **Space-time 实现** → [spacetime/](spacetime/) - Space-time 实现和测试
- **AutoSA 参考** → [autosa/](autosa/) - AutoSA 分析和参考
- **项目基础** → [project/](project/) - 架构、构建、开发、规划
- **测试验证** → [testing/](testing/) - 测试结果和参考样本
- **其他问题** → [other-issues/](other-issues/) - SIMD2、数据重用等问题

---

## 📖 核心文档推荐

### 快速开始（必读）

| 文档 | 说明 | 位置 |
|------|------|------|
| **快速参考** | 常用命令和参数配置 | [project/QUICK_REFERENCE.md](project/QUICK_REFERENCE.md) |
| **构建指南** | 详细的构建步骤 | [project/BUILD_STEPS.md](project/BUILD_STEPS.md) |
| **项目状态** | 项目当前状态 | [project/PROJECT_STATUS.md](project/PROJECT_STATUS.md) |

### 核心功能文档

| 文档 | 说明 | 位置 |
|------|------|------|
| **架构设计** | 项目架构和设计理念 | [project/ARCHITECTURE.md](project/ARCHITECTURE.md) |
| **开发指南** | 开发流程和代码结构 | [project/DEVELOPMENT_GUIDE.md](project/DEVELOPMENT_GUIDE.md) |
| **写时重排** | 写时重排功能（包括随机访问问题） | [write-time-reordering/](write-time-reordering/) |
| **Space-time 实现** | Space-time 实现计划 | [spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](spacetime/SPACETIME_IMPLEMENTATION_PLAN.md) |
| **AutoSA 分析** | AutoSA 架构、算法分析 | [autosa/AUTOSA_ANALYSIS.md](autosa/AUTOSA_ANALYSIS.md) |

### 问题分析和测试

| 文档 | 说明 | 位置 |
|------|------|------|
| **随机访问问题** | 随机读取问题分析 | [write-time-reordering/RANDOM_ACCESS_ANALYSIS.md](write-time-reordering/RANDOM_ACCESS_ANALYSIS.md) |
| **测试结果** | 测试结果和下一步工作 | [testing/TEST_RESULTS.md](testing/TEST_RESULTS.md) |
| **SIMD2 问题** | SIMD=2 代码生成问题 | [other-issues/SIMD2_ISSUE_ANALYSIS.md](other-issues/SIMD2_ISSUE_ANALYSIS.md) |
| **数据重用策略** | 数据重用策略分析 | [other-issues/DATA_REUSE_STRATEGY.md](other-issues/DATA_REUSE_STRATEGY.md) |

---

## 🔍 文档查找指南

### 我想...

- **快速开始构建** → [project/BUILD_STEPS.md](project/BUILD_STEPS.md)
- **查找常用命令** → [project/QUICK_REFERENCE.md](project/QUICK_REFERENCE.md)
- **了解写时重排** → [write-time-reordering/README.md](write-time-reordering/README.md)
- **了解随机访问问题** → [write-time-reordering/RANDOM_ACCESS_ANALYSIS.md](write-time-reordering/RANDOM_ACCESS_ANALYSIS.md)
- **实现 Space-time** → [spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](spacetime/SPACETIME_IMPLEMENTATION_PLAN.md)
- **了解 AutoSA** → [autosa/AUTOSA_ANALYSIS.md](autosa/AUTOSA_ANALYSIS.md)
- **查看测试结果** → [testing/TEST_RESULTS.md](testing/TEST_RESULTS.md)
- **了解项目状态** → [project/PROJECT_STATUS.md](project/PROJECT_STATUS.md)
- **查看下一阶段工作计划** → [project/NEXT_PHASE_WORK_PLAN.md](project/NEXT_PHASE_WORK_PLAN.md)

---

## 📊 文档统计

| 分类 | 文档数量 | 主要用途 |
|------|---------|---------|
| **write-time-reordering/** | 13 | 写时重排功能（包括随机访问问题） |
| **spacetime/** | 8 | Space-time 实现和测试 |
| **autosa/** | 6 | AutoSA 参考和分析 |
| **project/** | 15+ | 架构、构建、开发、规划、分析总结 |
| **testing/** | 3 | 测试和验证 |
| **other-issues/** | 2 | 其他问题分析 |
| **总计** | **47+** | - |

---

## 📁 历史文档

过时的文档已移动到 `archive/` 目录，包括：
- 旧的构建指南（已合并到 BUILD_STEPS.md）
- 旧的实现总结（已过时）
- 旧的测试结果（已过时）
- 已完成的工作计划

如需查看历史文档，请访问 `docs/archive/` 目录。

---

## 📝 文档维护

- ✅ 文档已按事项/功能分类整理
- ✅ 每个分类都有 README.md 导航文档
- ✅ 核心文档保持更新，反映最新状态
- ✅ 过时文档已移动到 archive 目录
- ✅ 文档组织结构清晰，便于查找

**文档组织说明**：详见 [DOCUMENT_ORGANIZATION.md](DOCUMENT_ORGANIZATION.md)

---

## 🎯 推荐阅读路径

### 路径 A：新用户（30 分钟）
1. [project/QUICK_REFERENCE.md](project/QUICK_REFERENCE.md) (10 min)
2. [project/BUILD_STEPS.md](project/BUILD_STEPS.md) (15 min)
3. [project/PROJECT_STATUS.md](project/PROJECT_STATUS.md) (5 min)

### 路径 B：开发者（1 小时）
1. [project/ARCHITECTURE.md](project/ARCHITECTURE.md) (20 min)
2. [project/DEVELOPMENT_GUIDE.md](project/DEVELOPMENT_GUIDE.md) (20 min)
3. [project/NEXT_PHASE_WORK_PLAN.md](project/NEXT_PHASE_WORK_PLAN.md) (20 min)

### 路径 C：功能实现（2 小时）
1. [write-time-reordering/RANDOM_ACCESS_ANALYSIS.md](write-time-reordering/RANDOM_ACCESS_ANALYSIS.md) (20 min)
2. [write-time-reordering/README.md](write-time-reordering/README.md) (30 min)
3. [spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](spacetime/SPACETIME_IMPLEMENTATION_PLAN.md) (30 min)
4. [testing/TEST_RESULTS.md](testing/TEST_RESULTS.md) (30 min)

---

**👉 推荐开始**: [project/QUICK_REFERENCE.md](project/QUICK_REFERENCE.md)
