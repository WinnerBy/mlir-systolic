# 写时重排实现进度

> **创建时间**: 2026-01-05  
> **状态**: 进行中

---

## 实现概述

写时重排（Write-time Reordering）用于解决随机读取问题，通过在写入时重排数组维度，使读取时能够顺序访问。

---

## 已完成的工作

### 1. 基础框架 ✅

**文件**: `tools/systolic-translate/systolic-translate.cpp`

**添加内容**:
- `ArrayAccessAnalyzer` 类：检测和分析随机读取问题
  - `hasNonLinearIndex()`: 检测非线性索引表达式
  - `needsReordering()`: 判断是否需要重排
  - `computeReordering()`: 计算最优重排方案

- `SystolicHLSEmitter` 扩展：
  - `arrayLayouts`: 存储数组布局信息
  - `reorderings`: 存储重排方案
  - `registerArrayLayout()`: 注册数组布局
  - `hasReordering()`: 检查是否有重排
  - `getReorderedDims()`: 获取重排后的维度
  - `getReorderedArrayDecl()`: 生成重排后的数组声明

### 2. 检测逻辑 ✅

**检测模式**:
- 查找包含 `*`, `/`, `%` 的索引表达式
- 分析读取访问模式
- 确定需要重排的维度

**重排策略**:
- 将包含非线性索引的维度移到中间位置
- 示例：`[8][2][4]` 如果 dim2 有非线性索引 → `[2][4][8]`

---

## 已完成的工作（续）

### 3. 代码生成函数修改 ✅

**修改的函数**:
- ✅ `emitIOL2InIntraTrans()`: 使用重排后的数组布局和索引
- ✅ `emitIOL2InInterTrans()`: 写入时应用重排索引映射
- ✅ `emitIOL2InInterTransBoundary()`: 边界情况处理
- ✅ `emitModuleDeclarations()`: 函数声明中使用重排后的数组类型

**实现内容**:
1. ✅ 在生成数组声明时使用 `getReorderedArrayDecl()`
2. ✅ 在 `inter_trans` 中写入时应用重排索引映射（`getReorderedWriteIndex()`）
3. ✅ 在 `intra_trans` 中读取时使用重排后的索引（`getReorderedReadIndex()`）
4. ✅ 添加了索引映射方法：`getReorderedWriteIndex()` 和 `getReorderedReadIndex()`

### 4. 索引映射逻辑 ✅

**实现内容**:
- `getReorderedWriteIndex()`: 将原始写入索引映射到重排后的索引
  - 示例：`[c4][c5][c6]` → `[c5][c6][c4]`（将第一维移到最后）
- `getReorderedReadIndex()`: 将原始读取索引映射到重排后的索引
  - 示例：`[c8][c5][8 * c6 / 16]` → `[c5][8 * c6 / 16][c8]`（非线性索引移到中间）

---

## 待完成的工作

### 1. 从 MLIR 分析访问模式 ⚠️

### 2. 从 MLIR 分析访问模式 ⚠️

**当前限制**:
- 当前代码生成器基于简单参数（arrayPart, latency, simd）
- 没有分析 MLIR 中的实际访问模式
- 需要从 MLIR 中提取访问模式信息

**解决方案**:
- 在 `SystolicDataflowGeneration` pass 中分析访问模式
- 将访问模式信息传递给代码生成器
- 或者通过命令行参数指定需要重排的数组

**当前状态**:
- ✅ 框架已就绪，可以通过 `registerArrayLayout()` 手动注册
- ⚠️ 需要添加自动检测或配置支持

### 3. 支持复杂 Kernel ⚠️

**当前支持**:
- 仅支持简单的 MM kernel（`local_A[latency][1]`）

**需要支持**:
- MTTKRP: `local_A[16][2][16]` 需要重排为 `local_A[2][16][16]`
- TTMc: 可能也需要重排
- CNN: 复杂的多维数组

**实现方式**:
- 扩展 `registerArrayLayout()` 以支持从配置或 MLIR 分析中获取布局
- 添加命令行参数或配置文件来指定复杂 kernel 的布局

---

## 实现示例

### MTTKRP 案例

**原始布局**:
```cpp
A_t16 local_A[16][2][16];
// 写入: local_A[c4][c5][c6] = out_data;
// 读取: local_A[c8][c5][8 * c6 / 16];  // 非线性索引
```

**重排后布局**:
```cpp
A_t16 local_A[2][16][16];
// 写入: local_A[c5][c6][c4] = out_data;  // 重排
// 读取: local_A[c5][8 * c6 / 16][c8];    // 顺序访问
```

**实现代码**:
```cpp
// 注册布局
emitter.registerArrayLayout("A", 
                           {16, 2, 16},  // 原始维度
                           {"c4", "c5", "c6"},  // 写入索引
                           {"c8", "c5", "8 * c6 / 16"});  // 读取索引

// 生成代码时应用重排
if (emitter.hasReordering("A")) {
  auto dims = emitter.getReorderedDims("A");  // {2, 16, 16}
  // 使用重排后的维度生成数组声明和访问
}
```

---

## 下一步计划

### 短期（1-2 天）

1. **修改代码生成函数**
   - 修改 `emitIOL2InIntraTrans()` 以支持重排
   - 修改 `emitIOL2InInterTrans()` 以应用重排
   - 测试简单的重排案例

2. **添加配置支持**
   - 添加命令行参数指定需要重排的数组
   - 或从配置文件读取布局信息

### 中期（3-5 天）

3. **MLIR 层面分析**
   - 在 `SystolicDataflowGeneration` 中分析访问模式
   - 自动检测需要重排的数组
   - 将信息传递给代码生成器

4. **测试验证**
   - 测试 MTTKRP kernel
   - 验证重排后的代码正确性
   - 对比 HLS 综合结果

### 长期（1-2 周）

5. **扩展到其他 Kernel**
   - 支持 TTMc
   - 支持 CNN
   - 通用化重排逻辑

---

## 参考文档

- [随机读取问题分析](RANDOM_ACCESS_ANALYSIS.md) - 问题分析和解决方案
- [下一阶段工作计划](NEXT_PHASE_WORK_PLAN.md) - 任务 1: 写时重排

---

## 更新日志

- **2026-01-05**: 创建实现进度文档
  - 添加基础框架（ArrayAccessAnalyzer）
  - 添加检测和重排逻辑
  - 记录待完成工作

- **2026-01-05**: 完成代码生成函数修改
  - ✅ 修改 `emitIOL2InIntraTrans()` 支持重排
  - ✅ 修改 `emitIOL2InInterTrans()` 应用重排
  - ✅ 修改 `emitIOL2InInterTransBoundary()` 支持重排
  - ✅ 修改 `emitModuleDeclarations()` 使用重排后的数组类型
  - ✅ 实现索引映射方法（`getReorderedWriteIndex()` 和 `getReorderedReadIndex()`）
  - ✅ 添加注册数组布局的示例代码（在 `emit()` 函数中）

