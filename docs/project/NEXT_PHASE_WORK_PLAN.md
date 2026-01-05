# 下一阶段工作计划

> **创建时间**: 2026-01-05  
> **目的**: 整合下一阶段的三个主要工作任务，制定详细的实施计划

---

## 工作概述

下一阶段需要完成三个主要任务：

1. **实现写时重排**：基于随机读取问题和写时重排解决方案
2. **扩展 MM 的 Spacetime 支持**：实现所有 6 种 Space-time 配置
3. **扩展其他 Kernel 的 Spacetime 支持**：CNN、MTTKRP、TTMc 等

---

## 任务 1: 实现写时重排（Write-time Reordering）

### 1.1 问题背景

**随机读取问题**：
- 在 `IO_L2_in_intra_trans` 函数中，从 L2 缓冲区读取数据时出现非线性索引
- 索引表达式包含 `*`, `/`, `%` 等非线性操作
- 导致非顺序访问模式，影响 HLS 性能和资源使用

**解决方案**：数组维度重排（写时重排）
- 在 `inter_trans`（写入）时进行数组维度重排
- 在 `intra_trans`（读取）时使用重排后的布局
- 将非线性索引移到中间维度，实现顺序访问

### 1.2 实施步骤

#### 步骤 1.1: 检测随机读取问题

**目标**：自动检测需要重排的数组访问模式

**实现位置**：
- `lib/Transforms/SystolicDataflowGeneration.cpp`
- 在生成 `IO_L2_in_intra_trans` 时检测非线性索引

**检测逻辑**：
```cpp
// 检测模式：
// 1. 查找 local_*[...][...][...] 访问
// 2. 检查索引表达式是否包含 *, /, % 操作
// 3. 分析访问模式是否非顺序
```

**参考**：
- `docs/RANDOM_ACCESS_ANALYSIS.md` - 问题分析和检测方法
- `scripts/check_random_access.sh` - 检查脚本

#### 步骤 1.2: 分析数组维度布局

**目标**：确定最优的数组维度重排方案

**分析内容**：
1. **当前布局**：分析 `inter_trans` 中的写入模式
2. **目标布局**：分析 `intra_trans` 中的读取模式
3. **重排方案**：确定维度重排顺序

**示例**（MTTKRP）：
```
原始布局: local_A[8][2][4]
写入模式: local_A[c8][c5][c4]
读取模式: local_A[c8][c5][4 * c6 / 16]  // 非线性索引

重排方案: local_A[2][4][8]
写入模式: local_A[c5][c6][c4]  // 重排
读取模式: local_A[c5][4 * c6 / 16][c8]  // 顺序访问
```

#### 步骤 1.3: 实现写时重排逻辑

**目标**：在代码生成时应用数组维度重排

**实现位置**：
- `tools/systolic-translate/systolic-translate.cpp`
- `EmitHLSCpp` translation 中的 `IO_L2_in` 模块生成

**实现内容**：
1. **修改数组声明**：
   ```cpp
   // 原始: A_t16 local_A[8][2][4];
   // 重排: A_t16 local_A[2][4][8];
   ```

2. **修改 inter_trans 写入**：
   ```cpp
   // 原始: local_A[c8][c5][c4] = out_data;
   // 重排: local_A[c5][c6][c4] = out_data;  // 根据重排方案调整
   ```

3. **修改 intra_trans 读取**：
   ```cpp
   // 原始: in_data = local_A[c8][c5][4 * c6 / 16];
   // 重排: in_data = local_A[c5][4 * c6 / 16][c8];  // 顺序访问
   ```

#### 步骤 1.4: 测试验证

**测试用例**：
- MTTKRP（已确认有随机读取问题）
- TTMc（可能有问题）
- TTM（可能有问题）
- CNN（可能有问题）

**验证方法**：
1. 运行 `scripts/check_random_access.sh` 检查
2. 对比重排前后的访问模式
3. 验证 HLS 综合结果（资源使用、性能）

### 1.3 预期结果

- ✅ 自动检测随机读取问题
- ✅ 自动应用数组维度重排
- ✅ 消除非线性索引访问
- ✅ 改善 HLS 综合性能

### 1.4 时间估算

- 检测逻辑实现：2-3 天
- 重排方案分析：2-3 天
- 代码生成实现：3-4 天
- 测试验证：2-3 天
- **总计：9-13 天**

---

## 任务 2: 扩展 MM 的 Spacetime 支持

### 2.1 当前状态

**已实现**：
- ✅ Space-time=3（2D Output-Stationary）：`[i, j]` 空间循环

