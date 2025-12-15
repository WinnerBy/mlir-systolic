# MLIR-Systolic 文档索引

> **最后更新**: 2025-12-10

本文档提供 MLIR-Systolic 项目所有文档的索引和导航。

---

## 📁 docs/ 目录文档

| 文档 | 说明 | 行数 | 推荐度 |
|------|------|------|--------|
| [COMPREHENSIVE_SUMMARY.md](./COMPREHENSIVE_SUMMARY.md) | **⭐ 推荐入口** - 项目综合总结（AutoSA分析、技术方案、实现进展） | ~700 | ⭐⭐⭐⭐⭐ |
| [AUTOSA_ARCHITECTURE.md](./AUTOSA_ARCHITECTURE.md) | **⭐ 重要** - AutoSA 两部分架构分析（Kernel + Host） | ~300 | ⭐⭐⭐⭐ |
| [POLYMER_INTEGRATION.md](./POLYMER_INTEGRATION.md) | **⭐ 核心** - Polymer 集成方案（为什么需要、如何集成） | ~300 | ⭐⭐⭐⭐⭐ |
| [POLYHEDRAL_STATUS.md](./POLYHEDRAL_STATUS.md) | **⭐ 重要** - 多面体能力使用状态（当前未使用） | ~200 | ⭐⭐⭐⭐ |
| [TECHNICAL_REDESIGN.md](./TECHNICAL_REDESIGN.md) | 技术方案重新设计（详细设计文档） | ~436 | ⭐⭐⭐⭐ |
| [QUICK_REFERENCE.md](./QUICK_REFERENCE.md) | 快速参考（关键改进点总结） | ~100 | ⭐⭐⭐ |
| [IMPLEMENTATION_PROGRESS.md](./IMPLEMENTATION_PROGRESS.md) | 实现进度（已完成工作和待办事项） | ~100 | ⭐⭐⭐ |
| [DOCUMENT_INDEX.md](./DOCUMENT_INDEX.md) | 本文档 - 文档索引和导航 | - | ⭐⭐⭐ |

---

## 📖 阅读建议

### 新手入门

1. **首先阅读** [README.md](../README.md) - 项目概述和快速开始
2. **然后查看** [COMPREHENSIVE_SUMMARY.md](./COMPREHENSIVE_SUMMARY.md) ⭐ **强烈推荐**
   - 包含完整的项目背景
   - AutoSA 架构深度分析
   - MLIR-Systolic 技术方案
   - 实现进展和下一步工作

### 了解技术方案

1. [COMPREHENSIVE_SUMMARY.md](./COMPREHENSIVE_SUMMARY.md) - 综合总结（第 4 节：技术方案）
2. **[POLYMER_INTEGRATION.md](./POLYMER_INTEGRATION.md)** ⭐ - **为什么需要 Polymer、如何集成**
3. [TECHNICAL_REDESIGN.md](./TECHNICAL_REDESIGN.md) - 详细技术设计
4. [QUICK_REFERENCE.md](./QUICK_REFERENCE.md) - 关键改进点快速参考

### 了解 AutoSA

