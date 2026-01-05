# 文档整理说明

> **创建时间**: 2026-01-05  
> **目的**: 说明文档分类和组织方式

---

## 文档分类

### 1. 核心文档（长期维护）

#### 快速开始
- `QUICK_REFERENCE.md` - 常用命令和参数配置快速查找
- `BUILD_STEPS.md` - 详细的构建步骤和故障排除
- `SCRIPTS.md` - 各个脚本的用途和使用方法

#### 项目状态和架构
- `PROJECT_STATUS.md` - 项目当前状态和最新更新
- `ARCHITECTURE.md` - 项目架构和设计理念
- `DEVELOPMENT_GUIDE.md` - 开发流程和代码结构

#### AutoSA 参考
- `AUTOSA_ANALYSIS.md` - AutoSA 架构、算法、参数影响分析
- `AUTOSA_SPACETIME_ANALYSIS.md` - Spacetime 参数详细分析（0-5）
- `AUTOSA_ARCHITECTURE.md` - AutoSA 架构详细分析
- `AUTOSA_TEST_GENERATION_GUIDE.md` - 参数配置参考

#### 测试和验证
- `TEST_RESULTS.md` - 测试结果、Kernel 计算公式、下一步工作计划
- `REFERENCE_SAMPLES.md` - Reference samples 概述

---

### 2. 问题分析和解决方案（已完成或进行中）

#### Space-time=3 相关分析
- `ST3_DETAILED_CODE_ANALYSIS.md` - 逐行代码差异分析及HLS行为影响评估
- `ST3_FUNCTION_DIFF_ANALYSIS.md` - 函数数量差异详细分析
- `ST3_CODE_VERIFICATION.md` - 有效代码一致性验证报告
- `ST3_OPTIMIZATION_ANALYSIS.md` - Space-time=3 代码生成优化分析

#### 其他问题分析
- `RANDOM_ACCESS_ANALYSIS.md` - 随机读取问题分析和修复方法
- `SIMD2_ISSUE_ANALYSIS.md` - SIMD=2 代码生成与 AutoSA 参考代码的差异分析
- `DATA_REUSE_STRATEGY.md` - 数据重用策略分析和实现方案

#### Space-time 实现相关
- `SPACETIME_TEST_ANALYSIS.md` - 不同 Space-time 配置的测试结果和优化方向
- `SPACETIME_IMPLEMENTATION_ANALYSIS.md` - 基于文档和参考代码的实现方案
- `SPACETIME_IMPLEMENTATION_PLAN.md` - 基于 AutoSA 官方文档的详细实现计划
- `IMPLEMENTATION_ROADMAP.md` - 详细的实施计划和进度跟踪

---

### 3. 集成和分析（参考文档）

- `ALLO_INTEGRATION_ANALYSIS.md` - Allo 项目集成方案分析
- `ALLO_HLS_CODE_GENERATION_RULES.md` - Allo HLS 代码生成规则
- `comparison_with_autosa.md` - mlir-systolic 与 AutoSA 的功能对比
- `POLYMER_LIBRARIES_REQUIRED.md` - Polymer 库依赖说明

---

### 4. 归档文档（历史记录）

已移动到 `archive/` 目录的文档：
- 旧的构建指南（已合并到 BUILD_STEPS.md）
- 旧的实现总结（已过时）
- 旧的测试结果（已过时）
- 已完成的工作计划（如 AUTOSA_OUTPUT_ORGANIZATION_PLAN.md）

---

## 文档维护原则

1. **核心文档**：保持更新，反映最新状态
2. **问题分析文档**：记录问题和解决方案，作为参考
3. **临时工作计划**：完成后移动到 archive
4. **过时文档**：移动到 archive，保留历史记录

---

## 最近更新

- **2026-01-05**: 
  - 修正 `ST3_DETAILED_CODE_ANALYSIS.md` 中关于 serialize 循环边界的错误分析
  - 将 `AUTOSA_OUTPUT_ORGANIZATION_PLAN.md` 移动到 archive（工作已完成）
  - 创建文档整理说明