**待实现**：
- ⚠️ Space-time=0（1D Output-Stationary）：`[i]` 空间循环
- ⚠️ Space-time=1（1D Output-Stationary）：`[j]` 空间循环
- ⚠️ Space-time=2（1D Input-Stationary）：`[k]` 空间循环（需要 reduction）
- ⚠️ Space-time=4（2D with H-Reduce）：`[i, k]` 空间循环（需要 reduction）
- ⚠️ Space-time=5（2D with V-Reduce）：`[j, k]` 空间循环（需要 reduction）

### 2.2 实施步骤

#### 步骤 2.1: 实现 Space-time=0 和 1（1D Output-Stationary）

**目标**：支持 1D 脉动阵列

**实现要点**：

1. **修改 PE_wrapper 生成**：
   - Space-time=0：B 在 PE 间水平传递
   - Space-time=1：A 在 PE 间垂直传递

2. **修改 IO 模块生成**：
   - 简化 IO 层级（L2, L1，无 L3）
   - 根据数据流方向生成不同的 IO 模块
   - 实现 Direct 数据流（A 或 B 直接发送）

3. **修改数据流生成**：
   - Horizontal 数据流（从左到右）
   - Vertical 数据流（从上到下）
   - 边界 PE 的特殊处理

**参考文档**：
- `docs/SPACETIME_IMPLEMENTATION_PLAN.md` - 详细实现方案
- `test/reference-samples/mm_st0_*.cpp` - 参考代码
- `test/reference-samples/mm_st1_*.cpp` - 参考代码

#### 步骤 2.2: 实现 Space-time=2（1D Input-Stationary）

**目标**：支持 1D reduction 脉动阵列

**实现要点**：

1. **Reduction 支持**：
   - C 沿 PE 累加（不是本地累加）
   - 实现累加数据流（从上游到下游）

2. **PE_wrapper 修改**：
   ```cpp
   void PE_wrapper(int idx, 
                   hls::stream<A_t> &fifo_A_in,      // A: broadcast
                   hls::stream<B_t> &fifo_B_in,      // B: broadcast
                   hls::stream<float> &fifo_C_in,    // C: 从上游 PE 输入
                   hls::stream<float> &fifo_C_out);  // C: 向下游 PE 输出（累加）
   ```

3. **IO 模块修改**：
   - C: L2_out（不是 drain，是累加输出）

**参考文档**：
- `docs/SPACETIME_IMPLEMENTATION_PLAN.md` - Space-time=2 部分
- `test/reference-samples/mm_st2_*.cpp` - 参考代码

#### 步骤 2.3: 实现 Space-time=4 和 5（2D with Reduction）

**目标**：支持 2D reduction 脉动阵列

**实现要点**：

1. **Space-time=4（H-Reduce）**：
   - C 水平累加（沿 j 方向）
   - B 垂直重用（沿 i 方向）
   - A 直接发送

2. **Space-time=5（V-Reduce）**：
   - C 垂直累加（沿 i 方向）
   - A 垂直重用（沿 j 方向）
   - B 直接发送

3. **Reduction 数据流**：
   - 实现累加数据流（水平或垂直）
   - 边界 PE 的特殊处理

**参考文档**：
- `docs/SPACETIME_IMPLEMENTATION_PLAN.md` - Space-time=4, 5 部分
- `test/reference-samples/mm_st4_*.cpp` - 参考代码
- `test/reference-samples/mm_st5_*.cpp` - 参考代码

### 2.3 测试验证

**测试用例**：
- Space-time=0: `mm_st0_I32_J32_K32_ap32_lat8_simd2`
- Space-time=1: `mm_st1_I32_J32_K32_ap32_lat8_simd2`
- Space-time=2: `mm_st2_I32_J32_K32_ap4_lat2_simd2`
- Space-time=4: `mm_st4_I32_J32_K32_ap32_lat16_simd2`
- Space-time=5: `mm_st5_I32_J32_K32_ap32_lat16_simd2`

**验证指标**：
- PIPELINE 数量（与参考代码一致）
- 代码行数（接近参考代码）
- 功能正确性（与参考代码对比）

### 2.4 预期结果

- ✅ 支持所有 6 种 Space-time 配置
- ✅ PIPELINE 数量与参考代码一致
- ✅ 代码行数接近参考代码
- ✅ 功能正确性验证通过

### 2.5 时间估算

- Space-time=0, 1 实现：4-6 天
- Space-time=2 实现：3-4 天
- Space-time=4, 5 实现：4-6 天
- 测试验证：3-4 天
- **总计：14-20 天**

---

## 任务 3: 扩展其他 Kernel 的 Spacetime 支持

### 3.1 当前状态

**已支持**：
- ✅ MM（矩阵乘法）：Space-time=3

**待支持**：
- ⚠️ CNN（卷积神经网络）
- ⚠️ MTTKRP（张量矩阵乘法）
- ⚠️ TTMc（张量链乘法）
- ⚠️ TTM（张量矩阵乘法）
- ⚠️ LU（LU 分解）

### 3.2 实施策略

