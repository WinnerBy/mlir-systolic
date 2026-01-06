# mlir-systolic 项目整理和分析报告

> **生成时间**: 2026-01-06  
> **报告范围**: 文档整理、项目完成情况、代码问题分析、下一步计划

---

## 一、文档和测试文件整理方案

### 1.1 当前问题分析

#### 文档混乱情况
- **test/ 目录**: 存在 20+ 个 .md 文件，大部分是工作过程中的临时总结
  - `DEBUG_FIX_SUMMARY.md`, `PASS_REGISTRATION_FIX.md`, `VERIFICATION_COMPLETE.md` 等为临时文件
  - `CURRENT_STATUS.md` 作为最新状态文档
  - 文件命名不规范，难以区分哪些是最终总结

- **docs/ 目录**: 虽已初步分类，但仍存在冗余
  - 多个 DOCUMENTATION_*.md 和 REORGANIZATION_*.md 文件
  - write-time-reordering 目录下有多个相似的分析文档
  - 过时的工作计划文档没有完全清理

- **代码生成的临时文件**: 
  - `autosa_hls_output/` 中的代码文件无法追溯参数
  - `autosa_reference_samples/` 缺少组织说明

### 1.2 建议的整理方案

#### 第一阶段：清理 test/ 目录

**要删除的文件** (已完成工作的临时记录):
```
test/DEBUG_FIX_SUMMARY.md
test/PASS_REGISTRATION_FIX.md
test/PASS_REGISTRATION_SUCCESS.md
test/PASS_CHECK_SUMMARY.md
test/PASS_STATUS_SUMMARY.md
test/DEBUG_SUCCESS.md
test/VERIFICATION_RESULTS.md
test/VERIFICATION_COMPLETE.md
test/VERIFICATION_SUMMARY.md
test/FINAL_VERIFICATION_SUMMARY.md
test/WRITE_TIME_REORDERING_SUCCESS.md
test/PASS_VERIFICATION_SUCCESS.md
test/verify_reordering_manual.md
test/MTTKRP_TEST_SUMMARY.md
test/MTTKRP_TEST_STATUS.md
```

**保留和整合的核心文件**:
- `test/CURRENT_STATUS.md` → 重命名为 `TEST_STATUS.md` (最新工作状态)
- `test/TEST_RESULTS.md` → 保留（汇总测试结果）
- `test/REORDERING_TEST_RESULTS.md` → 整合到 `TEST_RESULTS.md`

**新建文件**:
- `test/TESTING_GUIDE.md` - 如何运行测试的说明

#### 第二阶段：整合和优化 docs/ 目录

**要删除的文件**:
```
docs/DOCUMENTATION_REORGANIZATION_SUMMARY.md  (已过时)
docs/DOCUMENT_ORGANIZATION.md  (已合并到 README.md)
docs/DOCUMENTATION_REORGANIZATION_SUMMARY.md
```

**write-time-reordering/ 目录优化**:
- 保留: `README.md`, `WRITE_TIME_REORDERING_ANALYSIS_REPORT.md`, `WRITE_TIME_REORDERING_EXECUTIVE_SUMMARY.md`
- 删除: `DOCUMENTATION_CHECKLIST.md`, `DOCUMENT_ORGANIZATION.md` (临时工作清单)
- 整合: `IMPLEMENTATION_IMPROVEMENTS.md` 和 `PHASE2_IMPLEMENTATION_SUMMARY.md` 合并

**spacetime/ 目录优化**:
- 所有 ST3 分析文档合并为单个 `ST3_IMPLEMENTATION_REPORT.md`
- 为将来的 ST0-ST5 实现预留结构

#### 第三阶段：添加新的组织文件

**创建 docs/ 根目录文件**:
- `INDEX.md` - 快速导航和文档索引
- `ARCHITECTURE_OVERVIEW.md` - 整体架构说明

---

## 二、项目完成情况分析

