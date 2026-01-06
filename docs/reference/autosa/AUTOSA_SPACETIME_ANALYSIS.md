# AutoSA Spacetime 参数详细分析

> **最后更新**: 2024-12  
> **目的**: 分析 AutoSA 的 spacetime 参数、多维参数支持，以及当前实现的局限性

---

## 1. Spacetime 参数概述

### 1.1 什么是 Spacetime？

**Spacetime** 是 AutoSA 中用于选择不同数据流（dataflow）的索引参数。它决定了哪些循环被映射为**空间循环**（space loops），哪些被映射为**时间循环**（time loops）。

- **空间循环（Space Loops）**：映射到不同的 PE，执行并发计算
- **时间循环（Time Loops）**：描述每个 PE 内部的串行计算

### 1.2 Spacetime 值的含义

对于**矩阵乘法（MM）**，AutoSA 可以生成 6 种不同的 systolic array：

| Spacetime | 空间循环 | 数组类型 | 数据流模式 | 特点 |
|-----------|----------|----------|------------|------|
| **0** | `[i]` | 1D | Output-stationary | C 在 PE 内计算，B 在 PE 间重用，A 直接发送 |
| **1** | `[j]` | 1D | Output-stationary | C 在 PE 内计算，A 在 PE 间重用，B 直接发送 |
| **2** | `[k]` | 1D | Input-stationary | C 沿 PE 累加，A 和 B 直接发送（需要 reduction） |
| **3** | `[i,j]` | 2D | Output-stationary | C 在 PE 内计算，A 水平重用，B 垂直重用 ✅ **当前支持** |
| **4** | `[i,k]` | 2D | - | C 水平累加，B 垂直重用，A 直接发送（需要 reduction） |
| **5** | `[j,k]` | 2D | - | C 水平累加，A 垂直重用，B 直接发送（需要 reduction） |

### 1.3 当前实现的局限性

**我们当前只支持 spacetime=3**：
- ✅ 2D systolic array
- ✅ 空间循环：`[i, j]`
- ✅ 时间循环：`[k]`
- ✅ Output-stationary 数据流

**需要补充的 spacetime 值**：
- ⚠️ spacetime=0, 1, 2：1D systolic array
- ⚠️ spacetime=4, 5：2D systolic array（需要 reduction 支持）

---

## 2. 多维参数支持

### 2.1 当前实现的局限性

**我们当前强制所有维度使用相同的值**：
- `array_part[8, 8, 8]` → 我们只支持 `array_part=8`
- `latency[8, 8]` → 我们只支持 `latency=8`
- `simd[2]` → 我们只支持 `simd=2`

### 2.2 AutoSA 支持的多维参数

#### 2.2.1 array_part（数组分区）

**示例**：
```bash
# 矩阵乘法：array_part[16,16,16] - 三个维度相同
--sa-sizes="{kernel[]->array_part[16,16,16]}"

# 矩阵乘法（大）：array_part[260,256,512] - 三个维度不同
--sa-sizes="{kernel[]->array_part[260,256,512]}"

# CNN：array_part[8,8,4,8] - 四个维度
--sa-sizes="{kernel[]->array_part[8,8,4,8]}"
```

**含义**：
- `array_part[i]`：第 i 个循环维度的分块大小
- 对于矩阵乘法（3 个循环：i, j, k）：
  - `array_part[0]`：循环 i 的分块大小
  - `array_part[1]`：循环 j 的分块大小
  - `array_part[2]`：循环 k 的分块大小（通常不用于空间循环）

#### 2.2.2 latency（延迟隐藏）

**示例**：
```bash
# 矩阵乘法：latency[8,8] - 两个维度相同
--sa-sizes="{kernel[]->latency[8,8]}"

# 矩阵乘法（大）：latency[20,16] - 两个维度不同
--sa-sizes="{kernel[]->latency[20,16]}"

# CNN：latency[4,2,4] - 三个维度
--sa-sizes="{kernel[]->latency[4,2,4]}"
```

**含义**：
- `latency[i]`：第 i 个并行循环的延迟隐藏因子
- 用于 strip-mining 并行循环，实现 II=1
- 对于 2D array（spacetime=3）：
  - `latency[0]`：循环 i 的延迟隐藏因子
  - `latency[1]`：循环 j 的延迟隐藏因子

#### 2.2.3 simd（SIMD 向量化）

**示例**：
```bash
# 矩阵乘法：simd[2] - 单个值
--sa-sizes="{kernel[]->simd[2]}"

# CNN：simd[1,1,1,2] - 四个维度
--sa-sizes="{kernel[]->simd[1,1,1,2]}"

# 矩阵乘法（大）：simd[8] - 单个值
--sa-sizes="{kernel[]->simd[8]}"
```

**含义**：
- `simd[i]`：第 i 个循环的 SIMD 因子
- 可以是单个值（应用于所有循环）或数组（每个循环不同）
- 对于矩阵乘法（spacetime=3）：
  - `simd[0]`：通常应用于循环 k（reduction 循环）

