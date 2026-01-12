# mlir-systolic 文档导航

> **最后更新**: January 2026  
> **版本**: 3.1 (文档重组完成)

---

## 📋 快速导航

### 🚀 快速开始
1. **[../README.md](../README.md)** — 项目概述
2. **[guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)** — 构建和安装
3. **[guide/DEVELOPMENT_GUIDE.md](guide/DEVELOPMENT_GUIDE.md)** — 开发指南

### 📚 核心文档
- **[ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)** — 系统架构
- **[guide/CODE_STRUCTURE.md](guide/CODE_STRUCTURE.md)** — 代码组织
- **[status/IMPLEMENTATION_STATUS.md](status/IMPLEMENTATION_STATUS.md)** ⭐ **最新** — 实现状态总结
- **[reference/PROJECT_STRUCTURE.md](reference/PROJECT_STRUCTURE.md)** — 项目目录结构
- **[../scripts/README.md](../scripts/README.md)** — Scripts 使用指南

### 📊 项目状态
- **[status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)** — 当前状态
- **[status/ROADMAP.md](status/ROADMAP.md)** — 技术路线图
- **[status/NEXT_STEPS_ROADMAP.md](status/NEXT_STEPS_ROADMAP.md)** ⭐ **最新** — 下一步工作路线图
- **[status/IMPLEMENTATION_STATUS.md](status/IMPLEMENTATION_STATUS.md)** ⭐ — 实现状态总结

### 🎯 Spacetime 功能
- **[SPACETIME_INDEX.md](SPACETIME_INDEX.md)** ⭐ **入口** — Spacetime 功能文档索引
- **[features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md](features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md)** ⭐ **最新** — 动态枚举功能（已实现）

---

## 🧩 核心特性

### Polymer 优化框架
- **[features/polymer/README.md](features/polymer/README.md)** — 概述
- **[features/polymer/POLYMER_QUICK_START.md](features/polymer/POLYMER_QUICK_START.md)** — 快速入门
- **[features/polymer/POLYMER_INTEGRATION_COMPLETE.md](features/polymer/POLYMER_INTEGRATION_COMPLETE.md)** — 集成完成报告

### Space-Time 数据流架构
- **[features/spacetime/README.md](features/spacetime/README.md)** ⭐ — 文档导航
- **[features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md](features/spacetime/SPACETIME_DYNAMIC_ENUMERATION.md)** — 动态枚举功能（已实现）
- **[features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md)** — 实现计划

### 写时重排序优化
- **[features/write-time-reordering/README.md](features/write-time-reordering/README.md)** — 概述
- **[features/write-time-reordering/PHASE2_IMPLEMENTATION_SUMMARY.md](features/write-time-reordering/PHASE2_IMPLEMENTATION_SUMMARY.md)** — Phase 2总结
- **[features/write-time-reordering/WRITE_TIME_REORDERING_IMPLEMENTATION.md](features/write-time-reordering/WRITE_TIME_REORDERING_IMPLEMENTATION.md)** — 实现细节

### AutoSA 集成与参考样例
- **[reference/autosa/AUTOSA_QUICK_REFERENCE.md](reference/autosa/AUTOSA_QUICK_REFERENCE.md)** 🚀 **一键导航** — 97 个样例速览与快速命令
- **[reference/autosa/AUTOSA_REFERENCE_STATUS.md](reference/autosa/AUTOSA_REFERENCE_STATUS.md)** ⭐ **最新** — 参考样例覆盖情况与生成策略
- **[reference/autosa/AUTOSA_REFERENCE_TABLES.md](reference/autosa/AUTOSA_REFERENCE_TABLES.md)** ⭐ **最新** — 参考参数详细表格（按 kernel 分组）
- **[reference/autosa/AUTOSA_REFERENCE_TABLES.csv](reference/autosa/AUTOSA_REFERENCE_TABLES.csv)** — 参数表 CSV 格式
- **[autosa/README.md](autosa/README.md)** — 概述
- **[autosa/REORGANIZATION_COMPLETION_REPORT.md](autosa/REORGANIZATION_COMPLETION_REPORT.md)** — HLS参考文件整理

### 📦 归档与备份
- **[archive/README.md](archive/README.md)** — 归档导航与说明
- **[archive/FINAL_GENERATION_REPORT.md](archive/FINAL_GENERATION_REPORT.md)** — 生成报告（历史）
- **[archive/obsolete/](archive/obsolete/)** — 过期文档

---

## 🔍 参考资料

- **[reference/autosa/](reference/autosa/)** — AutoSA 分析和集成
- **[reference/allo/](reference/allo/)** — Allo HLS代码生成
- **[reference/testing/](reference/testing/)** — 测试参考