### 2.1 已完成的工作

#### ✅ 核心 Pass 实现
- **SystolicTransform Pass**
  - 依赖分析 (使用 Polymer/ISL)
  - 空间循环选择
  - 循环置换和多级分块
  - 配置信息存储 (函数属性)
  - **状态**: 完整实现，仅限 spacetime=3 的 MM kernel

- **SystolicDataflowGeneration Pass**
  - 数组引用分组 (IO/PE/Drain)
  - 改进的 IO 层级分析
  - 配置信息读取
  - SystolicDataflow Dialect 操作创建
  - **状态**: 框架完成，功能部分完成

- **EmitHLSCpp Translation**
  - IO_L2_in 模块完整实现
  - C_drain 模块完整实现
  - PIPELINE pragma 插入 (24个，与AutoSA一致)
  - **状态**: 针对 spacetime=3 的完整实现

#### ✅ 测试和验证
- 11 个 AutoSA 参考配置通过 (spacetime=3, MM kernel)
- PIPELINE pragma 数量匹配 AutoSA
- 代码结构验证通过
- 写时重排功能初步实现

#### ✅ 文档和工具
- 构建系统重构 (自动化脚本)
- Polymer 集成完成
- 详细的 spacetime=3 分析文档

### 2.2 存在的主要问题

#### ⚠️ 问题 1: 仅适用于 Spacetime=3 的硬编码

**位置**: 多个 Pass 和代码生成器

**具体问题**:
1. **SystolicTransform.cpp**
   - 空间循环选择硬编码为 `[i, j]` (第0、1维)
   - 时间循环固定为 `[k]` (第2维)
   - 不支持其他 spacetime 配置 (ST0-ST5)

2. **SystolicDataflowGeneration.cpp**
   - PE 阵列访问模式假设为 `[i][j]`
   - 数据流方向硬编码:
     - A 水平传递 (假设 A[i,k] 只使用 i)
     - B 垂直传递 (假设 B[k,j] 只使用 j)
     - C 本地 (假设 C[i,j] 使用 i,j)

3. **SystolicDataflowToHLS.cpp**
   - PE 模块循环结构固定为 3 层 (对应 MM)
   - 不支持其他 kernel 的循环结构

4. **EmitHLSCpp translation**
   - IO_L2_in 模块固定为 3 维数组
   - serialize 循环边界依赖具体的 spacetime=3 配置
   - 不支持动态调整

**影响**:
- 无法支持其他 spacetime 配置
- 无法支持其他 kernel (MTTKRP, CNN, LU 等)

**解决方案**:
参见第三部分 "下一步工作" 的 "技术方案"

#### ⚠️ 问题 2: 配置信息流不清晰

**问题描述**:
- `SpaceTimeInfo` 中存储的信息不完整
- `SystolicConfig` 中的参数与实际代码生成参数不一致
- 配置如何从 Polymer 分析传递到 HLS 代码生成缺乏清晰的路径

**具体位置**:
- `lib/Analysis/SystolicConfig.h` - 配置数据结构定义不完整
- `lib/Transforms/SystolicTransform.cpp` - 配置信息存储方式临时
- `tools/systolic-translate/systolic-translate.cpp` - 配置读取方式不规范

**影响**:
- 难以支持多个配置同时运行
- 配置参数调整困难
- 代码生成结果不可预测

#### ⚠️ 问题 3: 写时重排分析局限

**问题描述**:
- 写时重排分析仅针对 3 维数组
- 分析结果未充分利用于代码生成
- 重排的有效性还未通过真实硬件验证

**具体位置**:
- `lib/Analysis/WriteTimeReorderingAnalysis.cpp` - 仅支持 `arrayDims.size() == 3`
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 重排信息存储但未使用
- 代码生成中没有生成实际的数组重排代码

#### ⚠️ 问题 4: 测试覆盖不足

