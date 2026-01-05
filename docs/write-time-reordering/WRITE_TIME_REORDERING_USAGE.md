# 写时重排使用指南

> **创建时间**: 2026-01-05  
> **目的**: 说明如何使用写时重排功能

---

## 概述

写时重排功能已实现，可以通过 `registerArrayLayout()` 方法注册数组布局，代码生成器会自动应用重排。

---

## 使用方法

### 1. 注册数组布局

在 `SystolicHLSEmitter::emit()` 函数中，调用 `registerArrayLayout()` 注册需要重排的数组：

```cpp
// 示例：MTTKRP kernel
emitter.registerArrayLayout("A", 
                           {16, 2, 16},                    // 原始维度 [dim0][dim1][dim2]
                           {"c4", "c5", "c6"},             // 写入索引
                           {"c8", "c5", "8 * c6 / 16"});  // 读取索引（包含非线性表达式）
```

### 2. 自动应用重排

注册后，代码生成器会自动：
- 检测是否需要重排（通过 `ArrayAccessAnalyzer::needsReordering()`）
- 计算重排方案（通过 `ArrayAccessAnalyzer::computeReordering()`）
- 在代码生成时应用重排：
  - 函数签名中使用重排后的数组声明
  - `inter_trans` 中使用重排后的写入索引
  - `intra_trans` 中使用重排后的读取索引

---

## 示例：MTTKRP Kernel

### 问题描述

MTTKRP kernel 中，数组 `local_A` 的访问模式：
- **原始布局**: `A_t16 local_A[16][2][16]`
- **写入模式**: `local_A[c4][c5][c6] = out_data;`
- **读取模式**: `local_A[c8][c5][8 * c6 / 16];` ← 非线性索引 `8 * c6 / 16`

### 解决方案

注册数组布局后，代码生成器会自动重排：

```cpp
// 在 emit() 函数中注册
emitter.registerArrayLayout("A", 
                           {16, 2, 16},                    // 原始维度
                           {"c4", "c5", "c6"},             // 写入索引
                           {"c8", "c5", "8 * c6 / 16"});  // 读取索引
```

**生成的代码**：
```cpp
// 重排后的数组声明
void A_IO_L2_in_intra_trans(..., A_t16 local_A[2][16][16], ...);

// 写入时应用重排
void A_IO_L2_in_inter_trans(...) {
  // ...
  local_A[c5][c6][c4] = out_data;  // 重排后的索引
}

// 读取时使用重排后的布局
void A_IO_L2_in_intra_trans(...) {
  // ...
  in_data = local_A[c5][8 * c6 / 16][c8];  // 非线性索引移到中间，顺序访问
}
```

---

## 重排策略

### 检测逻辑

`ArrayAccessAnalyzer` 会：
1. 检查读取索引中是否包含非线性操作（`*`, `/`, `%`）
2. 如果发现非线性索引，标记需要重排

### 重排方案

**策略**：将包含非线性索引的维度移到中间位置

**示例**：
- 原始：`[16][2][16]`，非线性在 dim2
- 重排：`[2][16][16]`（将 dim2 移到中间）

**索引映射**：
- 写入：`[c4][c5][c6]` → `[c5][c6][c4]`
- 读取：`[c8][c5][8 * c6 / 16]` → `[c5][8 * c6 / 16][c8]`

---

## 当前限制

### 1. 需要手动注册

当前实现需要手动调用 `registerArrayLayout()` 注册数组布局。未来可以：
- 从 MLIR 分析中自动检测
- 从配置文件读取
- 通过命令行参数指定

### 2. 仅支持 3 维数组

当前实现主要针对 3 维数组的重排。对于其他维度，需要扩展重排逻辑。

### 3. 重排模式固定

当前重排策略是将非线性维度移到中间。对于更复杂的情况，可能需要更灵活的重排策略。

---

## 下一步改进

### 1. 自动检测（优先级：高）

在 `SystolicDataflowGeneration` pass 中：
- 分析数组访问模式
- 自动检测非线性索引
- 自动计算重排方案
- 将信息传递给代码生成器

### 2. 配置支持（优先级：中）

添加命令行参数或配置文件：
```bash
--reorder-array A:16,2,16:c4,c5,c6:c8,c5,8*c6/16
```

### 3. 扩展支持（优先级：低）

- 支持 2 维和 4+ 维数组
- 支持更复杂的重排策略
- 支持多个数组同时重排

---

## 测试验证

### 测试步骤

1. **注册数组布局**：
   ```cpp
   emitter.registerArrayLayout("A", {16, 2, 16}, {"c4", "c5", "c6"}, {"c8", "c5", "8 * c6 / 16"});
   ```

2. **生成代码**：
   ```bash
   ./build/bin/systolic-translate input.mlir -o output.cpp
   ```

3. **验证重排**：
   - 检查函数签名中的数组声明
   - 检查 `inter_trans` 中的写入索引
   - 检查 `intra_trans` 中的读取索引
   - 运行 `scripts/check_random_access.sh` 验证无随机访问

4. **HLS 综合验证**：
   - 对比重排前后的资源使用
   - 验证性能改善

---

## 相关文档

- [写时重排实现进度](WRITE_TIME_REORDERING_IMPLEMENTATION.md) - 实现细节
- [随机读取问题分析](RANDOM_ACCESS_ANALYSIS.md) - 问题背景
- [下一阶段工作计划](NEXT_PHASE_WORK_PLAN.md) - 任务 1

---

## 更新日志

- **2026-01-05**: 创建使用指南
  - 说明如何使用写时重排功能
  - 提供 MTTKRP 示例
  - 记录当前限制和下一步改进