1. [COMPREHENSIVE_SUMMARY.md](./COMPREHENSIVE_SUMMARY.md) - 第 2 节：AutoSA 架构深度分析
2. 参考项目：[AutoSA](https://github.com/UCLA-VAST/AutoSA) 源代码

### 参与开发

1. **[POLYMER_INTEGRATION.md](./POLYMER_INTEGRATION.md)** ⭐ - **最高优先级：Polymer 集成**
2. [IMPLEMENTATION_PROGRESS.md](./IMPLEMENTATION_PROGRESS.md) - 当前进度和待办事项
3. [TECHNICAL_REDESIGN.md](./TECHNICAL_REDESIGN.md) - 实现路线图
4. [README.md](../README.md) - 构建和使用说明

---

## 📊 文档关系图

```
MLIR-Systolic 文档体系
│
├── 📄 README.md (项目入口)
│
├── 📁 docs/ (核心文档)
│   ├── DOCUMENT_INDEX.md (本文档 - 索引)
│   ├── COMPREHENSIVE_SUMMARY.md (⭐ 推荐入口 - 综合总结)
│   ├── TECHNICAL_REDESIGN.md (技术方案详细设计)
│   ├── QUICK_REFERENCE.md (快速参考)
│   └── IMPLEMENTATION_PROGRESS.md (实现进度)
│
└── 📁 代码目录
    ├── include/systolic/Dialect/ (Dialect 定义)
    ├── lib/Transforms/ (Pass 实现)
    └── test/ (测试用例)
```

---

## 📝 文档内容概览

### COMPREHENSIVE_SUMMARY.md

**内容**:
- 项目背景与目标
- AutoSA 架构深度分析（关键数据结构、分层 IO、双缓冲、空间-时间映射等）
- ScaleHLS 尝试与问题分析
- MLIR-Systolic 技术方案（架构设计、Dialect 设计）
- 实现进展
- 与 AutoSA 的对应关系
- 技术优势与创新点
- 下一步工作

**适合**: 全面了解项目，包括背景、设计、实现和未来计划

### TECHNICAL_REDESIGN.md

**内容**:
- 问题分析
- 改进方案详细设计
- Dialect 定义示例
- 实现细节
- 实现路线图

**适合**: 深入了解技术方案和实现细节

### QUICK_REFERENCE.md

**内容**:
- 核心问题
- 关键改进
- 实现步骤
- 与 AutoSA 的对应

**适合**: 快速了解关键改进点

### IMPLEMENTATION_PROGRESS.md

**内容**:
- 已完成工作
- 文件结构
- 待实现功能
- 构建和测试说明

**适合**: 了解当前实现状态和下一步工作

---

## 🔗 外部参考

### 参考项目文档

以下文档记录了在 ScaleHLS 上的工作，为 MLIR-Systolic 提供了宝贵的经验：

- `scalehls/docs/MLIR_AutoSA_Implementation.md` - ScaleHLS 实现细节
- `scalehls/docs/AutoSA_Quick_Reference.md` - AutoSA 快速参考
- `scalehls/docs/HLS_Output_Comparison.md` - 输出对比分析

### 相关项目

- [AutoSA](https://github.com/UCLA-VAST/AutoSA) - 脉动阵列生成算法（参考实现）
- [ScaleHLS](https://github.com/UIUC-ChenLab/scalehls) - HLS Dialect 和代码生成（参考实现）
- [Polygeist/Polymer](https://github.com/llvm/Polygeist) - 多面体分析工具

---

## 📝 文档维护说明

### 文档位置原则

1. **docs/** - 核心文档、技术方案、总结文档
2. **README.md** - 项目入口，快速开始指南
3. **代码注释** - 实现细节和 API 文档

### 更新本索引

当添加新文档时，请更新本索引文件，保持文档体系的可发现性。

### 文档更新频率

- **COMPREHENSIVE_SUMMARY.md**: 重要里程碑时更新
- **IMPLEMENTATION_PROGRESS.md**: 每次完成重要功能时更新
- **TECHNICAL_REDESIGN.md**: 技术方案变更时更新

---

## 🎯 快速导航

### 我想了解...

- **项目整体情况** → [COMPREHENSIVE_SUMMARY.md](./COMPREHENSIVE_SUMMARY.md)
- **技术方案细节** → [TECHNICAL_REDESIGN.md](./TECHNICAL_REDESIGN.md)
- **快速上手** → [README.md](../README.md)
- **当前进度** → [IMPLEMENTATION_PROGRESS.md](./IMPLEMENTATION_PROGRESS.md)
- **关键改进点** → [QUICK_REFERENCE.md](./QUICK_REFERENCE.md)

---

**最后更新**: 2025-12-10