**问题描述**:
- 仅测试了 MM kernel 的 spacetime=3
- MTTKRP 测试存在状态不明
- 没有对其他 spacetime 的测试
- 没有对其他 kernel 的测试

**现有测试状态**:
```
✅ MM kernel, spacetime=3  - 11 个配置，全部通过
⚠️ MTTKRP - 部分实现，测试状态不明
❌ ST0, ST1, ST2, ST4, ST5 - 未实现
❌ CNN, LU 等 kernel - 未实现
```

#### ⚠️ 问题 5: Dialect 实现不完整

**问题描述**:
- SystolicDataflow Dialect 操作定义完整，但语义不清晰
- 循环体迁移和双缓冲逻辑未完成
- 无法完整表示所有脉动阵列结构

**具体位置**:
- `lib/Dialect/SystolicDataflow/SystolicDataflow.cpp` - 框架完成，操作实现部分
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 循环体迁移代码缺失

### 2.3 代码质量评估

| 模块 | 完成度 | 质量评估 | 问题 |
|------|--------|---------|------|
| SystolicTransform | 80% | 接近生产级 | 硬编码为 ST3, MM |
| SystolicDataflow Dialect | 70% | 框架完成 | 语义不清晰 |
| SystolicDataflowGeneration | 60% | 框架完成 | 功能不完整 |
| EmitHLSCpp Translation | 85% | 质量好 | 仅支持 ST3 |
| Analysis Modules | 75% | 质量可 | 配置流不清晰 |

---

## 三、下一步工作规划

### 3.1 优先级排序

#### 优先级 1: 文档和代码整理 (1-2 天)
**目标**: 使项目结构清晰，便于后续开发

1. **清理 test/ 目录** (0.5 天)
   - 删除 20+ 个临时文件
   - 创建 `TEST_STATUS.md` 汇总当前测试状态

2. **整合 docs/ 文档** (0.5 天)
   - 删除过时的组织文件
   - 整合重复的分析文档
   - 创建统一的文档索引

3. **代码结构梳理** (1 天)
   - 识别 spacetime=3 相关的硬编码
   - 添加 FIXME 注释标记
   - 创建 `CODE_STRUCTURE.md` 说明

#### 优先级 2: 支持多种 Spacetime 配置 (5-7 天)
**目标**: 系统地支持 ST0-ST5 配置

**实现计划**:

**阶段 A: 配置系统重构** (2 天)
1. 扩展 `SpaceTimeInfo` 支持动态空间/时间循环选择
2. 改进 `SystolicConfig` 为参数化配置
3. 创建配置验证和转换框架
4. 添加 spacetime 参数自动推导

**阶段 B: ST0, ST1 实现** (2 天)
- ST0 ([i], 水平传递 B)
- ST1 ([j], 垂直传递 A)
- 关键改动:
  - 参数化空间循环选择
  - 参数化数据流方向
  - 参数化 PE 阵列维度

**阶段 C: ST2, ST4, ST5 实现** (3 天)
- ST2 ([k], 约化支持)
- ST4 ([i,k], 2D 约化)
- ST5 ([j,k], 2D 约化)
- 关键改动:
  - 约化树逻辑生成
  - 多维约化支持

#### 优先级 3: 支持其他 Kernel (7-10 天)
**目标**: 支持 MTTKRP, CNN, LU 等 kernel

**分析需求**:
1. **获取 AutoSA 代码** (与用户协作)
   - 学习 AutoSA 如何处理不同 kernel
   - 分析 kernel 参数提取逻辑
   - 参考多面体分析方法

2. **Kernel 检测和分类** (2 天)
   - 实现 kernel 类型自动识别
   - 支持可变循环层数 (3+)
   - 支持复杂访问模式

3. **通用空间循环选择** (2 天)
   - 扩展依赖距离分析
   - 实现通用空间循环选择算法
   - 支持多种空间循环组合

