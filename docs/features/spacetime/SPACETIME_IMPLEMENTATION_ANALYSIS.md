# Space-time 实现方案分析

> **最后更新**: 2024-12-02  
> **目的**: 基于文档和参考代码分析，制定不同 Space-time 的实现方案

---

## 文档分析总结

### 文档中包含的信息

✅ **理论层面**:
- 不同 spacetime 的数据流方向（Horizontal/Vertical/Direct/Broadcast）
- 不同 spacetime 的空间循环和时间循环映射
- IO 模块生成的结构（L1/L2/L3）
- Reduction 需要的参数（--local-reduce, --reduce-op等）

✅ **架构层面**:
- AutoSA 的整体流程和数据结构
- 空间-时间变换的算法流程
- 多级分块的实现方式

### 文档中缺少的信息

❌ **实现细节**:
- 如何具体生成不同 spacetime 的 IO 模块代码
- 如何实现数据流方向的具体逻辑（Horizontal/Vertical/Direct/Broadcast）
- Reduction 的具体实现细节（如何在 PE 间传递累加结果）
- 1D vs 2D PE 阵列的代码生成差异
- PE_wrapper 函数签名的差异
- IO 模块的循环结构和数据访问模式

---

## 参考代码分析

### Space-time=0 (1D [i])

**IO 模块结构**:
- `A_IO_L2_in` - L2 级别 IO（简化，无 L3）
- `A_IO_L1_in` - L1 级别 IO（包含 inter_trans 和 intra_trans）
- `B_IO_L2_in` - B 矩阵的 L2 IO
- `C_drain_IO_L1_out` - C 矩阵的 drain 模块（L1 级别）

**PE_wrapper 签名**:
```cpp
void PE_wrapper(int idx, 
                hls::stream<A_t2> &fifo_A_in, 
                hls::stream<B_t2> &fifo_B_in, 
                hls::stream<B_t2> &fifo_B_out,  // B 在 PE 间传递
                hls::stream<float> &fifo_C_drain_out);
```

**数据流**:
- A: Direct（直接发送到每个 PE）
- B: Horizontal（水平流动，在 PE 间传递）
- C: Local（本地累加，通过 drain 收集）

**特点**:
- 1D PE 阵列（只有 idx 维度）
- B 矩阵在 PE 间水平传递
- C 通过 drain 模块收集

---

### Space-time=2 (1D [k], reduction)

**IO 模块结构**:
- `A_IO_L2_in` - A 矩阵的 L2 IO
- `A_IO_L1_in` - A 矩阵的 L1 IO
- `B_IO_L1_in` - B 矩阵的 L1 IO
- `C_IO_L2_out` - C 矩阵的 L2 输出（不是 drain）

**PE_wrapper 签名**:
```cpp
void PE_wrapper(int idx, 
                hls::stream<A_t2> &fifo_A_in, 
                hls::stream<B_t2> &fifo_B_in, 
                hls::stream<float> &fifo_C_in,   // C 从上游 PE 输入
                hls::stream<float> &fifo_C_out);  // C 向下游 PE 输出
```

**数据流**:
- A: Broadcast（广播到所有 PE）
- B: Broadcast（广播到所有 PE）
- C: Reduce Horizontal（C 在 PE 间累加传递）

**特点**:
- 1D PE 阵列（只有 idx 维度）
- C 矩阵在 PE 间累加传递（reduction）
- 需要 `--local-reduce` 和 `--reduce-op="+"` 参数

---

### Space-time=3 (2D [i,j])

**IO 模块结构**:
- `A_IO_L3_in` - A 矩阵的 L3 IO
- `A_IO_L2_in` - A 矩阵的 L2 IO（双缓冲）
- `A_IO_L1_in` - A 矩阵的 L1 IO
- `B_IO_L3_in` - B 矩阵的 L3 IO
- `B_IO_L2_in` - B 矩阵的 L2 IO（双缓冲）
- `B_IO_L1_in` - B 矩阵的 L1 IO
- `C_drain_IO_L1_out` - C 矩阵的 drain 模块

**PE_wrapper 签名**:
```cpp
void PE_wrapper(int idx, 
                hls::stream<A_t2> &fifo_A_in, 
                hls::stream<A_t2> &fifo_A_out,    // A 水平传递
                hls::stream<B_t2> &fifo_B_in, 
                hls::stream<B_t2> &fifo_B_out,    // B 垂直传递
                hls::stream<float> &fifo_C_drain_out);
```

**数据流**:
- A: Horizontal（水平流动，沿 j 方向）
- B: Vertical（垂直流动，沿 i 方向）
- C: Local（本地累加）

**特点**:
- 2D PE 阵列（idx 可能是一维索引或二维索引）
- A 和 B 都在 PE 间传递
- 完整的 L3/L2/L1 三级 IO 结构

---

## 关键实现差异

### 1. PE_wrapper 函数签名差异

