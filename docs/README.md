# mlir-systolic 文档索引

> **最后更新**: 2026-01-07  \
> **说明**: 完成 Polymer 文档归类、Phase 进展归档，路径已校正。

---

## 📚 核心文档（必读）
- **[整理工作总结.md](整理工作总结.md)** — 项目概览与结论
- **[PROJECT_ORGANIZATION_AND_ANALYSIS.md](PROJECT_ORGANIZATION_AND_ANALYSIS.md)** — 项目组织与分析
- **[CODE_ISSUES_DETAILED_ANALYSIS.md](CODE_ISSUES_DETAILED_ANALYSIS.md)** — 代码问题详解
- **[NEXT_STEPS_TECHNICAL_ROADMAP.md](NEXT_STEPS_TECHNICAL_ROADMAP.md)** — 技术路线图
- **[快速参考卡.md](快速参考卡.md)** — 快速查阅卡
- **[整理工作文件索引.md](整理工作文件索引.md)** — 整理产出索引

## 🏗️ 架构与代码
- **[ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)** — 架构概览
- **[CODE_STRUCTURE.md](CODE_STRUCTURE.md)** — 代码结构与问题清单
- **[PROJECT_STRUCTURE.md](../PROJECT_STRUCTURE.md)** — 项目目录结构说明

## 🔧 构建与测试
- **[guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)** — 构建/依赖/脚本整合指南
- **[guide/DEVELOPMENT_GUIDE.md](guide/DEVELOPMENT_GUIDE.md)** — 开发指南
- **[../test/TESTING_GUIDE.md](../test/TESTING_GUIDE.md)** — 测试指南
- **[../test/TEST_STATUS.md](../test/TEST_STATUS.md)** — 测试现状

## 📈 状态与报告
- **[status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)** — 当前状态（精简）
- **[status/ROADMAP.md](status/ROADMAP.md)** — 阶段目标与测试标准
- **[status/PROJECT_COMPLETION_REPORT.md](status/PROJECT_COMPLETION_REPORT.md)** — Polymer/ISL 集成完成总结

## 🧩 专题 / 功能
- **Polymer 集成文档** → [features/polymer/POLYMER_DOCUMENTATION_INDEX.md](features/polymer/POLYMER_DOCUMENTATION_INDEX.md)（索引）
- **Space-time** → [features/spacetime/](features/spacetime/)
- **写时重排** → [features/write-time-reordering/](features/write-time-reordering/)
- **问题分析** → [issues/](issues/)
- **参考资料** → [reference/autosa/](reference/autosa/), [reference/allo/](reference/allo/), [reference/testing/](reference/testing/)

## 🗂️ 归档
- **[archive/README.md](archive/README.md)** — 归档说明与索引
- **近期归档**: [archive/2026-01-phase2/](archive/2026-01-phase2/)（Phase1/2 进展与清单）、[archive/2024-12/](archive/2024-12/)（历史实现）

---

## 🗂️ 目录概览（最新分组）

```
docs/
├── guide/                     # 构建 & 开发指南
├── status/                    # 状态、路线图、完成报告
├── features/                  # 功能/专题文档
│   ├── polymer/               # Polymer 全套文档（索引/快速开始/路线图…）
│   ├── spacetime/             # Space-time 实现与 ST3 报告
│   └── write-time-reordering/ # 写时重排（分析/实现/代码）
├── reference/                 # 参考资料（AutoSA/Allo/Testing）
├── issues/                    # 问题分析（SIMD2、数据重用等）
└── archive/                   # 历史归档（2024-12/，2026-01-phase2/…）
```

---

## 🔎 快速导航

- 构建/依赖 → [guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)
- 开发指南 → [guide/DEVELOPMENT_GUIDE.md](guide/DEVELOPMENT_GUIDE.md)
- 当前状态 → [status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)
- 路线图 → [status/ROADMAP.md](status/ROADMAP.md)
- 完成报告 → [status/PROJECT_COMPLETION_REPORT.md](status/PROJECT_COMPLETION_REPORT.md)
- Polymer 文档 → [features/polymer/POLYMER_DOCUMENTATION_INDEX.md](features/polymer/POLYMER_DOCUMENTATION_INDEX.md)
- Space-time → [features/spacetime/](features/spacetime/)
- 写时重排 → [features/write-time-reordering/](features/write-time-reordering/)
- AutoSA 参考 → [reference/autosa/](reference/autosa/)
- Allo 集成 → [reference/allo/](reference/allo/)
- 测试结果/样本 → [reference/testing/](reference/testing/)
- 问题分析 → [issues/](issues/)
- 历史归档 → [archive/README.md](archive/README.md)

---

## 📖 推荐阅读路径

### 新人 30 分钟
1. [整理工作总结.md](整理工作总结.md)
2. [ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)
3. [guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)
4. [test/TESTING_GUIDE.md](../test/TESTING_GUIDE.md)

### 开发者 1 小时
1. [CODE_STRUCTURE.md](CODE_STRUCTURE.md)
2. [status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)
3. [status/ROADMAP.md](status/ROADMAP.md)
4. [features/polymer/POLYMER_DOCUMENTATION_INDEX.md](features/polymer/POLYMER_DOCUMENTATION_INDEX.md)
5. [features/spacetime/](features/spacetime/)
6. [features/write-time-reordering/](features/write-time-reordering/)

### 管理/规划
1. [PROJECT_ORGANIZATION_AND_ANALYSIS.md](PROJECT_ORGANIZATION_AND_ANALYSIS.md)
2. [NEXT_STEPS_TECHNICAL_ROADMAP.md](NEXT_STEPS_TECHNICAL_ROADMAP.md)
3. [status/PROJECT_COMPLETION_REPORT.md](status/PROJECT_COMPLETION_REPORT.md)
4. [status/ROADMAP.md](status/ROADMAP.md)

---

## 🧭 说明

- 旧目录 `project/`、`autosa/`、`testing/`、`other-issues/` 已拆分进当前分组；历史版本请从 `archive/` 查找。
- Polymer 全套文档已归入 `features/polymer/`；Phase 进展类文档下沉到 `archive/2026-01-phase2/`。
- 路径更新后，如遇链接失效，可通过 `archive/` 或 `git log --follow` 追溯历史位置。
