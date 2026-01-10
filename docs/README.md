# mlir-systolic 文档导航

> **最后更新**: January 2026  
> **版本**: 3.0 (文档整理)

---

## 📋 快速导航

### 🚀 快速开始
1. **[../README.md](../README.md)** — 项目概述
2. **[guide/BUILD_GUIDE.md](guide/BUILD_GUIDE.md)** — 构建和安装
3. **[guide/DEVELOPMENT_GUIDE.md](guide/DEVELOPMENT_GUIDE.md)** — 开发指南

### 📚 核心文档
- **[ARCHITECTURE_OVERVIEW.md](ARCHITECTURE_OVERVIEW.md)** — 系统架构
- **[CODE_STRUCTURE.md](CODE_STRUCTURE.md)** — 代码组织
- **[../PROJECT_STRUCTURE.md](../PROJECT_STRUCTURE.md)** — 项目目录结构

### 📊 项目状态
- **[status/PROJECT_STATUS.md](status/PROJECT_STATUS.md)** — 当前状态
- **[status/ROADMAP.md](status/ROADMAP.md)** — 技术路线图

---

## 🧩 核心特性

### Polymer 优化框架
- **[features/polymer/README.md](features/polymer/README.md)** — 概述
- **[features/polymer/POLYMER_QUICK_START.md](features/polymer/POLYMER_QUICK_START.md)** — 快速入门
- **[features/polymer/POLYMER_INTEGRATION_COMPLETE.md](features/polymer/POLYMER_INTEGRATION_COMPLETE.md)** — 集成完成报告

### Space-Time 数据流架构
- **[features/spacetime/README.md](features/spacetime/README.md)** — 概述
- **[features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md)** — 实现方案

### 写时重排序优化
- **[features/write-time-reordering/README.md](features/write-time-reordering/README.md)** — 概述
- **[features/write-time-reordering/PHASE2_IMPLEMENTATION_SUMMARY.md](features/write-time-reordering/PHASE2_IMPLEMENTATION_SUMMARY.md)** — Phase 2总结
- **[features/write-time-reordering/WRITE_TIME_REORDERING_IMPLEMENTATION.md](features/write-time-reordering/WRITE_TIME_REORDERING_IMPLEMENTATION.md)** — 实现细节

### AutoSA 集成
- **[autosa/README.md](autosa/README.md)** — 概述
- **[autosa/REORGANIZATION_COMPLETION_REPORT.md](autosa/REORGANIZATION_COMPLETION_REPORT.md)** — HLS参考文件整理

---

## 🔍 参考资料

- **[reference/autosa/](reference/autosa/)** — AutoSA 分析和集成
- **[reference/allo/](reference/allo/)** — Allo HLS代码生成
- **[reference/testing/](reference/testing/)** — 测试参考

---

## 🐛 已知问题

- **[issues/README.md](issues/README.md)** — 问题跟踪和分析

---

## 📁 文档结构

```
docs/
├── README.md                                    # 本文件（文档导航）
├── ARCHITECTURE_OVERVIEW.md                     # 架构总览
├── CODE_STRUCTURE.md                            # 代码结构
│
├── guide/                                       # 开发指南
│   ├── BUILD_GUIDE.md                           # 构建指南
│   └── DEVELOPMENT_GUIDE.md                     # 开发指南
│
├── autosa/                                      # AutoSA集成
│   ├── README.md
│   └── REORGANIZATION_COMPLETION_REPORT.md
│
├── features/                                    # 核心特性
│   ├── polymer/                                 # Polymer优化
│   │   ├── README.md
│   │   ├── POLYMER_QUICK_START.md
│   │   └── POLYMER_INTEGRATION_COMPLETE.md
│   ├── spacetime/                               # Space-Time架构
│   │   ├── README.md
│   │   └── SPACETIME_IMPLEMENTATION_PLAN.md
│   └── write-time-reordering/                   # 写时重排序
│       ├── README.md
│       ├── PHASE2_IMPLEMENTATION_SUMMARY.md
│       └── WRITE_TIME_REORDERING_IMPLEMENTATION.md
│
├── status/                                      # 项目状态
│   ├── PROJECT_STATUS.md                        # 当前状态
│   └── ROADMAP.md                               # 路线图
│
├── reference/                                   # 参考资料
│   ├── autosa/                                  # AutoSA参考
│   │   ├── README.md
│   │   ├── AUTOSA_ANALYSIS.md
│   │   ├── AUTOSA_ARCHITECTURE.md
│   │   └── comparison_with_autosa.md
│   ├── allo/                                    # Allo参考
│   │   ├── README.md
│   │   └── ALLO_HLS_CODE_GENERATION_RULES.md
│   └── testing/                                 # 测试参考
│       ├── README.md
│       └── REFERENCE_SAMPLES.md
│
└── issues/                                      # 已知问题
    └── README.md
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