| Space-time | A 流 | B 流 | C 流 | 特殊参数 |
|-----------|------|------|------|----------|
| **0** | `fifo_A_in` | `fifo_B_in`, `fifo_B_out` | `fifo_C_drain_out` | - |
| **1** | `fifo_A_in`, `fifo_A_out` | `fifo_B_in` | `fifo_C_drain_out` | - |
| **2** | `fifo_A_in` | `fifo_B_in` | `fifo_C_in`, `fifo_C_out` | Reduction |
| **3** | `fifo_A_in`, `fifo_A_out` | `fifo_B_in`, `fifo_B_out` | `fifo_C_drain_out` | - |
| **4** | `fifo_A_in` | `fifo_B_in`, `fifo_B_out` | `fifo_C_in`, `fifo_C_out` | Reduction |
| **5** | `fifo_A_in`, `fifo_A_out` | `fifo_B_in` | `fifo_C_in`, `fifo_C_out` | Reduction |

### 2. IO 模块层级差异

| Space-time | A IO | B IO | C IO |
|-----------|------|------|------|
| **0** | L2, L1 | L2, L1 | L1 drain |
| **1** | L2, L1 | L2, L1 | L1 drain |
| **2** | L2, L1 | L1 | L2 out |
| **3** | L3, L2, L1 | L3, L2, L1 | L1 drain |
| **4** | L2, L1 | L3, L2, L1 | L2 out |
| **5** | L3, L2, L1 | L2, L1 | L2 out |

### 3. 数据流方向实现

**Horizontal（水平流动）**:
- 数据从左侧 PE 读取，转发到右侧 PE
- 边界 PE 从 feeder 读取
- 实现：`fifo_*_in` 和 `fifo_*_out` 参数

**Vertical（垂直流动）**:
- 数据从上方 PE 读取，转发到下方 PE
- 边界 PE 从 feeder 读取
- 实现：`fifo_*_in` 和 `fifo_*_out` 参数

**Direct（直接发送）**:
- 数据直接发送到每个 PE，不在 PE 间传递
- 实现：只有 `fifo_*_in`，没有 `fifo_*_out`

**Broadcast（广播）**:
- 数据广播到所有 PE，不在 PE 间传递
- 实现：只有 `fifo_*_in`，没有 `fifo_*_out`

**Reduce（归约）**:
- 数据在 PE 间累加传递
- 实现：`fifo_C_in` 和 `fifo_C_out`，在 PE 内累加

---

## AutoSA 官方文档分析