---

## 3. 不同 Spacetime 值的详细分析

### 3.1 Spacetime=0: [i] - 1D Output-Stationary

**特点**：
- 1D systolic array
- 空间循环：`[i]`
- 时间循环：`[j, k]`
- C 在 PE 内计算
- B 在 PE 间重用（水平方向）
- A 直接发送到每个 PE

**示例命令**：
```bash
./autosa ./autosa_tests/mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[0];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}"
```

**生成的 PE 阵列**：
- 1×4 1D array（如果 `array_part[0]=32`, `latency[0]=8` → `32/8=4`）

**需要实现的功能**：
- ⚠️ 1D PE 阵列生成
- ⚠️ 水平数据重用（B 矩阵）
- ⚠️ 垂直数据直接发送（A 矩阵）

---

### 3.2 Spacetime=1: [j] - 1D Output-Stationary

**特点**：
- 1D systolic array
- 空间循环：`[j]`
- 时间循环：`[i, k]`
- C 在 PE 内计算
- A 在 PE 间重用（垂直方向）
- B 直接发送到每个 PE

**示例命令**：
```bash
./autosa ./autosa_tests/mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[1];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}"
```

**生成的 PE 阵列**：
- 1×4 1D array（如果 `array_part[1]=32`, `latency[1]=8` → `32/8=4`）

**需要实现的功能**：
- ⚠️ 1D PE 阵列生成
- ⚠️ 垂直数据重用（A 矩阵）
- ⚠️ 水平数据直接发送（B 矩阵）

---

### 3.3 Spacetime=2: [k] - 1D Input-Stationary

**特点**：
- 1D systolic array
- 空间循环：`[k]`
- 时间循环：`[i, j]`
- C 沿 PE 累加（reduction）
- A 和 B 直接发送到每个 PE

**示例命令**：
```bash
./autosa ./autosa_tests/mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[2];kernel[]->array_part[4,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
    --local-reduce \
    --reduce-op="+" \
    --simd-touch-space \
    --array-contraction
```

**生成的 PE 阵列**：
- 1×2 1D array（如果 `array_part[2]=4`, `latency[0]=2` → `4/2=2`）

**需要实现的功能**：
- ⚠️ 1D PE 阵列生成
- ⚠️ Reduction 支持（`--local-reduce`, `--reduce-op`）
- ⚠️ SIMD 应用于空间循环（`--simd-touch-space`）
- ⚠️ 数组收缩优化（`--array-contraction`）

---

### 3.4 Spacetime=3: [i,j] - 2D Output-Stationary ✅ **当前支持**

**特点**：
- 2D systolic array
- 空间循环：`[i, j]`
- 时间循环：`[k]`
- C 在 PE 内计算
- A 水平重用（沿 j 方向）
- B 垂直重用（沿 i 方向）

**示例命令**：
```bash
./autosa ./autosa_tests/mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}"
```

**生成的 PE 阵列**：
- 2×2 2D array（如果 `array_part[0]=16`, `latency[0]=8` → `16/8=2`）

**当前实现状态**：
- ✅ 2D PE 阵列生成
- ✅ 水平数据重用（A 矩阵）
- ✅ 垂直数据重用（B 矩阵）
- ✅ 双缓冲支持
- ✅ Drain 模块支持

---

### 3.5 Spacetime=4: [i,k] - 2D with Reduction

**特点**：
- 2D systolic array
- 空间循环：`[i, k]`
- 时间循环：`[j]`
- C 水平累加（reduction）
- B 垂直重用（沿 i 方向）
- A 直接发送到每个 PE

**示例命令**：
```bash
./autosa ./autosa_tests/mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
    --local-reduce \
    --reduce-op="+" \
    --simd-touch-space \
    --array-contraction
```

**生成的 PE 阵列**：
- 2×2 2D array（如果 `array_part[0]=32`, `latency[0]=16` → `32/16=2`；`array_part[2]=32`, `latency[1]=16` → `32/16=2`）

**需要实现的功能**：
- ⚠️ 2D PE 阵列生成（不同的空间循环组合）
- ⚠️ Reduction 支持
- ⚠️ SIMD 应用于空间循环
- ⚠️ 数组收缩优化

---

### 3.6 Spacetime=5: [j,k] - 2D with Reduction

**特点**：
- 2D systolic array
- 空间循环：`[j, k]`
- 时间循环：`[i]`
- C 水平累加（reduction）
- A 垂直重用（沿 j 方向）
- B 直接发送到每个 PE

**示例命令**：
```bash
./autosa ./autosa_tests/mm/kernel.c \
    --sa-sizes="{kernel[]->space_time[5];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
    --local-reduce \
    --reduce-op="+" \
    --simd-touch-space \
    --array-contraction
```

**生成的 PE 阵列**：
- 2×2 2D array（如果 `array_part[1]=32`, `latency[0]=16` → `32/16=2`；`array_part[2]=32`, `latency[1]=16` → `32/16=2`）