**核心思路**：
- Spacetime 是多面体要划分的时空空间
- 对于不同 kernel，不同的 spacetime 可能产生不同的脉动阵列结构
- 但处理思路延续 MM 对不同 spacetime 的处理方式

**实施步骤**：

#### 步骤 3.1: 分析每个 Kernel 的 Spacetime 需求

**分析内容**：
1. **循环结构**：识别 kernel 的循环嵌套
2. **数据依赖**：分析数据依赖关系
3. **Spacetime 映射**：确定哪些循环可以映射为空间循环

**Kernel 分析**：

| Kernel | 循环结构 | 可能的 Spacetime | 特点 |
|--------|----------|-----------------|------|
| **CNN** | `for o, for i, for r, for c, for k` | 0-5 | 5 维循环，复杂 |
| **MTTKRP** | `for i, for j, for k, for l` | 0-5 | 4 维循环 |
| **TTMc** | `for i, for j, for k, for l, for m` | 0-5 | 5 维循环 |
| **LU** | `for i, for j, for k` | 0-3 | 3 维循环，需要特殊处理 |

#### 步骤 3.2: 实现 Kernel 特定的 Spacetime 支持

**实现方式**：
1. **复用 MM 的 Spacetime 处理逻辑**
2. **适配 Kernel 特定的循环结构**
3. **处理 Kernel 特定的数据流**

**实现位置**：
- `lib/Transforms/SystolicTransform.cpp` - Spacetime 映射
- `lib/Transforms/SystolicDataflowGeneration.cpp` - 数据流生成
- `tools/systolic-translate/systolic-translate.cpp` - 代码生成

#### 步骤 3.3: 处理 Kernel 特定的问题

**CNN**：
- 5 维循环结构
- 卷积窗口滑动
- 边界处理

**MTTKRP/TTMc**：
- 高维张量
- 随机读取问题（需要结合任务 1）
- 复杂的数据重用模式

**LU**：
- 需要 C++ 模板支持
- 特殊的数据依赖（下三角矩阵）

### 3.3 测试验证

**测试用例**：
- CNN: `cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2`
- MTTKRP: `mttkrp_st3_ap128_128_2_lat16_8_simd8_1`
- TTMc: `ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1`
- LU: `lu_st3_ap_auto_lat_auto`

**验证方法**：
- 与 AutoSA 参考代码对比
- 功能正确性验证
- HLS 综合验证

### 3.4 预期结果

- ✅ 支持主要 Kernel 的 Spacetime 配置
- ✅ 功能正确性验证通过
- ✅ 代码质量与参考代码接近

### 3.5 时间估算

- Kernel 分析：3-5 天
- CNN 实现：5-7 天
- MTTKRP/TTMc 实现：5-7 天
- LU 实现：3-5 天
- 测试验证：5-7 天
- **总计：21-31 天**

---

## 总体时间估算

| 任务 | 时间估算 | 优先级 |
|------|----------|--------|
| 任务 1: 写时重排 | 9-13 天 | 高（影响多个 kernel） |
| 任务 2: MM Spacetime 扩展 | 14-20 天 | 高（核心功能） |
| 任务 3: 其他 Kernel Spacetime | 21-31 天 | 中（逐步扩展） |
| **总计** | **44-64 天** | |

---

## 实施顺序建议

### 阶段 1: 写时重排（优先级最高）

**原因**：
- 影响多个 kernel（MTTKRP、TTMc 等）
- 是其他任务的基础
- 相对独立，可以快速完成

**时间**：9-13 天

### 阶段 2: MM Spacetime 扩展（核心功能）

**原因**：
- MM 是最基础的 kernel
- 为其他 kernel 提供参考
- 验证 Spacetime 实现的正确性

**时间**：14-20 天

### 阶段 3: 其他 Kernel Spacetime（逐步扩展）

**原因**：
- 基于 MM 的实现经验
- 可以复用大部分逻辑
- 逐步支持，降低风险

**时间**：21-31 天

---

## 相关文档

### 核心参考文档
- [随机读取问题分析](RANDOM_ACCESS_ANALYSIS.md) - 写时重排解决方案
- [Space-time 扩展实现方案](SPACETIME_IMPLEMENTATION_PLAN.md) - 详细实现方案
- [Space-time 扩展实施路线图](IMPLEMENTATION_ROADMAP.md) - 实施计划
- [AutoSA Spacetime 分析](AUTOSA_SPACETIME_ANALYSIS.md) - Spacetime 参数分析

### 参考代码
- `test/reference-samples/` - AutoSA 生成的参考代码
- `autosa_hls_output/` - 其他 kernel 的参考代码

---

## 更新日志

- **2026-01-05**: 创建下一阶段工作计划
  - 整合三个主要任务
  - 制定详细的实施步骤
  - 估算时间和优先级