4. **通用代码生成** (3 天)
   - 动态 PE 模块结构生成
   - 动态 IO 层级生成
   - 动态约化逻辑生成

#### 优先级 4: 写时重排优化 (3-5 天)
**目标**: 验证和优化写时重排功能

1. **扩展支持多维数组** (1 天)
   - 支持 2D, 4D+ 数组
   - 通用重排模式检测

2. **代码生成** (1.5 天)
   - 生成实际的数组重排代码
   - 集成到 HLS 代码生成

3. **硬件验证** (2 天)
   - 在 U250 或其他 FPGA 上测试
   - 测量性能改进

#### 优先级 5: 数据复用和调优 (后续)
**目标**: 实现 AutoSA 的高级功能

1. L2 层数据复用 (host-serialize 优化)
2. 自动调优框架
3. 与 Allo 融合

### 3.2 技术方案概览

#### 方案 1: Spacetime 配置参数化

**关键设计**:
```cpp
// 当前硬编码
spaceLoops = {0, 1};  // [i, j]
timeLoops = {2};      // [k]

// 改进为配置驱动
SpaceTimeInfo {
  vector<int> spaceLoopIndices;  // 动态指定
  vector<int> timeLoopIndices;   // 动态指定
  std::string spacetimeType;     // "ST0", "ST1", ..., "ST5"
}
```

**关键改动**:
1. SystolicTransform 中的空间循环选择算法改为通用
2. 数据流方向根据 spacetime 类型自动推导
3. PE 模块维度根据 spaceLoops 大小确定

#### 方案 2: Kernel 参数通用提取

**核心概念**:
```
Input Kernel (Affine Loop Nest)
    ↓
Loop Analysis (Polymer)
    ↓
Kernel Type Detection (ML 或规则)
    ↓
Problem Size Inference
    ↓
Spacetime Selection (自动推荐)
```

**关键步骤**:
1. 从 Polymer 的 schedule 中推断循环结构
2. 根据访问模式识别 kernel 类型
3. 自动推荐合适的 spacetime 配置

#### 方案 3: 通用代码生成框架

**架构**:
```
SystolicConfig (参数化)
    ↓
SystolicDataflowGeneration (参数驱动)
    ↓
SystolicDataflow Dialect (中间表示)
    ↓
EmitHLSCpp (参数驱动的代码生成)
```

**模板化代码生成**:
- 使用 C++ 模板或代码生成库
- 根据 spacetime 参数生成不同的 PE 模块
- 根据 kernel 参数生成不同的 IO 模块

### 3.3 AutoSA 代码参考

**获取和分析重点**:
1. **AutoSA 主处理流程** (sa.cpp, main.cpp)
   - 如何遍历不同 kernel
   - 如何处理不同 spacetime
   - 参数推荐算法

2. **多面体分析部分**
   - 依赖距离计算方法
   - 空间循环选择算法
   - 多级分块策略

3. **代码生成部分**
   - IO 模块生成逻辑
   - PE 模块生成逻辑
   - HLS pragma 插入规则

4. **参数处理** (autosa_utils.cpp)
   - 如何从 kernel 推导 PE 数量
   - 如何选择分块大小
   - 如何计算延迟隐藏参数

---

## 四、建议的实现路线图

### 4.1 短期 (1-2 周)

**Week 1**:
- Day 1-2: 文档和代码清理
- Day 3-4: 获取 AutoSA 代码，进行深入分析
- Day 5: 规划 Spacetime 参数化重构

**Week 2**:
- Day 1-2: SpaceTimeInfo 和 SystolicConfig 重构
- Day 3-4: ST0, ST1 实现和测试
- Day 5: 代码审视和优化

### 4.2 中期 (2-4 周)

**Week 3-4**:
- 完成 ST2, ST4, ST5 实现
- 通用 spacetime 选择算法
- 扩展测试覆盖

**Week 5-6**:
- Kernel 类型识别系统
- 多 kernel 支持
- 性能优化

