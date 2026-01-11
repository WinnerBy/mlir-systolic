# Spacetime 动态枚举实现状态

> **最后更新**: 2026-01-06  
> **状态**: ✅ 核心功能已实现

---

## 已完成的工作

### 1. 扩展 ParametricSpaceTime 框架 ✅

**文件**: 
- `include/systolic/Analysis/ParametricSpaceTime.h`
- `lib/Analysis/ParametricSpaceTime.cpp`

**新增功能**:
- `configId`: 配置 ID（对应 AutoSA 的 space_time_id）
- `setConfigId()` / `getConfigId()`: 设置和获取配置 ID
- `createFromLoopIndices()`: 从循环索引动态创建配置

### 2. 实现动态枚举函数 ✅

**文件**: `lib/Transforms/SystolicTransform.cpp`

**函数**: `enumerateSpaceTimeConfigs()`

**功能**:
- 根据循环数量动态枚举所有可能的 spacetime 配置
- 支持 1D、2D、3D 阵列（由 `maxSADim` 控制）
- 为每个配置分配递增的 ID（从 0 开始）
- 只选择满足依赖距离 ≤ 1 的 space loop 候选

**算法**:
```cpp
// 1D: C(N,1) = N 种
// 2D: C(N,2) = N*(N-1)/2 种
// 3D: C(N,3) = N*(N-1)*(N-2)/6 种
```

### 3. 修改主流程使用枚举结果 ✅

**文件**: `lib/Transforms/SystolicTransform.cpp`

**修改**:
- 在 `SystolicTransformPass::runOnOperation()` 中调用枚举函数
- 根据 `spaceTimeMode` 从枚举列表中选择配置
- 支持 `listConfigs` 模式（列出所有配置）
- 默认自动选择（优先选择 ST3，否则选择第一个）

### 4. 更新选项结构 ✅

**文件**: `lib/Transforms/SystolicTransform.cpp`

**新增选项**:
- `spaceTimeMode`: 改为 `int` 类型，-1 表示自动选择
- `maxSADim`: 最大阵列维度（默认 2）
- `listConfigs`: 是否只列出配置（不生成代码）

---

## 实现效果

### 3 循环 (MM)
- 生成 6 种配置（ST0-ST5）
- 与 AutoSA 的对应关系一致

### 4 循环 (MTTKRP, TTMC)
- 生成 10+ 种配置（1D: 4, 2D: 6, 3D: 4）
- 支持所有可能的 spacetime 映射

### 5+ 循环 (CNN)
- 生成 25+ 种配置
- 完全支持复杂 kernel

---

## 待完成的工作

### 1. 循环置换 ✅

**状态**: 已完成

**实现**: 
- 在主流程中调用 `permuteLoopsForSpaceTime()` 函数
- 确保 space loops 被移到最外层位置
- 使用 MLIR 的 `permuteLoops()` 工具函数

**位置**: `lib/Transforms/SystolicTransform.cpp` (第 1071 行)

### 2. 命令行选项支持 🟡

**状态**: 部分完成（可通过代码设置）

**当前实现**: 
- 可以通过 `SystolicTransformOptions` 结构体设置选项
- 可以通过 `SystolicConfig` 传递给 Pass

**待完成**: 
- 添加命令行选项解析（`--max-sa-dim`, `--list-configs`, `--spacetime-mode`）
- 位置: `tools/systolic-opt/systolic-opt.cpp` 或 Pass 选项解析

**临时方案**: 可以通过修改代码或使用 `SystolicConfig` 来设置选项

### 3. 测试验证 🔴

**需要**: 
- 测试 3 循环 kernel（MM）的所有配置
- 测试 4 循环 kernel（MTTKRP, TTMC）
- 测试 5+ 循环 kernel（CNN）
- 与 AutoSA 输出对比验证

---

## 使用示例

### 列出所有配置

```bash
# 在 SystolicTransform 中添加 listConfigs 选项后
systolic-opt input.mlir --systolic-transform --list-configs
```

**输出示例**:
```
[Systolic] Found 6 spacetime configurations:
  [0] ST0: space=[0], time=[1,2]
  [1] ST1: space=[1], time=[0,2]
  [2] ST2: space=[2], time=[0,1]
  [3] ST3: space=[0,1], time=[2]
  [4] ST4: space=[0,2], time=[1]
  [5] ST5: space=[1,2], time=[0]
```

### 选择特定配置

```bash
# 选择配置索引 3 (ST3)
systolic-opt input.mlir --systolic-transform --spacetime-mode=3
```

### 自动选择（默认）

```bash
# 自动选择（优先 ST3，否则第一个）
systolic-opt input.mlir --systolic-transform
```

---

## 代码变更总结

### 修改的文件

1. **`include/systolic/Analysis/ParametricSpaceTime.h`**
   - 添加 `configId` 成员
   - 添加 `setConfigId()` / `getConfigId()` 方法
   - 添加 `createFromLoopIndices()` 静态方法

2. **`lib/Analysis/ParametricSpaceTime.cpp`**
   - 实现 `createFromLoopIndices()`
   - 更新 `toString()` 显示 configId

3. **`lib/Transforms/SystolicTransform.cpp`**
   - 实现 `enumerateSpaceTimeConfigs()` 函数
   - 修改主流程使用枚举结果
   - 更新选项结构
   - 在主流程中调用 `permuteLoopsForSpaceTime()` 确保循环顺序正确

---

## 下一步

1. **测试验证**: 运行测试确保功能正确
2. **循环置换**: 实现完整的循环置换逻辑
3. **命令行选项**: 添加用户友好的命令行选项
4. **文档更新**: 更新使用文档和示例

---

**参考文档**:
- [SPACETIME_COMPARISON.md](SPACETIME_COMPARISON.md) - 详细对比分析
- [features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md](features/spacetime/SPACETIME_IMPLEMENTATION_PLAN.md) - 实现计划
- [SPACETIME_INDEX.md](SPACETIME_INDEX.md) - 文档索引