**需要实现的功能**：
- ⚠️ 2D PE 阵列生成（不同的空间循环组合）
- ⚠️ Reduction 支持
- ⚠️ SIMD 应用于空间循环
- ⚠️ 数组收缩优化

---

## 4. 多维参数示例分析

### 4.1 矩阵乘法示例

#### 示例 1：相同维度
```bash
--sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}"
```
- `array_part[16,16,16]`：三个维度都是 16
- `latency[8,8]`：两个维度都是 8
- `simd[2]`：单个值，应用于所有循环

#### 示例 2：不同维度
```bash
--sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[260,256,512];kernel[]->latency[20,16];kernel[]->simd[8]}"
```
- `array_part[260,256,512]`：三个维度不同
- `latency[20,16]`：两个维度不同
- `simd[8]`：单个值

**含义**：
- `array_part[0]=260`：循环 i 的分块大小
- `array_part[1]=256`：循环 j 的分块大小
- `array_part[2]=512`：循环 k 的分块大小（通常不用于空间循环）
- `latency[0]=20`：循环 i 的延迟隐藏因子
- `latency[1]=16`：循环 j 的延迟隐藏因子

### 4.2 CNN 示例

```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}"
```

**参数解析**：
- `array_part[8,8,4,8]`：四个维度（对应循环 o, i, r, c）
- `latency[4,2,4]`：三个维度（对应并行循环）
- `simd[1,1,1,2]`：四个维度（每个循环的 SIMD 因子）

---

## 5. 当前实现的局限性总结

### 5.1 Spacetime 支持

| Spacetime | 状态 | 优先级 |
|-----------|------|--------|
| **0** [i] | ❌ 不支持 | 中 |
| **1** [j] | ❌ 不支持 | 中 |
| **2** [k] | ❌ 不支持 | 低（需要 reduction） |
| **3** [i,j] | ✅ 支持 | - |
| **4** [i,k] | ❌ 不支持 | 中（需要 reduction） |
| **5** [j,k] | ❌ 不支持 | 中（需要 reduction） |

### 5.2 多维参数支持

| 参数 | 当前实现 | AutoSA 支持 | 优先级 |
|------|----------|-------------|--------|
| **array_part** | 单个值 | 多维数组 | 高 |
| **latency** | 单个值 | 多维数组 | 高 |
| **simd** | 单个值 | 单个值或多维数组 | 中 |

### 5.3 功能支持

| 功能 | 当前实现 | AutoSA 支持 | 优先级 |
|------|----------|-------------|--------|
| **Reduction** | ❌ 不支持 | ✅ 支持 | 中 |
| **SIMD on space loops** | ❌ 不支持 | ✅ 支持 | 中 |
| **Array contraction** | ❌ 不支持 | ✅ 支持 | 低 |

---

## 6. 实现建议

### 6.1 短期目标（高优先级）

1. **支持多维 array_part**：
   - 修改 `SystolicTransform` 接受数组参数
   - 修改 `SystolicDataflowGeneration` 使用不同维度的值
   - 修改 `SystolicHLSEmitter` 生成正确的代码

2. **支持多维 latency**：
   - 修改 `SystolicTransform` 接受数组参数
   - 修改 PE 生成逻辑使用不同维度的值

3. **支持 spacetime=0, 1**：
   - 实现 1D PE 阵列生成
   - 实现水平/垂直数据重用

### 6.2 中期目标（中优先级）

1. **支持 spacetime=4, 5**：
   - 实现不同的空间循环组合
   - 实现 reduction 支持

2. **支持多维 simd**：
   - 修改 SIMD 向量化逻辑
   - 支持每个循环不同的 SIMD 因子

### 6.3 长期目标（低优先级）

1. **支持 spacetime=2**：
   - 实现 reduction 支持
   - 实现 SIMD on space loops
   - 实现数组收缩优化

2. **支持更多优化**：
   - Array contraction
   - 其他 AutoSA 优化技术

---

## 7. 相关文档

- **AutoSA 文档**：`/home/user/work/AutoSA/docs/`
- **理论背景**：`/home/user/work/AutoSA/docs/tutorials/theory_background.rst`
- **数组优化**：`/home/user/work/AutoSA/docs/tutorials/optimize_array.rst`
- **矩阵乘法示例**：`/home/user/work/AutoSA/docs/examples/mm.rst`
- **CNN 示例**：`/home/user/work/AutoSA/docs/examples/cnn.rst`

---

## 8. 下一步行动

1. ✅ **分析完成**：理解 spacetime 参数和多维参数的含义
2. ⚠️ **实现多维参数支持**：array_part, latency, simd
3. ⚠️ **实现其他 spacetime 值**：0, 1, 2, 4, 5
4. ⚠️ **实现 reduction 支持**：用于 spacetime=2, 4, 5
5. ⚠️ **测试和验证**：确保新功能正确工作