### 4.3 长期 (1-2 月)

- 写时重排硬件验证
- 数据复用优化
- 自动调优框架
- Allo 融合探索

---

## 五、关键建议

### 5.1 代码组织建议

1. **创建 Spacetime 配置文件**:
   ```
   include/systolic/Config/SpacetimeConfigs.h
   lib/Config/SpacetimeConfigs.cpp
   ```
   定义 ST0-ST5 的预设配置

2. **分离 Kernel 处理**:
   ```
   include/systolic/Kernels/KernelBase.h
   lib/Kernels/MMKernel.cpp
   lib/Kernels/MTTKRPKernel.cpp
   lib/Kernels/...
   ```

3. **创建代码生成模板**:
   ```
   lib/CodeGen/Templates/
   ├── pe_module_1d.cpp.template
   ├── pe_module_2d.cpp.template
   ├── io_module.cpp.template
   └── ...
   ```

### 5.2 测试策略

1. **单元测试**: 
   - 每个 spacetime 配置的基本测试
   - 每个 kernel 类型的基本测试

2. **集成测试**:
   - Spacetime + Kernel 的组合测试
   - 端到端流程验证

3. **参考对比**:
   - 与 AutoSA 输出对比
   - PIPELINE 数量、代码行数等指标

### 5.3 文档维护

1. **keep docs/ 简洁**:
   - 核心文档应该精而不是多
   - 过时文档及时归档或删除
   - 每个文档明确标记更新时间

2. **代码注释**:
   - 标记 spacetime 相关的硬编码
   - 为参数化的代码添加说明

3. **设计文档**:
   - 在代码重构前编写设计文档
   - 记录重要的设计决策

---

## 六、总结

### 6.1 当前状态

✅ **已完成**:
- Spacetime=3 MM kernel 的完整实现
- 核心 Pass 框架
- Polymer 集成

⚠️ **存在问题**:
- 硬编码限制在 spacetime=3 和 MM
- 配置系统不完善
- 文档需要整理

❌ **未完成**:
- 其他 spacetime 配置
- 其他 kernel 支持
- 写时重排硬件验证

### 6.2 后续行动

**立即行动** (本周):
1. 执行文档清理计划
2. 获取 AutoSA 代码
3. 详细分析 AutoSA 的实现

**关键里程碑**:
1. 完成 Spacetime 参数化重构
2. 实现 ST0-ST5 所有配置
3. 支持多个 kernel
4. 硬件验证

### 6.3 成功指标

- ✅ 支持 6 种 spacetime 配置 (ST0-ST5)
- ✅ 支持至少 3 种 kernel (MM, MTTKRP, CNN)
- ✅ 代码与 AutoSA 输出可比性 ≥ 95%
- ✅ 硬件测试性能收益 ≥ 10% (对于写时重排)

---

## 附录: 详细的代码问题位置

### A.1 Spacetime=3 硬编码位置

| 文件 | 行号 | 问题 | 优先级 |
|------|------|------|--------|
| SystolicTransform.cpp | ~120-200 | 空间循环硬编码为 [i,j] | 高 |
| SystolicTransform.cpp | ~300-400 | 时间循环硬编码为 [k] | 高 |
| SystolicDataflowGeneration.cpp | ~200-250 | 数据流硬编码 | 高 |
| SystolicDataflowGeneration.cpp | ~350-400 | PE 阵列维度假设 | 高 |
| EmitHLSCpp translation | ~100-200 | 3 维数组假设 | 中 |
| SystolicDataflowToHLS.cpp | ~50-100 | 循环结构固定 | 中 |

### A.2 配置流问题

| 组件 | 问题 | 影响 |
|------|------|------|
| SpaceTimeInfo | 信息不完整 | 无法推导所有参数 |
| SystolicConfig | 参数与代码生成不一致 | 配置流断裂 |
| 属性存储 | 函数属性方式不规范 | 难以维护 |