参考 [AutoSA 官方文档 - Matrix Multiplication](https://autosa.readthedocs.io/en/latest/examples/mm.html) 的 Dataflow Exploration 部分，不同 spacetime 的架构如下：

### Array 1: [i] - 1D Output-Stationary

**架构特点**（参考官方文档）:
- 1D 脉动阵列，使用循环 i 作为空间循环
- **Output-stationary**: C 在 PE 内计算
- **B 矩阵**: 在 PE 间重用（水平方向）
- **A 矩阵**: 直接发送到每个 PE

**命令示例**:
```bash
--sa-sizes="{kernel[]->space_time[0];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}"
```

**生成的阵列**: 1×4 1D array（array_part[0]=32, latency[0]=8 → 32/8=4）

---

### Array 2: [j] - 1D Output-Stationary

**架构特点**（参考官方文档）:
- 1D 脉动阵列，使用循环 j 作为空间循环
- **Output-stationary**: C 在 PE 内计算
- **A 矩阵**: 在 PE 间重用（垂直方向）
- **B 矩阵**: 直接发送到每个 PE

**命令示例**:
```bash
--sa-sizes="{kernel[]->space_time[1];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}"
```

**生成的阵列**: 1×4 1D array（array_part[1]=32, latency[1]=8 → 32/8=4）

---

### Array 3: [k] - 1D Input-Stationary

**架构特点**（参考官方文档）:
- 1D 脉动阵列，使用循环 k 作为空间循环
- **Input-stationary**: C 沿 PE 累加（reduction）
- **A 和 B 矩阵**: 直接发送到每个 PE（broadcast）

**命令示例**:
```bash
--sa-sizes="{kernel[]->space_time[2];kernel[]->array_part[4,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
--local-reduce --reduce-op="+" --simd-touch-space --array-contraction
```

**生成的阵列**: 1×2 1D array（array_part[2]=4, latency[0]=2 → 4/2=2）

**特殊参数**:
- `--local-reduce`: 启用本地归约
- `--reduce-op="+"`: 归约操作符为加法
- `--simd-touch-space`: SIMD 应用于空间循环
- `--array-contraction`: 数组收缩优化（减少 local_C 缓冲区大小）

---

### Array 4: [i,j] - 2D Output-Stationary ✅ **当前支持**

**架构特点**（参考官方文档）:
- 2D 脉动阵列，使用循环 i 和 j 作为空间循环
- **Output-stationary**: C 在 PE 内计算
- **A 矩阵**: 水平重用（沿 j 方向）
- **B 矩阵**: 垂直重用（沿 i 方向）

**命令示例**:
```bash
--sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}"
```

**生成的阵列**: 2×2 2D array（array_part[0]=16, latency[0]=8 → 16/8=2）

---

### Array 5: [i,k] - 2D with H-Reduce

**架构特点**（参考官方文档）:
- 2D 脉动阵列，使用循环 i 和 k 作为空间循环
- **C 矩阵**: 水平累加（reduction，沿 k 方向）
- **B 矩阵**: 垂直重用（沿 i 方向）
- **A 矩阵**: 直接发送到每个 PE

**命令示例**:
```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
--local-reduce --reduce-op="+" --simd-touch-space --array-contraction
```

**生成的阵列**: 2×2 2D array

---

### Array 6: [j,k] - 2D with V-Reduce

**架构特点**（参考官方文档）:
- 2D 脉动阵列，使用循环 j 和 k 作为空间循环
- **C 矩阵**: 水平累加（reduction，沿 k 方向）
- **A 矩阵**: 垂直重用（沿 j 方向）
- **B 矩阵**: 直接发送到每个 PE

**命令示例**:
```bash
--sa-sizes="{kernel[]->space_time[5];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
--local-reduce --reduce-op="+" --simd-touch-space --array-contraction
```

**生成的阵列**: 2×2 2D array

---

## MLIR 自动 Reduction 检测

### 当前实现方式

**用户提到**: 先前基于 ScaleHLS 实现时，让 MLIR 自动推测并 reduce，而不需要参数来指定。

**MLIR 的优势**:
- MLIR Affine Dialect 可以自动检测 reduction 操作
- 通过依赖分析可以识别累加模式
- 不需要显式指定 `--local-reduce` 参数

**实现策略**:
1. **自动检测 Reduction**:
   - 分析循环依赖关系
   - 识别累加模式（`C[i][j] += A[i][k] * B[k][j]`）
   - 确定 reduction 维度（通常是内层循环）

2. **自动应用 Reduction**:
   - 当检测到 reduction 时，自动生成 reduction 数据流
   - 在 PE 间传递累加结果
   - 生成 `fifo_C_in` 和 `fifo_C_out` 参数

3. **SIMD 自动选择**:
   - 自动选择 reduction 循环作为 SIMD 目标
   - 不需要 `--simd-touch-space` 参数

---

## 是否需要拉取 AutoSA 源代码？

### 文档分析结论（更新）

**文档提供的信息**:
- ✅ 理论框架和架构设计
- ✅ 数据流方向的概念
- ✅ 整体流程和算法
- ✅ **官方文档提供了架构图和详细描述**（新增）

**文档缺少的信息**:
- ❌ 具体的代码生成逻辑
- ❌ IO 模块的循环结构细节
- ❌ PE 数据流的具体实现
- ❌ Reduction 的代码生成细节

### 参考代码分析结论

**可以从参考代码中提取**:
- ✅ PE_wrapper 函数签名差异
- ✅ IO 模块的结构差异
- ✅ 数据流方向的实现方式（通过 fifo 参数）
- ✅ 基本的代码模式

**参考代码无法提供**:
- ❌ 如何从 MLIR 生成这些代码的逻辑
- ❌ 循环变换的具体规则
- ❌ 数据访问模式的推导过程
- ❌ 边界条件的处理逻辑

---

## 最终建议（更新）

**建议：先不拉取 AutoSA 源代码**

**理由**:
1. **文档已提供足够理论框架**: 
   - `AUTOSA_SPACETIME_ANALYSIS.md` 和 `AUTOSA_ANALYSIS.md` 提供了完整的理论框架
   - **官方文档提供了架构图和详细描述**（新增）
2. **参考代码提供了实现模式**: 可以从参考代码中提取 PE_wrapper 签名、IO 模块结构等关键信息
3. **MLIR 有自动检测能力**: 可以利用 MLIR 的自动 reduction 检测，不需要显式参数
4. **当前问题主要是优化**: 当前的主要问题是 PIPELINE 数量和代码行数差异，不是功能缺失
5. **可以逐步完善**: 可以先基于现有信息实现，遇到具体问题再查看源代码

**如果遇到以下情况，再考虑拉取源代码**:
- 无法理解参考代码的生成逻辑
- 需要实现复杂的数据流模式
- 遇到无法解决的实现问题
- 需要深入理解 AutoSA 的算法细节

---

## 下一步工作

### 基于文档和参考代码的实现方案

1. **分析参考代码模式**
   - 提取不同 spacetime 的 PE_wrapper 签名
   - 提取不同 spacetime 的 IO 模块结构
   - 分析数据流方向的实现方式

2. **实现代码生成逻辑**
   - 根据 spacetime 参数生成不同的 PE_wrapper 签名
   - 根据数据流方向生成不同的 IO 模块
   - 实现 Reduction 支持

3. **优化 PIPELINE 数量**
   - 分析参考代码的 PIPELINE 插入位置
   - 调整流水线插入策略
   - 针对不同 spacetime 实现不同的策略

4. **测试和验证**
   - 对比生成的代码与参考代码
   - 验证功能正确性
   - 优化代码行数差异

---

## 相关文档

- **Spacetime 分析**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`
- **AutoSA 分析**: `docs/AUTOSA_ANALYSIS.md`
- **测试结果**: `docs/SPACETIME_TEST_ANALYSIS.md`
- **参考代码**: `test/reference-samples/`

