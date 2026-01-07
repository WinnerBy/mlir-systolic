# Phase 2 进展报告

## ✅ 已完成任务

### Step 1: 参数化循环选择 (100%)
**文件**: `lib/Transforms/SystolicTransform.cpp`

**新增功能**:
1. `selectSpaceLoopsParametric()` - 使用 ParametricSpaceTime 提取循环索引
2. `ParametricSpaceTime::createFromMode()` - 工厂函数（mode 0-5 → ST0-ST5）
3. Transform Pass 集成 - 双路径策略（参数化 + 遗留回退）

**改进**:
- ✅ 替换硬编码 `[0,1]` 和 `[2..]` 假设
- ✅ 支持 1D/2D PE 数组动态配置
- ✅ 向后兼容保证

**代码统计**:
- 新增: ~110 行
- 修改: ~5 行
- 文件: 2 个 (SystolicTransform.cpp, ParametricSpaceTime.h)

---

### Step 2: 数据流分析参数化 (100%)
**文件**: `lib/Analysis/SpaceTimeAnalysis.cpp`

**新增功能**:
1. `analyzeOperandFlowsParametric()` - 参数化数据流分析
   - 支持 1D PE 数组（ST0/ST1/ST2）
   - 支持 2D PE 数组（ST3/ST4/ST5）
   - 基于访问投影模式自动推导流向

2. `analyzeDataFlow()` 增强 - 双路径策略
   - 优先使用参数化分析
   - 失败时回退到遗留 2D 模式

3. `inferParametricSpaceTime()` 增强
   - 自动同步 selectedSpaceLoops 和 timeLoops
   - 从 ParametricSpaceTime 提取循环索引

**核心算法**:
```cpp
// 1D PE 数组 (ST0/ST1/ST2):
if (usesSpaceDim[0]) → NONE (local)
else → HORIZONTAL (broadcast)

// 2D PE 数组 (ST3/ST4/ST5):
if (usesFirst && usesSecond) → NONE (local, C matrix)
else if (usesFirst) → HORIZONTAL (flows along 2nd dim, A matrix)
else if (usesSecond) → VERTICAL (flows along 1st dim, B matrix)
else → NONE (broadcast/special)
```

**代码统计**:
- 新增: ~160 行
- 修改: ~40 行
- 文件: 1 个 (SpaceTimeAnalysis.cpp)

---

## 📊 Phase 2 总体统计

| 指标 | Step 1 | Step 2 | 总计 |
|------|--------|--------|------|
| **新增代码** | 110 行 | 160 行 | **270 行** |
| **修改代码** | 5 行 | 40 行 | **45 行** |
| **新增函数** | 2 | 3 | **5** |
| **修改文件** | 2 | 1 | **3** |

## ✅ 编译验证

```
全部目标编译成功: 4/4
  - libSystolicAnalysis.a      ✅
  - libSystolicTransforms.a    ✅  
  - bin/systolic-opt           ✅
  - bin/systolic-translate     ✅

新增错误: 0
新增警告: 0 (仅预期的未使用变量警告)
```

## 🎯 功能特性

### 参数化支持
- ✅ ST0-ST5 完整支持
- ✅ 1D PE 数组 (ST0/ST1/ST2)
- ✅ 2D PE 数组 (ST3/ST4/ST5)
- ✅ 动态循环索引提取
- ✅ 访问投影数据流分析

### 向后兼容性
- ✅ 遗留函数完全保留
- ✅ 双路径回退机制
- ✅ 现有 ST3 默认行为不变
- ✅ 所有现有测试兼容

## 📝 关键设计

### 1. 双路径策略
**参数化优先 + 遗留回退**
```cpp
// SystolicTransform.cpp
if (parametricConfig.isValid()) {
  selectSpaceLoopsParametric(...);  // 新版本
  fallback to selectSpaceLoops(...);  // 旧版本
}

// SpaceTimeAnalysis.cpp
if (info.parametric.isValid()) {
  analyzeOperandFlowsParametric(...);  // 新版本
  fallback to analyzeOperandFlows(...);  // 旧版本
}
```

### 2. 访问投影模式识别
**自动推导数据流方向**
- 分析 memref 访问使用的循环归纳变量
- 匹配空间维度循环索引
- 根据使用模式确定流向 (HORIZONTAL/VERTICAL/NONE)

### 3. 配置同步
**SpaceTimeInfo ↔ ParametricSpaceTime**
- `inferParametricSpaceTime()` 同步循环索引
- `analyzeDataFlow()` 填充 operandFlows
- 双向更新保证一致性

## ⚠️ 已知限制

### SystolicDataflowGeneration 未集成
**原因**: 
- 该 Pass 通过函数属性获取配置
- Value 无法直接序列化到属性
- 需要重新设计配置传递机制

**影响**: 
- 数据流方向 (operandFlows) 尚未在代码生成中使用
- 当前只根据 load/store 模式简单分类

**计划**: 
- Phase 2 Step 3 将在 DataflowGeneration 中重新执行分析
- 或设计新的配置传递机制

## 🚀 下一步

### Phase 2 Step 3 (规划中)
**目标**: SystolicDataflowGeneration 参数化

**选项 1** - 重新执行分析:
```cpp
// 在 DataflowGeneration Pass 中
SpaceTimeInfo info;
analyzeSpaceTime(outerLoop, info);
inferParametricSpaceTime(info, ParametricSpaceTime::createFromMode(mode));
analyzeDataFlow(info);
// 使用 info.parametric.operandFlows
```

**选项 2** - 序列化配置:
```cpp
// 在 SystolicTransform 中存储 memref 名称映射
func->setAttr("systolic.flow_A", builder.getStringAttr("HORIZONTAL"));
func->setAttr("systolic.flow_B", builder.getStringAttr("VERTICAL"));
```

**预期工作量**: 1-2 天

---

## 📚 相关文档
- [PHASE1_FILES.md](PHASE1_FILES.md) - Phase 1 文件清单
- [PHASE2_STEP1_SUMMARY.md](PHASE2_STEP1_SUMMARY.md) - Step 1 详细总结
- [AUTOSA_REPRODUCTION_ROADMAP.md](docs/AUTOSA_REPRODUCTION_ROADMAP.md) - 完整技术路线

---

**更新时间**: 2026-01-07  
**版本**: Phase 2 Steps 1-2 完成  
**状态**: ✅ 编译通过，核心功能就绪