---

## 🐛 已知问题

- **[issues/README.md](issues/README.md)** — 问题跟踪和分析

---

## 📁 文档结构（已整理）

```
docs/
├── README.md                                    # 本文件（文档导航）
├── ARCHITECTURE_OVERVIEW.md                     # 架构总览
│
├── guide/                                       # 开发指南
│   ├── BUILD_GUIDE.md                           # 构建指南
│   ├── DEVELOPMENT_GUIDE.md                     # 开发指南
│   └── CODE_STRUCTURE.md                        # 代码结构 ⭐ 已整理
│
├── reference/                                   # 参考资料
│   ├── PROJECT_STRUCTURE.md                     # 项目结构 ⭐ 已整理
│   ├── autosa/                                  # AutoSA 参考
│   │   ├── README.md
│   │   ├── AUTOSA_QUICK_REFERENCE.md            # ⭐ 已整理
│   │   ├── AUTOSA_REFERENCE_STATUS.md           # ⭐ 已整理
│   │   ├── AUTOSA_REFERENCE_TABLES.md           # ⭐ 已整理
│   │   ├── AUTOSA_REFERENCE_TABLES.csv          # ⭐ 已整理
│   │   ├── AUTOSA_ANALYSIS.md
│   │   ├── AUTOSA_ARCHITECTURE.md
│   │   └── comparison_with_autosa.md
│   ├── allo/                                    # Allo 参考
│   │   ├── README.md
│   │   └── ALLO_HLS_CODE_GENERATION_RULES.md
│   └── testing/                                 # 测试参考
│       ├── README.md
│       └── REFERENCE_SAMPLES.md
│
├── status/                                      # 项目状态
│   ├── PROJECT_STATUS.md                        # 项目状态
│   ├── ROADMAP.md                               # 技术路线图
│   ├── IMPLEMENTATION_STATUS.md                 # 实现状态 ⭐ 已整理
│   └── NEXT_STEPS_ROADMAP.md                    # 下一步路线 ⭐ 已整理
│
├── autosa/                                      # AutoSA 集成
│   ├── README.md
│   └── REORGANIZATION_COMPLETION_REPORT.md
│
├── features/                                    # 核心特性
│   ├── polymer/                                 # Polymer 优化
│   │   ├── README.md
│   │   ├── POLYMER_QUICK_START.md
│   │   └── POLYMER_INTEGRATION_COMPLETE.md
│   ├── spacetime/                               # Space-Time 架构
│   │   ├── README.md
│   │   ├── SPACETIME_DYNAMIC_ENUMERATION.md
│   │   └── SPACETIME_IMPLEMENTATION_PLAN.md
│   └── write-time-reordering/                   # 写时重排序
│       ├── README.md
│       ├── PHASE2_IMPLEMENTATION_SUMMARY.md
│       └── WRITE_TIME_REORDERING_IMPLEMENTATION.md
│
├── issues/                                      # 已知问题
│   └── README.md
│
└── archive/                                     # 📦 归档
    ├── README.md                                # 归档导航
    ├── FINAL_GENERATION_REPORT.md               # 生成报告
    └── obsolete/                                # 过期文档位置
```

---

## 📖 使用建议

### 第一次接触项目？
1. 阅读 [../README.md](../README.md)
2. 查看 [ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)
3. 按照 [guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md) 构建

### 想深入了解某个特性？
- Polymer → [features/polymer/README.md](features/polymer/README.md)
- Space-Time → [features/spacetime/README.md](features/spacetime/README.md)
- Write-Time-Reordering → [features/write-time-reordering/README.md](features/write-time-reordering/README.md)

### 参考AutoSA或Allo？
- [reference/autosa/](reference/autosa/)
- [reference/allo/](reference/allo/)

### 遇到问题？
- [issues/README.md](issues/README.md)

---

## ✨ 最近更新

**2026-01-12**：完成 docs 文件重组
- 将 `CODE_STRUCTURE.md` 移至 `guide/`
- 将 `PROJECT_STRUCTURE.md` 移至 `reference/`
- 将 `IMPLEMENTATION_STATUS.md` 和 `NEXT_STEPS_ROADMAP.md` 移至 `status/`
- 将 `AUTOSA_QUICK_REFERENCE.md` 等 AutoSA 相关文件移至 `reference/autosa/`
- 将 `FINAL_GENERATION_REPORT.md` 归档至 `archive/`
- 创建 `scripts/README.md` 整合所有脚本说明
- 删除过时的 scripts 相关文档

