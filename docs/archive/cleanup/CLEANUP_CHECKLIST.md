# 文档整理执行清单

> **创建时间**: 2026-01-06  
> **目的**: 具体的文档整理和删除任务

---

## 第一部分: test/ 目录清理

### 待删除的文件 (共 15 个)

这些文件都是工作过程中的临时总结，内容已被合并或过时:

```bash
# Pass 注册相关 (已完成)
rm test/PASS_REGISTRATION_FIX.md
rm test/PASS_REGISTRATION_SUCCESS.md
rm test/PASS_CHECK_SUMMARY.md
rm test/PASS_STATUS_SUMMARY.md

# 验证相关 (已完成)
rm test/DEBUG_FIX_SUMMARY.md
rm test/DEBUG_SUCCESS.md
rm test/VERIFICATION_RESULTS.md
rm test/VERIFICATION_COMPLETE.md
rm test/VERIFICATION_SUMMARY.md
rm test/FINAL_VERIFICATION_SUMMARY.md
rm test/PASS_VERIFICATION_SUCCESS.md

# 写时重排验证 (已完成)
rm test/WRITE_TIME_REORDERING_SUCCESS.md
rm test/MTTKRP_TEST_SUMMARY.md
rm test/MTTKRP_TEST_STATUS.md

# 手动验证 (已完成)
rm test/verify_reordering_manual.md
```

### 保留的核心文件

```
test/
├── CURRENT_STATUS.md          → 重命名为 TEST_STATUS.md (最新工作状态)
├── TEST_RESULTS.md            → 保留 (汇总测试结果)
├── REORDERING_TEST_RESULTS.md → 整合到 TEST_RESULTS.md (可删除)
├── test_results.log           → 保留 (原始日志)
└── [其他测试脚本和 MLIR 文件]
```

### 新建文件

**test/TESTING_GUIDE.md**:
- 如何运行各个测试脚本
- 测试覆盖范围说明
- 预期的通过条件

---

## 第二部分: docs/ 目录整理

### 待删除的文件 (共 4 个)

```bash
# 根目录的组织文件 (内容已合并到 README.md)
rm docs/DOCUMENTATION_REORGANIZATION_SUMMARY.md
rm docs/DOCUMENT_ORGANIZATION.md

# write-time-reordering 目录的工作清单
rm docs/write-time-reordering/DOCUMENTATION_CHECKLIST.md
rm docs/write-time-reordering/DOCUMENT_ORGANIZATION.md
```

### write-time-reordering/ 目录优化

**保留的文件**:
```
write-time-reordering/
├── README.md                                     (概览)
├── WRITE_TIME_REORDERING_ANALYSIS_REPORT.md   (详细分析)
├── WRITE_TIME_REORDERING_EXECUTIVE_SUMMARY.md (执行总结)
├── WRITE_TIME_REORDERING_QUICK_REFERENCE.md   (快速参考)
├── RANDOM_ACCESS_ANALYSIS.md                   (随机访问分析)
└── [其他有保留价值的文档]
```

**需要整合的文件**:
- `IMPLEMENTATION_IMPROVEMENTS.md` (改进方案)
- `PHASE2_IMPLEMENTATION_SUMMARY.md` (阶段 2 总结)
→ 合并为 `IMPLEMENTATION_ROADMAP.md`

### spacetime/ 目录优化

**当前状态**:
```
spacetime/
├── README.md
├── SPACETIME_IMPLEMENTATION_PLAN.md
├── SPACETIME_IMPLEMENTATION_ANALYSIS.md
├── SPACETIME_TEST_ANALYSIS.md
├── ST3_DETAILED_CODE_ANALYSIS.md
├── ST3_FUNCTION_DIFF_ANALYSIS.md
├── ST3_CODE_VERIFICATION.md
├── ST3_OPTIMIZATION_ANALYSIS.md
└── IMPLEMENTATION_ROADMAP.md
```

**优化方案**:
```
spacetime/
├── README.md                          (入口)
├── SPACETIME_IMPLEMENTATION_PLAN.md   (总体计划 - 保留)
├── implementations/
│   ├── ST3_REPORT.md                 (所有 ST3 分析合并)
│   │   ├── 详细代码分析
│   │   ├── 函数差异分析
│   │   ├── 代码验证结果
│   │   └── 优化方案
│   ├── ST0_DESIGN.md                 (为 ST0 预留)
│   ├── ST1_DESIGN.md                 (为 ST1 预留)
│   └── ...
└── IMPLEMENTATION_ROADMAP.md          (实现路线图)
```

### project/ 目录保持不变

