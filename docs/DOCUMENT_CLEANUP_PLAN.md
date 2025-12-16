# 文档整理计划

> **创建日期**: 2025-01-XX  
> **目的**: 整理 docs 文件夹，移除冗余和过时文档

---

## 📊 当前文档分析

### 核心文档（保留）

| 文档 | 状态 | 说明 |
|------|------|------|
| `README.md` | ✅ 保留 | 项目根目录，项目入口 |
| `CURRENT_STATUS.md` | ✅ **新增** | 整合所有当前状态信息 |
| `DOCUMENT_INDEX.md` | ✅ 更新 | 文档索引和导航 |
| `AUTOSA_ANALYSIS.md` | ✅ 保留 | AutoSA 详细分析（核心参考） |
| `AUTOSA_ARCHITECTURE.md` | ✅ 保留 | AutoSA 架构说明 |
| `ARCHITECTURE.md` | ✅ 保留 | mlir-systolic 架构设计 |
| `POLYMER_INTEGRATION.md` | ✅ 保留 | Polymer 集成方案 |
| `BUILD_POLYGEIST.md` | ✅ 保留 | Polygeist 构建指南 |

### 可归档的文档（移到 archive/）

| 文档 | 原因 | 替代文档 |
|------|------|----------|
| `POLYMER_IMPLEMENTATION_STATUS.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `POLYMER_IMPLEMENTATION_SUMMARY.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `POLYMER_ENABLE_STATUS.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `POLYMER_ENABLE_PROGRESS.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `POLYMER_BUILD_PLAN.md` | 过时，信息已整合 | `BUILD_POLYGEIST.md` |
| `BUILD_SUMMARY.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `TEST_RESULTS.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `DEBUG_STATUS.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `POLYMER_INTEGRATION_PROGRESS.md` | 过时，信息已整合 | `CURRENT_STATUS.md` |
| `IMPLEMENTATION_PROGRESS.md` | 可合并 | `CURRENT_STATUS.md` |
| `WORK_PROGRESS_AND_PLAN.md` | 可合并 | `CURRENT_STATUS.md` |
| `POLYHEDRAL_STATUS.md` | 可合并 | `CURRENT_STATUS.md` |

### 可选保留的文档

| 文档 | 建议 | 说明 |
|------|------|------|
| `COMPREHENSIVE_SUMMARY.md` | ⚠️ 评估 | 如果内容仍有价值可保留，否则归档 |
| `TECHNICAL_REDESIGN.md` | ⚠️ 评估 | 如果设计已完成可归档，否则保留 |
| `comparison_with_autosa.md` | ✅ 保留 | 对比分析仍有参考价值 |
| `DEVELOPMENT_GUIDE.md` | ⚠️ 评估 | 如果内容仍有用可保留 |
| `QUICK_REFERENCE.md` | ⚠️ 评估 | 如果内容仍有用可保留 |
| `SCOP_REGION.md` | ⚠️ 评估 | 如果内容仍有用可保留 |

---

## 🗂️ 建议的文档结构

### 保留在 docs/ 的文档

```
docs/
├── CURRENT_STATUS.md          ⭐ 新增 - 当前状态总结
├── DOCUMENT_INDEX.md          📖 文档索引
├── AUTOSA_ANALYSIS.md         📚 AutoSA 分析
├── AUTOSA_ARCHITECTURE.md     📚 AutoSA 架构
├── ARCHITECTURE.md            📚 项目架构
├── POLYMER_INTEGRATION.md     🔧 Polymer 集成
├── BUILD_POLYGEIST.md         🔧 构建指南
├── comparison_with_autosa.md  📊 对比分析（可选）
└── archive/                   📦 归档目录
    ├── POLYMER_IMPLEMENTATION_STATUS.md
    ├── POLYMER_IMPLEMENTATION_SUMMARY.md
    ├── POLYMER_ENABLE_STATUS.md
    ├── POLYMER_ENABLE_PROGRESS.md
    ├── POLYMER_BUILD_PLAN.md
    ├── BUILD_SUMMARY.md
    ├── TEST_RESULTS.md
    ├── DEBUG_STATUS.md
    ├── POLYMER_INTEGRATION_PROGRESS.md
    ├── IMPLEMENTATION_PROGRESS.md
    ├── WORK_PROGRESS_AND_PLAN.md
    └── POLYHEDRAL_STATUS.md
```

---

## 📋 执行步骤

### 步骤 1: 创建归档目录
```bash
cd docs
mkdir -p archive
```

### 步骤 2: 移动过时文档到 archive/
```bash
# 移动所有过时的状态文档
mv POLYMER_IMPLEMENTATION_STATUS.md archive/
mv POLYMER_IMPLEMENTATION_SUMMARY.md archive/
mv POLYMER_ENABLE_STATUS.md archive/
mv POLYMER_ENABLE_PROGRESS.md archive/
mv POLYMER_BUILD_PLAN.md archive/
mv BUILD_SUMMARY.md archive/
mv TEST_RESULTS.md archive/
mv DEBUG_STATUS.md archive/
mv POLYMER_INTEGRATION_PROGRESS.md archive/
mv IMPLEMENTATION_PROGRESS.md archive/
mv WORK_PROGRESS_AND_PLAN.md archive/
mv POLYHEDRAL_STATUS.md archive/
```

### 步骤 3: 评估可选文档
- 检查 `COMPREHENSIVE_SUMMARY.md` 是否仍有价值
- 检查 `TECHNICAL_REDESIGN.md` 是否已完成
- 检查 `DEVELOPMENT_GUIDE.md`、`QUICK_REFERENCE.md`、`SCOP_REGION.md` 是否仍有用

### 步骤 4: 更新 DOCUMENT_INDEX.md
- 更新文档索引，移除已归档的文档
- 添加 archive 目录说明

---

## ✅ 整理后的文档结构

### 核心文档（7-10 个）
1. `CURRENT_STATUS.md` - 当前状态（整合所有状态信息）
2. `DOCUMENT_INDEX.md` - 文档索引
3. `AUTOSA_ANALYSIS.md` - AutoSA 分析
4. `AUTOSA_ARCHITECTURE.md` - AutoSA 架构
5. `ARCHITECTURE.md` - 项目架构
6. `POLYMER_INTEGRATION.md` - Polymer 集成
7. `BUILD_POLYGEIST.md` - 构建指南
8. `comparison_with_autosa.md` - 对比分析（可选）
9. `DEVELOPMENT_GUIDE.md` - 开发指南（如果仍有用）
10. `QUICK_REFERENCE.md` - 快速参考（如果仍有用）

### 归档文档（12+ 个）
- 所有过时的状态文档
- 所有过时的进度文档
- 已完成的设计文档（如果不再需要）

---

## 🎯 整理目标

- **减少文档数量**: 从 25 个减少到 7-10 个核心文档
- **提高可发现性**: 通过 `DOCUMENT_INDEX.md` 和 `CURRENT_STATUS.md` 快速找到信息
- **保持历史记录**: 过时文档移到 archive/ 而不是删除
- **单一真实来源**: 每个主题只保留一个主要文档

---

## 📝 注意事项

1. **不要删除文档**: 只移动到 archive/ 目录
2. **保留 git 历史**: 移动文件时使用 `git mv` 保留历史
3. **更新引用**: 检查是否有其他文档引用被移动的文档
4. **定期清理**: 建议每季度评估一次文档结构

---

**执行建议**: 可以先移动明显过时的文档，然后逐步评估其他文档。

