# mlir-systolic 文档索引

> **最后更新**: 2024  
> **文档已重新整理和分类**
---

- **[NEXT_STEPS_TECHNICAL_ROADMAP.md](../NEXT_STEPS_TECHNICAL_ROADMAP.md)** ⭐ - 技术路线图和实施计划
- **[CLEANUP_CHECKLIST.md](../CLEANUP_CHECKLIST.md)** - 文档清理检查清单
- **[CODE_STRUCTURE.md](CODE_STRUCTURE.md)** ⭐ - 代码结构和组织
- **[PROJECT_STRUCTURE.md](../PROJECT_STRUCTURE.md)** - 项目结构说明
- **[test/TESTING_GUIDE.md](../test/TESTING_GUIDE.md)** ⭐ - 完整的测试指南

---
# mlir-systolic 文档索引

> **最后更新**: 2026-01-06  
> **说明**: 子目录已重新分组，避免“project/”“autosa/”“testing/”等旧路径带来的混乱。

---

## 📚 核心文档（必读）

### 项目分析与规划
- **[整理工作总结.md](../整理工作总结.md)** ⭐ 总览与结论
- **[PROJECT_ORGANIZATION_AND_ANALYSIS.md](../PROJECT_ORGANIZATION_AND_ANALYSIS.md)** ⭐ 项目组织与分析
- **[CODE_ISSUES_DETAILED_ANALYSIS.md](../CODE_ISSUES_DETAILED_ANALYSIS.md)** ⭐ 代码问题详解
- **[NEXT_STEPS_TECHNICAL_ROADMAP.md](../NEXT_STEPS_TECHNICAL_ROADMAP.md)** ⭐ 技术路线图
- **[CLEANUP_CHECKLIST.md](../CLEANUP_CHECKLIST.md)** 与 **[CLEANUP_COMPLETE.md](../CLEANUP_COMPLETE.md)**

### 架构与代码
- **[ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)** ⭐ 架构概览
- **[CODE_STRUCTURE.md](CODE_STRUCTURE.md)** ⭐ 代码结构与问题清单
- **[PROJECT_STRUCTURE.md](../PROJECT_STRUCTURE.md)** 项目目录结构说明

### 构建与测试
- **[guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)** ⭐ 构建/依赖/脚本合并版
- **[guide/DEVELOPMENT_GUIDE.md](guide/DEVELOPMENT_GUIDE.md)** 开发指南
- **[test/TESTING_GUIDE.md](../test/TESTING_GUIDE.md)** ⭐ 测试指南
- **[test/TEST_STATUS.md](../test/TEST_STATUS.md)** 测试现状

### 状态与路线图
- **[status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)** ⭐ 当前状态（精简）
- **[status/ROADMAP.md](status/ROADMAP.md)** 阶段目标与测试标准

---

## 🗂️ 目录概览（新分组）

```
docs/
├── guide/                     # 构建 & 开发指南
├── status/                    # 状态与路线图
├── features/                  # 功能实现详解
│   ├── spacetime/             # Space-time 实现与 ST3 报告
│   └── write-time-reordering/ # 写时重排（分析/实现/代码）
├── reference/                 # 参考资料
│   ├── autosa/                # AutoSA 参考与对比
│   ├── allo/                  # Allo 集成
│   └── testing/               # 测试结果与样本
├── issues/                    # 问题分析（SIMD2、数据重用等）
└── archive/                   # 历史归档（2024-12/…）
```

---

## 🔎 快速导航

- 构建/依赖 → [guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)
- 开发指南 → [guide/DEVELOPMENT_GUIDE.md](guide/DEVELOPMENT_GUIDE.md)
- 当前状态 → [status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)
- 路线图 → [status/ROADMAP.md](status/ROADMAP.md)
- Space-time → [features/spacetime/](features/spacetime/)
- 写时重排 → [features/write-time-reordering/](features/write-time-reordering/)
- AutoSA 参考 → [reference/autosa/](reference/autosa/)
- Allo 集成 → [reference/allo/](reference/allo/)
- 测试结果/样本 → [reference/testing/](reference/testing/)
- 问题分析 → [issues/](issues/)

---

## 📖 推荐阅读路径

### 新人 30 分钟
1. [整理工作总结.md](../整理工作总结.md)
2. [ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)
3. [guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)
4. [test/TESTING_GUIDE.md](../test/TESTING_GUIDE.md)

### 开发者 1 小时
1. [CODE_STRUCTURE.md](CODE_STRUCTURE.md)
2. [status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)
3. [status/ROADMAP.md](status/ROADMAP.md)
4. [features/spacetime/](features/spacetime/)
5. [features/write-time-reordering/](features/write-time-reordering/)

### 管理/规划
1. [PROJECT_ORGANIZATION_AND_ANALYSIS.md](../PROJECT_ORGANIZATION_AND_ANALYSIS.md)
2. [NEXT_STEPS_TECHNICAL_ROADMAP.md](../NEXT_STEPS_TECHNICAL_ROADMAP.md)
3. [status/ROADMAP.md](status/ROADMAP.md)

---

## 🧭 说明

- 旧目录 `project/`、`autosa/`、`testing/`、`other-issues/` 已拆分到新的分组，避免混用。
- 历史文档已整体下沉到 `archive/2024-12/`，默认不再展示。
- 路径更新后，如遇外部链接失效，可通过 `git log` 追溯旧路径或从 `archive/` 查找。
- **查看下一阶段工作计划** → [project/NEXT_PHASE_WORK_PLAN.md](project/NEXT_PHASE_WORK_PLAN.md)