```
project/
├── README.md
├── BUILD_STEPS.md
├── QUICK_REFERENCE.md
├── PROJECT_STATUS.md
├── ARCHITECTURE.md
├── DEVELOPMENT_GUIDE.md
└── IMPLEMENTATION_ROADMAP.md
```

### 根目录 docs/README.md 更新

删除对已删除文件的引用，更新导航。

---

## 第三部分: 新建文档

### 1. test/TESTING_GUIDE.md

内容框架:
```
# 测试指南

## 快速开始
- 如何编译
- 如何运行所有测试

## 测试组织
- test/matmul/ - MatMul kernel 测试
- test/mttkrp_*.mlir - MTTKRP 测试文件
- test/*.sh - 测试脚本

## 当前测试覆盖

| Kernel | Spacetime | 状态 | 脚本 |
|--------|-----------|------|------|
| MM | ST3 | ✅ 通过 11 个配置 | test_all_configs.sh |
| MTTKRP | - | ⚠️ 部分实现 | test_mttkrp_reordering.sh |
| ... | ... | ... | ... |

## 添加新测试
- 如何添加新的 kernel 测试
- 如何添加新的 spacetime 测试
```

### 2. docs/CODE_STRUCTURE.md

内容框架:
```
# 代码结构说明

## 核心模块

### 1. lib/Transforms/
- SystolicTransform.cpp: 主 Pass，执行 space-time 变换
- SystolicDataflowGeneration.cpp: 数据流 IR 生成
- SystolicDataflowToHLS.cpp: 降低到 HLS

### 2. lib/Analysis/
- SpaceTimeAnalysis.cpp: Space-time 分析
- SystolicConfig.h: 配置定义
- PolymerAnalysis.cpp: Polymer 集成

### 3. lib/Dialect/
- SystolicDataflow Dialect: 脉动阵列 IR

### 4. tools/systolic-translate/
- HLS C++ 代码生成

## 已知问题和 FIXME

### 硬编码位置 (需要参数化)

[表格列出具体的硬编码位置]

### 配置流 (需要改进)

[说明当前配置如何流动，以及问题所在]
```

### 3. docs/ARCHITECTURE_OVERVIEW.md

高级架构说明文档。

---

## 第四部分: 代码注释和标记

### 需要添加 FIXME 注释的位置

在以下文件中添加标记，说明哪些代码是 spacetime=3 特有的:

**lib/Transforms/SystolicTransform.cpp**:
```cpp
// FIXME: [spacetime=3 hardcoded] 空间循环选择硬编码为 [i,j]
// 需要参数化为支持其他 spacetime 配置
for (unsigned i = 0; i < 2; ++i) {
  spaceLoops.push_back(i);
}
```

**lib/Transforms/SystolicDataflowGeneration.cpp**:
```cpp
// FIXME: [spacetime=3 hardcoded] PE 阵列访问假设 [i][j]
// 需要根据实际 spacetime 参数调整
```

类似地为所有硬编码的地方添加标记。

---

## 第五部分: 执行顺序

### 立即执行 (今天)
1. 创建 `PROJECT_ORGANIZATION_AND_ANALYSIS.md` (主报告) ✓
2. 创建本文件 `CLEANUP_CHECKLIST.md` ✓

### 第一阶段 (明天)
1. 删除 test/ 目录的 15 个文件
2. 将 `test/CURRENT_STATUS.md` 重命名为 `TEST_STATUS.md`
3. 创建 `test/TESTING_GUIDE.md`
4. 更新 `test/TEST_RESULTS.md`

### 第二阶段 (后天)
1. 删除 docs/ 的 4 个文件
2. 重组 spacetime/ 目录结构
3. 创建 `docs/CODE_STRUCTURE.md`
4. 创建 `docs/ARCHITECTURE_OVERVIEW.md`

### 第三阶段 (本周)
1. 更新所有文档的导航链接
2. 在代码中添加 FIXME 标记
3. 创建 `docs/INDEX.md` 统一导航

---

## 第六部分: 验证清单

完成后需要验证:

- [ ] test/ 目录只保留必要的文件
- [ ] docs/ 目录结构清晰，无重复
- [ ] 所有过时文件都已删除或归档
- [ ] 新建文档完整可读
- [ ] 所有 markdown 链接有效
- [ ] 没有破损的交叉引用

---

## 注意事项

1. **备份**: 删除前建议 git commit 当前状态
2. **链接**: 更新所有指向被删除文件的链接
3. **历史**: 删除的文件通过 git history 仍可查看
4. **增量**: 建议分批执行，每个阶段后进行 git commit

