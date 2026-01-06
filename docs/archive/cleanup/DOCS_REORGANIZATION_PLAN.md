# docs/ 目录重组计划

**创建日期**: 2026-01-06  
**状态**: 📋 规划中

---

## 目录

1. [当前问题](#当前问题)
2. [重组目标](#重组目标)
3. [新目录结构](#新目录结构)
4. [文档合并方案](#文档合并方案)
5. [删除清单](#删除清单)
6. [执行步骤](#执行步骤)

---

## 当前问题

### 目录结构问题
- **project/** (16个文档) - 内容过于杂乱，混合了架构、构建、状态、总结等多种类型
- **archive/** (23个文档) - 与主线文档并列，容易混淆新旧内容
- **write-time-reordering/** (11个文档) - 重复的摘要、分析、实施文档过多
- **spacetime/** (8个文档) - ST3 相关文档拆分过细

### 文档重复问题
| 目录 | 重复文档 | 数量 |
|-----|---------|------|
| project/ | PROJECT_STATUS, IMPLEMENTATION_ROADMAP, NEXT_PHASE_WORK_PLAN | 3份状态/计划 |
| project/ | ANALYSIS_SUMMARY, FINAL_SUMMARY, README_ANALYSIS, ANALYSIS_DOCUMENTATION_GUIDE, QUICK_REFERENCE | 5份总结/索引 |
| write-time-reordering/ | QUICK_REFERENCE, EXECUTIVE_SUMMARY | 2份摘要 |
| write-time-reordering/ | RANDOM_ACCESS_ANALYSIS, ANALYSIS_REPORT | 2份分析 |
| write-time-reordering/ | IMPLEMENTATION, IMPROVEMENT_PLAN, USAGE | 3份实施文档 |
| spacetime/ | ST3_DETAILED_CODE_ANALYSIS, ST3_FUNCTION_DIFF_ANALYSIS, ST3_CODE_VERIFICATION, ST3_OPTIMIZATION_ANALYSIS | 4份 ST3 文档 |

### 统计数据
- **总文档数**: 73个 markdown 文件
- **archive/ 文档**: 23个（大部分已过时）
- **重复/可合并文档**: 约 20-25个

---

## 重组目标

### 清晰性
- ✅ 按**功能主题**分类，而非混杂类型
- ✅ 每个目录有明确的用途定义
- ✅ 避免状态/总结/分析文档散落各处

### 简洁性
- ✅ 删除过时的临时文档
- ✅ 合并重复内容
- ✅ 减少文档总数至 **40-50 个**

### 可导航性
- ✅ 顶层统一索引
- ✅ 每个子目录有清晰的 README
- ✅ 文档之间有明确的阅读路径

---

## 新目录结构

```
docs/
├── README.md                        # 📚 总索引（重写）
├── ARCHITECTURE_OVERVIEW.md         # ⭐ 架构概述（已存在）
├── CODE_STRUCTURE.md                # ⭐ 代码结构（已存在）
│
├── guide/                           # 📖 指南类
│   ├── README.md
│   ├── BUILD_GUIDE.md               # 构建指南（合并）
│   ├── DEVELOPMENT_GUIDE.md         # 开发指南（来自 project/）
│   └── TESTING_GUIDE.md             # 测试指南（来自 ../test/）
│
├── status/                          # 📊 状态和路线图
│   ├── README.md
│   ├── PROJECT_STATUS.md            # 项目状态（合并）
│   └── ROADMAP.md                   # 路线图（合并）
│
├── features/                        # 🔧 功能实现详解
│   ├── README.md
│   │
│   ├── spacetime/                   # Space-time 实现
│   │   ├── README.md
│   │   ├── IMPLEMENTATION_PLAN.md
│   │   ├── IMPLEMENTATION_ANALYSIS.md
│   │   ├── TEST_ANALYSIS.md
│   │   └── ST3_REPORT.md            # ST3 差异与优化（合并4份）
│   │
│   └── write-time-reordering/       # 写时重排
│       ├── README.md
│       ├── OVERVIEW.md              # 概述与快速参考（合并2份）
│       ├── ANALYSIS.md              # 问题分析（合并2份）
│       ├── IMPLEMENTATION.md        # 实现指南（合并3份）
│       ├── POLYHEDRAL.md            # 多面体理论
│       └── code/                    # 代码参考
│           ├── CODEGEN_INTEGRATION.cpp
│           └── ISL_IMPLEMENTATION.cpp
│
├── reference/                       # 📚 参考资料
│   ├── README.md
│   │
│   ├── autosa/                      # AutoSA 参考
│   │   ├── README.md
│   │   ├── ARCHITECTURE.md
│   │   ├── SPACETIME_ANALYSIS.md
│   │   ├── TEST_GENERATION.md
│   │   └── COMPARISON.md            # 与 AutoSA 对比
│   │
│   ├── allo/                        # Allo 集成
│   │   ├── README.md
│   │   ├── INTEGRATION_ANALYSIS.md
│   │   └── HLS_CODE_GENERATION_RULES.md
│   │
│   └── testing/                     # 测试参考
│       ├── README.md
│       ├── TEST_RESULTS.md
│       └── REFERENCE_SAMPLES.md
│
├── issues/                          # 🐛 问题分析
│   ├── README.md
│   ├── SIMD2_ISSUE.md
│   └── DATA_REUSE_STRATEGY.md
│
└── archive/                         # 📦 历史归档（隐藏）
    └── 2024-12/
        └── (旧状态文档)
```

---

## 文档合并方案

### 1. project/ 目录合并

#### 状态和路线图 → status/
```
合并：
- project/PROJECT_STATUS.md
- project/IMPLEMENTATION_ROADMAP.md
- project/NEXT_PHASE_WORK_PLAN.md

合并为：
→ status/PROJECT_STATUS.md (当前状态部分)
→ status/ROADMAP.md (计划和路线图部分)
```

#### 构建指南 → guide/
```
合并：
- project/BUILD_STEPS.md
- project/POLYMER_LIBRARIES_REQUIRED.md
- project/SCRIPTS.md

合并为：
→ guide/BUILD_GUIDE.md
  章节：
  - 依赖安装
  - 构建步骤
  - 脚本说明
  - 常见问题
```

#### 保留并移动
```
- project/DEVELOPMENT_GUIDE.md → guide/DEVELOPMENT_GUIDE.md
- project/ARCHITECTURE.md → 删除（已有 ARCHITECTURE_OVERVIEW.md）
- project/ALLO_INTEGRATION_ANALYSIS.md → reference/allo/INTEGRATION_ANALYSIS.md
- project/ALLO_HLS_CODE_GENERATION_RULES.md → reference/allo/HLS_CODE_GENERATION_RULES.md
```

#### 删除（冗余/过时）
```
- project/ANALYSIS_SUMMARY.md (内容已整合到根目录文档)
- project/FINAL_SUMMARY.md (内容已整合到根目录文档)
- project/README_ANALYSIS.md (用新的 README 替代)
- project/ANALYSIS_DOCUMENTATION_GUIDE.md (用新的 README 替代)
- project/QUICK_REFERENCE.md (信息已在其他文档中)
```

### 2. write-time-reordering/ 合并

```
合并：
- QUICK_REFERENCE.md + EXECUTIVE_SUMMARY.md
  → features/write-time-reordering/OVERVIEW.md

- RANDOM_ACCESS_ANALYSIS.md + WRITE_TIME_REORDERING_ANALYSIS_REPORT.md
  → features/write-time-reordering/ANALYSIS.md

- IMPLEMENTATION.md + IMPROVEMENT_PLAN.md + USAGE.md
  → features/write-time-reordering/IMPLEMENTATION.md

保留：
- POLYHEDRAL.md → features/write-time-reordering/POLYHEDRAL.md
- CODEGEN_INTEGRATION.cpp → features/write-time-reordering/code/
- ISL_IMPLEMENTATION.cpp → features/write-time-reordering/code/

删除：
- PHASE2_IMPLEMENTATION_SUMMARY.md (内容已过时或整合)
```

### 3. spacetime/ 合并

```
合并 ST3 系列：
- ST3_DETAILED_CODE_ANALYSIS.md
- ST3_FUNCTION_DIFF_ANALYSIS.md
- ST3_CODE_VERIFICATION.md
- ST3_OPTIMIZATION_ANALYSIS.md
  → features/spacetime/ST3_REPORT.md
    章节：
    - ST3 代码差异分析
    - 函数级差异
    - 代码验证结果
    - 优化分析

保留：
- IMPLEMENTATION_PLAN.md
- IMPLEMENTATION_ANALYSIS.md
- TEST_ANALYSIS.md
```

### 4. autosa/ 重组

```
重命名和移动：
- AUTOSA_ARCHITECTURE.md → reference/autosa/ARCHITECTURE.md
- AUTOSA_SPACETIME_ANALYSIS.md → reference/autosa/SPACETIME_ANALYSIS.md
- AUTOSA_TEST_GENERATION_GUIDE.md → reference/autosa/TEST_GENERATION.md
- comparison_with_autosa.md → reference/autosa/COMPARISON.md
- AUTOSA_ANALYSIS.md → reference/autosa/README.md (作为索引)
```

### 5. testing/ 移动

```
移动到 reference/testing/:
- TEST_RESULTS.md
- REFERENCE_SAMPLES.md
- README.md
```

### 6. other-issues/ 移动

```
移动到 issues/:
- SIMD2_ISSUE_ANALYSIS.md → SIMD2_ISSUE.md
- DATA_REUSE_STRATEGY.md
```

---

## 删除清单

### archive/ 目录（22个文件）
```bash
# 除了 README.md，其余全部删除或移到 archive/2024-12/
docs/archive/AUTOSA_OPTIMIZATION_SUMMARY.md
docs/archive/AUTOSA_OUTPUT_ORGANIZATION_PLAN.md
docs/archive/BUILD_STEPS_OPTIMIZED.md
docs/archive/BUILD_SUMMARY.md
docs/archive/BUILD_UNIFIED.md
docs/archive/CODE_DIFF_ANALYSIS.md
docs/archive/DEBUG_STATUS.md
docs/archive/DEVELOPMENT_BUILD.md
docs/archive/DRAIN_MODULE_IMPLEMENTATION.md
docs/archive/IMPLEMENTATION_PROGRESS.md
docs/archive/OPTIMIZATION_SUMMARY.md
docs/archive/POLYHEDRAL_STATUS.md
docs/archive/POLYMER_BUILD_PLAN.md
docs/archive/POLYMER_ENABLE_PROGRESS.md
docs/archive/POLYMER_ENABLE_STATUS.md
docs/archive/POLYMER_IMPLEMENTATION_STATUS.md
docs/archive/POLYMER_IMPLEMENTATION_SUMMARY.md
docs/archive/POLYMER_INTEGRATION_PROGRESS.md
docs/archive/SYSTOLIC_DATAFLOW_GENERATION_COMPLETE.md
docs/archive/TEST_RESULTS.md
docs/archive/TEST_SUMMARY.md
docs/archive/WORK_PROGRESS_AND_PLAN.md
```

### project/ 目录（5个文件）
```bash
docs/project/ANALYSIS_SUMMARY.md
docs/project/FINAL_SUMMARY.md
docs/project/README_ANALYSIS.md
docs/project/ANALYSIS_DOCUMENTATION_GUIDE.md
docs/project/QUICK_REFERENCE.md
```

### write-time-reordering/ 目录（1个文件）
```bash
docs/write-time-reordering/PHASE2_IMPLEMENTATION_SUMMARY.md
```

**删除总计**: 约 28 个文件

---

## 执行步骤

### Phase 1: 准备和备份 ✅
```bash
# 1. Git 提交当前状态
git add -A
git commit -m "docs: 重组前备份"

# 2. 创建新目录结构
mkdir -p docs/{guide,status,features/{spacetime,write-time-reordering/code},reference/{autosa,allo,testing},issues}
```

### Phase 2: 删除 archive/ 临时文件
```bash
# 移动 archive 内容到深层目录
mkdir -p docs/archive/2024-12
mv docs/archive/*.md docs/archive/2024-12/ 2>/dev/null || true
# 保留 README
mv docs/archive/2024-12/README.md docs/archive/
```

### Phase 3: 合并和创建新文档
按以下顺序创建合并后的文档：
1. ✅ guide/BUILD_GUIDE.md
2. ✅ status/PROJECT_STATUS.md
3. ✅ status/ROADMAP.md
4. ✅ features/spacetime/ST3_REPORT.md
5. ✅ features/write-time-reordering/OVERVIEW.md
6. ✅ features/write-time-reordering/ANALYSIS.md
7. ✅ features/write-time-reordering/IMPLEMENTATION.md

### Phase 4: 移动和重命名文件
```bash
# autosa
mv docs/autosa/*.md docs/reference/autosa/

# allo
mv docs/project/ALLO_*.md docs/reference/allo/

# testing
mv docs/testing/*.md docs/reference/testing/

# issues
mv docs/other-issues/*.md docs/issues/

# guide
mv docs/project/DEVELOPMENT_GUIDE.md docs/guide/
```

### Phase 5: 删除冗余文件
```bash
# project/
rm docs/project/ANALYSIS_SUMMARY.md
rm docs/project/FINAL_SUMMARY.md
rm docs/project/README_ANALYSIS.md
rm docs/project/ANALYSIS_DOCUMENTATION_GUIDE.md
rm docs/project/QUICK_REFERENCE.md

# 删除空目录
rmdir docs/project docs/other-issues docs/testing
```

### Phase 6: 更新所有 README
1. 更新 docs/README.md（总索引）
2. 创建各子目录的 README.md
3. 更新根目录的相关文档引用

### Phase 7: Git 提交
```bash
git add -A
git status
git commit -m "docs: 完成目录重组

- 重组为 guide/, status/, features/, reference/, issues/ 结构
- 删除 archive/ 中 22 个过时文档
- 删除 project/ 中 5 个冗余文档
- 合并重复的状态、分析、实施文档
- 更新所有索引和交叉引用"
```

---

## 预期结果

### 文档数量变化
| 分类 | 重组前 | 重组后 | 变化 |
|-----|--------|--------|------|
| archive/ | 23 | 1 | -22 |
| project/ | 16 | 0 | -16 (移动/合并) |
| write-time-reordering/ | 11 | 5 | -6 (合并) |
| spacetime/ | 8 | 4 | -4 (合并) |
| autosa/ | 6 | 5 | -1 (重组) |
| testing/ | 3 | 3 | 0 (移动) |
| other-issues/ | 3 | 2 | -1 |
| **新目录** | 0 | 20+ | +20+ |
| **总计** | 73 | ~45 | **-28** |

### 目录层次
- **重组前**: 7个一级目录，结构扁平
- **重组后**: 6个功能性一级目录，层次清晰

### 可维护性提升
- ✅ 每个目录有明确的主题定义
- ✅ 重复文档减少 40%
- ✅ 新人阅读路径清晰
- ✅ 文档更新不会产生冗余

---

## 风险和注意事项

### 风险
1. **外部链接失效**: 如果有外部文档引用了旧路径
2. **脚本依赖**: 某些脚本可能硬编码了文档路径

### 缓解措施
1. ✅ Git 保留完整历史，可随时恢复
2. ✅ 逐步执行，每个 phase 独立提交
3. ✅ 更新根目录的索引文档引用
4. ⚠️ 搜索代码库中的文档路径引用

### 验证清单
- [ ] 所有新文档创建完成
- [ ] 所有移动操作完成
- [ ] 所有删除操作完成
- [ ] README 索引更新
- [ ] 交叉引用检查
- [ ] Git 提交历史清晰

---

## 相关文档

- [CLEANUP_CHECKLIST.md](CLEANUP_CHECKLIST.md) - 根目录文档清理
- [CLEANUP_COMPLETE.md](CLEANUP_COMPLETE.md) - 第一轮清理完成报告
- [PROJECT_ORGANIZATION_AND_ANALYSIS.md](PROJECT_ORGANIZATION_AND_ANALYSIS.md) - 项目组织分析

---

**下一步**: 执行 Phase 1-2，删除 archive/ 临时文档
