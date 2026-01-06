# 根目录文档整理计划

**日期**: 2026-01-06  
**目标**: 整理根目录的 12 个 markdown 文件

---

## 当前根目录文档清单

| 文件名 | 大小 | 用途 | 处理方案 |
|--------|------|------|----------|
| README.md | 19K | 项目主说明 | ✅ **保留** - 主入口 |
| PROJECT_STRUCTURE.md | 6.2K | 项目结构说明 | ✅ **保留** - 核心文档 |
| 整理工作总结.md | 11K | 整理工作总结 | 📦 **移动** → docs/ |
| 整理工作文件索引.md | 9.6K | 文档索引 | 📦 **移动** → docs/ |
| 快速参考卡.md | 7.4K | 快速参考 | 📦 **移动** → docs/ |
| PROJECT_ORGANIZATION_AND_ANALYSIS.md | 16K | 项目组织分析 | 📦 **移动** → docs/ |
| CODE_ISSUES_DETAILED_ANALYSIS.md | 13K | 代码问题分析 | 📦 **移动** → docs/ |
| NEXT_STEPS_TECHNICAL_ROADMAP.md | 17K | 技术路线图 | 📦 **移动** → docs/ |
| CLEANUP_CHECKLIST.md | 7.0K | 清理检查清单 | 🗄️ **归档** → docs/archive/cleanup/ |
| CLEANUP_COMPLETE.md | 7.1K | 清理完成报告 | 🗄️ **归档** → docs/archive/cleanup/ |
| DOCS_REORGANIZATION_PLAN.md | 13K | docs 重组计划 | 🗄️ **归档** → docs/archive/cleanup/ |
| 一些零散的工作计划.md | 2.3K | 零散计划（原始需求） | 🗄️ **归档** → docs/archive/cleanup/ |

---

## 整理原则

### 保留在根目录
**标准**: 项目核心入口和基础文档
- ✅ README.md - 项目主入口
- ✅ PROJECT_STRUCTURE.md - 项目结构说明

### 移动到 docs/
**标准**: 正式的分析、总结、路线图文档
- 整理工作总结.md → docs/整理工作总结.md
- 整理工作文件索引.md → docs/整理工作文件索引.md
- 快速参考卡.md → docs/快速参考卡.md
- PROJECT_ORGANIZATION_AND_ANALYSIS.md → docs/PROJECT_ORGANIZATION_AND_ANALYSIS.md
- CODE_ISSUES_DETAILED_ANALYSIS.md → docs/CODE_ISSUES_DETAILED_ANALYSIS.md
- NEXT_STEPS_TECHNICAL_ROADMAP.md → docs/NEXT_STEPS_TECHNICAL_ROADMAP.md

### 归档到 docs/archive/cleanup/
**标准**: 临时性的清理/重组计划文档（已完成任务）
- CLEANUP_CHECKLIST.md
- CLEANUP_COMPLETE.md
- DOCS_REORGANIZATION_PLAN.md
- 一些零散的工作计划.md（原始输入）

---

## 执行步骤

### Step 1: 创建归档目录
```bash
mkdir -p docs/archive/cleanup
```

### Step 2: 移动正式文档到 docs/
```bash
mv 整理工作总结.md docs/
mv 整理工作文件索引.md docs/
mv 快速参考卡.md docs/
mv PROJECT_ORGANIZATION_AND_ANALYSIS.md docs/
mv CODE_ISSUES_DETAILED_ANALYSIS.md docs/
mv NEXT_STEPS_TECHNICAL_ROADMAP.md docs/
```

### Step 3: 归档清理文档到 docs/archive/cleanup/
```bash
mv CLEANUP_CHECKLIST.md docs/archive/cleanup/
mv CLEANUP_COMPLETE.md docs/archive/cleanup/
mv DOCS_REORGANIZATION_PLAN.md docs/archive/cleanup/
mv 一些零散的工作计划.md docs/archive/cleanup/
```

### Step 4: 更新 docs/README.md 引用
将正式文档的链接从 `../xxx.md` 更新为 `xxx.md`（因为它们现在在同一目录）

### Step 5: 更新根目录 README.md
添加指向 docs/ 的导航链接

---

## 预期结果

### 根目录（清爽）
```
mlir-systolic/
├── README.md                    # 项目主入口
├── PROJECT_STRUCTURE.md         # 项目结构
├── CMakeLists.txt
├── docs/                        # 所有文档在这里
├── lib/
├── include/
├── test/
└── ...
```

### docs/ 目录（完整）
```
docs/
├── README.md                                    # docs 总索引
├── 整理工作总结.md                              # 整理总结
├── 整理工作文件索引.md                          # 文档索引
├── 快速参考卡.md                                # 快速参考
├── PROJECT_ORGANIZATION_AND_ANALYSIS.md         # 项目组织分析
├── CODE_ISSUES_DETAILED_ANALYSIS.md             # 代码问题分析
├── NEXT_STEPS_TECHNICAL_ROADMAP.md              # 技术路线图
├── ARCHITECTURE_OVERVIEW.md
├── CODE_STRUCTURE.md
├── guide/
├── status/
├── features/
├── reference/
├── issues/
└── archive/
    ├── 2024-12/                                 # 历史实现文档
    └── cleanup/                                 # 清理过程文档
        ├── CLEANUP_CHECKLIST.md
        ├── CLEANUP_COMPLETE.md
        ├── DOCS_REORGANIZATION_PLAN.md
        └── 一些零散的工作计划.md
```

---

## 优点

1. **根目录更简洁**: 只保留 README 和 PROJECT_STRUCTURE
2. **文档集中管理**: 所有文档都在 docs/ 下
3. **清理过程可追溯**: 清理相关文档归档到 docs/archive/cleanup/
4. **便于维护**: 正式文档与临时文档分开

---

## 执行时间估计

- Step 1-3: 2 分钟（移动文件）
- Step 4: 5 分钟（更新引用）
- Step 5: 3 分钟（更新 README）
- **总计**: 约 10 分钟
