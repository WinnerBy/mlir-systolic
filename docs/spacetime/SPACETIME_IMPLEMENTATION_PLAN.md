# Space-time 扩展实现方案

> **最后更新**: 2024-12-02  
> **目的**: 基于 AutoSA 官方文档和参考代码，制定不同 Space-time 的实现方案

---

## 参考资源

- **AutoSA 官方文档**: [Matrix Multiplication Example](https://autosa.readthedocs.io/en/latest/examples/mm.html)
- **参考代码**: `test/reference-samples/`
- **理论文档**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`

---

## 不同 Space-time 的架构特点

### Space-time=0: [i] - 1D Output-Stationary

**架构**（参考 [官方文档](https://autosa.readthedocs.io/en/latest/examples/mm.html#array-1-i)）:
- 1D 脉动阵列（1×N）
- C 在 PE 内计算（output-stationary）
- B 在 PE 间水平传递（reuse）
- A 直接发送到每个 PE（direct）

**实现要点**:
1. **PE_wrapper 签名**:
   ```cpp
   void PE_wrapper(int idx, 
                   hls::stream<A_t> &fifo_A_in,      // A: direct
                   hls::stream<B_t> &fifo_B_in,      // B: 从左侧 PE 或 feeder
                   hls::stream<B_t> &fifo_B_out,    // B: 转发到右侧 PE
                   hls::stream<float> &fifo_C_drain_out);  // C: drain
   ```

2. **IO 模块**:
   - A: L2_in, L1_in（简化，无 L3）
   - B: L2_in, L1_in（B 在 PE 间传递）
   - C: L1_out drain（收集 C 结果）

3. **数据流方向**:
   - A: Direct（每个 PE 独立接收）
   - B: Horizontal（从左到右传递）
   - C: Local（本地累加，通过 drain 收集）

---

### Space-time=1: [j] - 1D Output-Stationary

**架构**（参考 [官方文档](https://autosa.readthedocs.io/en/latest/examples/mm.html#array-2-j)）:
- 1D 脉动阵列（1×N）
- C 在 PE 内计算（output-stationary）
- A 在 PE 间垂直传递（reuse）
- B 直接发送到每个 PE（direct）

**实现要点**:
1. **PE_wrapper 签名**:
   ```cpp
   void PE_wrapper(int idx, 
                   hls::stream<A_t> &fifo_A_in,      // A: 从上方 PE 或 feeder
                   hls::stream<A_t> &fifo_A_out,    // A: 转发到下方 PE
                   hls::stream<B_t> &fifo_B_in,      // B: direct
                   hls::stream<float> &fifo_C_drain_out);  // C: drain
   ```

2. **IO 模块**:
   - A: L2_in, L1_in（A 在 PE 间传递）
   - B: L2_in, L1_in（简化，无 L3）
   - C: L1_out drain

3. **数据流方向**:
   - A: Vertical（从上到下传递）
   - B: Direct（每个 PE 独立接收）
   - C: Local（本地累加）

---

### Space-time=2: [k] - 1D Input-Stationary

**架构**（参考 [官方文档](https://autosa.readthedocs.io/en/latest/examples/mm.html#array-3-k)）:
- 1D 脉动阵列（1×N）
- C 沿 PE 累加（input-stationary, reduction）
- A 和 B 直接发送到每个 PE（broadcast）

**实现要点**:
1. **PE_wrapper 签名**:
   ```cpp
   void PE_wrapper(int idx, 
                   hls::stream<A_t> &fifo_A_in,      // A: broadcast
                   hls::stream<B_t> &fifo_B_in,      // B: broadcast
                   hls::stream<float> &fifo_C_in,    // C: 从上游 PE 输入
                   hls::stream<float> &fifo_C_out);  // C: 向下游 PE 输出（累加）
   ```

2. **IO 模块**:
   - A: L2_in, L1_in
   - B: L1_in（简化）
   - C: L2_out（不是 drain，是累加输出）

3. **数据流方向**:
   - A: Broadcast（广播到所有 PE）
   - B: Broadcast（广播到所有 PE）
   - C: Reduce Horizontal（在 PE 间累加传递）

4. **Reduction 实现**:
   - MLIR 自动检测 reduction 操作
   - 在 PE 内：`local_C = fifo_C_in.read() + A * B`
   - 在 PE 间：`fifo_C_out.write(local_C)`

---

### Space-time=3: [i,j] - 2D Output-Stationary ✅ **当前支持**

**架构**（参考 [官方文档](https://autosa.readthedocs.io/en/latest/examples/mm.html#array-4-i-j)）:
- 2D 脉动阵列（M×N）
- C 在 PE 内计算（output-stationary）
- A 水平重用（沿 j 方向）
- B 垂直重用（沿 i 方向）

**实现要点**:
1. **PE_wrapper 签名**:
   ```cpp
   void PE_wrapper(int idx, int idy, 
                   hls::stream<A_t> &fifo_A_in, 
                   hls::stream<A_t> &fifo_A_out,    // A: 水平传递
                   hls::stream<B_t> &fifo_B_in, 
                   hls::stream<B_t> &fifo_B_out,    // B: 垂直传递
                   hls::stream<float> &fifo_C_drain_out);
   ```

2. **IO 模块**:
   - A: L3_in, L2_in, L1_in（完整三级）
   - B: L3_in, L2_in, L1_in（完整三级）
   - C: L1_out drain

---

### Space-time=4: [i,k] - 2D with H-Reduce

**架构**（参考 [官方文档](https://autosa.readthedocs.io/en/latest/examples/mm.html#array-5-i-k)）:
- 2D 脉动阵列（M×N）
- C 水平累加（reduction，沿 k 方向）
- B 垂直重用（沿 i 方向）
- A 直接发送到每个 PE（direct）

**实现要点**:
1. **PE_wrapper 签名**:
   ```cpp
   void PE_wrapper(int idx, int idy, 
                   hls::stream<A_t> &fifo_A_in,      // A: direct
                   hls::stream<B_t> &fifo_B_in,      // B: 从上方 PE
                   hls::stream<B_t> &fifo_B_out,    // B: 转发到下方 PE
                   hls::stream<float> &fifo_C_in,    // C: 从左侧 PE
                   hls::stream<float> &fifo_C_out);  // C: 转发到右侧 PE（累加）
   ```

2. **IO 模块**:
   - A: L2_in, L1_in
   - B: L3_in, L2_in, L1_in（B 垂直传递）
   - C: L2_out（C 水平累加）

3. **数据流方向**:
   - A: Direct
   - B: Vertical
   - C: Reduce Horizontal

---

### Space-time=5: [j,k] - 2D with V-Reduce

**架构**（参考 [官方文档](https://autosa.readthedocs.io/en/latest/examples/mm.html#array-6-j-k)）:
- 2D 脉动阵列（M×N）
- C 水平累加（reduction，沿 k 方向）
- A 垂直重用（沿 j 方向）
- B 直接发送到每个 PE（direct）

**实现要点**:
1. **PE_wrapper 签名**:
   ```cpp
   void PE_wrapper(int idx, int idy, 
                   hls::stream<A_t> &fifo_A_in,      // A: 从上方 PE
                   hls::stream<A_t> &fifo_A_out,    // A: 转发到下方 PE
                   hls::stream<B_t> &fifo_B_in,      // B: direct
                   hls::stream<float> &fifo_C_in,    // C: 从左侧 PE
                   hls::stream<float> &fifo_C_out);  // C: 转发到右侧 PE（累加）
   ```

2. **IO 模块**:
   - A: L3_in, L2_in, L1_in（A 垂直传递）
   - B: L2_in, L1_in
   - C: L2_out（C 水平累加）

3. **数据流方向**:
   - A: Vertical
   - B: Direct
   - C: Reduce Horizontal

---

## MLIR 自动 Reduction 检测策略

### 优势

**MLIR 可以自动检测 reduction**，不需要显式指定 `--local-reduce` 参数：

1. **依赖分析**:
   ```mlir
   // MLIR 可以自动识别这种模式
   affine.for %k = 0 to K {
     %c = affine.load %C[%i, %j]
     %prod = arith.mulf %a, %b
     %sum = arith.addf %c, %prod  // ← 自动检测为 reduction
     affine.store %sum, %C[%i, %j]
   }
   ```

2. **循环分析**:
   - 检测累加模式（load → add → store）
   - 识别 reduction 维度（通常是内层循环）
   - 确定 reduction 操作符（`+`, `*`, `max`, `min` 等）

3. **自动应用**:
   - 当检测到 reduction 时，自动生成 reduction 数据流
   - 在 PE 间传递累加结果
   - 生成 `fifo_C_in` 和 `fifo_C_out` 参数

### 实现步骤

1. **在 `SystolicTransform` 中检测 Reduction**:
   ```cpp
   // 检测 reduction 循环
   bool isReductionLoop(AffineForOp loop) {
     // 检查循环内是否有累加操作
     // 检查依赖关系
     // 返回 true 如果检测到 reduction
   }
   ```

2. **在 `SystolicDataflowGeneration` 中应用 Reduction**:
   ```cpp
   // 如果检测到 reduction，生成 reduction 数据流
   if (isReductionLoop(timeLoop)) {
     // 生成 fifo_C_in 和 fifo_C_out
     // 在 PE 内累加：local_C = fifo_C_in.read() + A * B
     // 在 PE 间传递：fifo_C_out.write(local_C)
   }
   ```

3. **在 `EmitHLSCpp` 中生成代码**:
   ```cpp
   // 根据是否 reduction 生成不同的 PE_wrapper 签名
   if (hasReduction) {
     // 生成带 fifo_C_in 和 fifo_C_out 的签名
   } else {
     // 生成带 fifo_C_drain_out 的签名
   }
   ```

---

## 实现方案

### 阶段 1: 优化当前实现（Space-time=3）

**目标**: 优化 PIPELINE 数量和代码行数

**工作项**:
1. 分析参考代码的 PIPELINE 插入位置
2. 调整流水线插入策略
3. 优化代码生成，减少冗余代码

---

### 阶段 2: 实现 1D 脉动阵列（Space-time=0, 1）

**目标**: 支持 1D output-stationary 脉动阵列

**工作项**:
1. **修改 PE_wrapper 生成**:
   - 根据 spacetime 参数生成不同的函数签名
   - Space-time=0: B 在 PE 间传递
   - Space-time=1: A 在 PE 间传递

2. **修改 IO 模块生成**:
   - 简化 IO 层级（L2, L1，无 L3）
   - 根据数据流方向生成不同的 IO 模块

3. **修改数据流生成**:
   - Direct: 每个 PE 独立接收
   - Horizontal: 从左到右传递
   - Vertical: 从上到下传递

**预期结果**:
- Space-time=0, 1 的代码生成正确
- PIPELINE 数量与参考代码一致（17 个）
- 代码行数接近参考代码

---

### 阶段 3: 实现 Reduction 支持（Space-time=2）

**目标**: 支持 1D input-stationary 脉动阵列（reduction）

**工作项**:
1. **实现 Reduction 检测**:
   - 在 `SystolicTransform` 中检测 reduction 循环
   - 识别累加模式
   - 确定 reduction 操作符

2. **实现 Reduction 数据流**:
   - 生成 `fifo_C_in` 和 `fifo_C_out` 参数
   - 在 PE 内累加：`local_C = fifo_C_in.read() + A * B`
   - 在 PE 间传递：`fifo_C_out.write(local_C)`

3. **修改 IO 模块**:
   - C 使用 L2_out（不是 drain）
   - A 和 B 使用 broadcast 模式

**预期结果**:
- Space-time=2 的代码生成正确
- Reduction 自动检测和应用
- PIPELINE 数量与参考代码一致（18 个）

---

### 阶段 4: 实现 2D Reduction（Space-time=4, 5）

**目标**: 支持 2D 脉动阵列（带 reduction）

**工作项**:
1. **扩展 Reduction 支持**:
   - 支持 2D 阵列的 reduction
   - C 在水平方向累加（沿 k 方向）

2. **修改数据流**:
   - Space-time=4: B 垂直传递，C 水平累加
   - Space-time=5: A 垂直传递，C 水平累加

3. **修改 PE_wrapper**:
   - 2D 索引（idx, idy）
   - 根据 spacetime 生成不同的数据流参数

**预期结果**:
- Space-time=4, 5 的代码生成正确
- PIPELINE 数量与参考代码一致（25 个）

---

## 关键实现细节

### 1. PE_wrapper 签名生成规则

```cpp
// 根据 spacetime 和数据流方向生成不同的签名
switch (spacetime) {
  case 0:  // [i]
    // A: direct, B: horizontal, C: local
    generatePEWrapper(hasAIn, hasBIn, hasBOut, hasCDrain);
    break;
  case 1:  // [j]
    // A: vertical, B: direct, C: local
    generatePEWrapper(hasAIn, hasAOut, hasBIn, hasCDrain);
    break;
  case 2:  // [k] - reduction
    // A: broadcast, B: broadcast, C: reduce
    generatePEWrapper(hasAIn, hasBIn, hasCIn, hasCOut);
    break;
  case 3:  // [i,j]
    // A: horizontal, B: vertical, C: local
    generatePEWrapper(hasAIn, hasAOut, hasBIn, hasBOut, hasCDrain);
    break;
  case 4:  // [i,k] - reduction
    // A: direct, B: vertical, C: reduce horizontal
    generatePEWrapper(hasAIn, hasBIn, hasBOut, hasCIn, hasCOut);
    break;
  case 5:  // [j,k] - reduction
    // A: vertical, B: direct, C: reduce horizontal
    generatePEWrapper(hasAIn, hasAOut, hasBIn, hasCIn, hasCOut);
    break;
}
```

### 2. 数据流方向实现

**Horizontal（水平流动）**:
```cpp
// 在 PE_wrapper 中
if (pe_j == 0) {
  data = fifo_A_feeder.read();  // 边界 PE 从 feeder 读取
} else {
  data = fifo_A_in.read();      // 内部 PE 从左侧 PE 读取
}
// 转发到右侧 PE
fifo_A_out.write(data);
```

**Vertical（垂直流动）**:
```cpp
// 在 PE_wrapper 中
if (pe_i == 0) {
  data = fifo_B_feeder.read();  // 边界 PE 从 feeder 读取
} else {
  data = fifo_B_in.read();       // 内部 PE 从上方 PE 读取
}
// 转发到下方 PE
fifo_B_out.write(data);
```

**Direct（直接发送）**:
```cpp
// 每个 PE 独立接收，不在 PE 间传递
data = fifo_A_feeder.read();  // 直接从 feeder 读取
// 没有 fifo_A_out
```

**Broadcast（广播）**:
```cpp
// 所有 PE 接收相同数据，不在 PE 间传递
data = fifo_A_feeder.read();  // 直接从 feeder 读取
// 没有 fifo_A_out
```

**Reduce（归约）**:
```cpp
// 在 PE_wrapper 中
float c_in = (pe_k == 0) ? 0.0f : fifo_C_in.read();  // 第一个 PE 初始化为 0
float c_local = c_in + a * b;                        // 累加
fifo_C_out.write(c_local);                          // 转发到下一个 PE
```

### 3. IO 模块层级选择

**规则**:
- **2D 阵列（spacetime=3,4,5）**: 使用完整的 L3/L2/L1 三级结构
- **1D 阵列（spacetime=0,1,2）**: 使用简化的 L2/L1 两级结构

**实现**:
```cpp
int getIOLevels(int spacetime) {
  if (spacetime == 3 || spacetime == 4 || spacetime == 5) {
    return 3;  // L3, L2, L1
  } else {
    return 2;  // L2, L1
  }
}
```

---

## 测试计划

### 测试配置

| Space-time | 配置 | 预期 PIPELINE | 预期代码行数 |
|-----------|------|--------------|-------------|
| 0 | `I32_J32_K32_ap32_lat8_simd2` | 17 | ~1082 |
| 1 | `I32_J32_K32_ap32_lat8_simd2` | 17 | ~1082 |
| 2 | `I32_J32_K32_ap4_lat2_simd2` | 18 | ~1105 |
| 3 | `I32_J32_K32_ap8_lat4_simd1` | 24 | ~1499 |
| 4 | `I32_J32_K32_ap32_lat16_simd2` | 25 | ~1528 |
| 5 | `I32_J32_K32_ap32_lat16_simd2` | 25 | ~1520 |

### 验证项

1. **功能正确性**:
   - PE_wrapper 函数签名正确
   - IO 模块结构正确
   - 数据流方向正确

2. **代码质量**:
   - PIPELINE 数量与参考代码一致
   - 代码行数接近参考代码
   - 核心结构完整

3. **Reduction 支持**:
   - 自动检测 reduction
   - 正确生成 reduction 数据流
   - 累加结果正确

---

## 相关文档

- **AutoSA 官方文档**: https://autosa.readthedocs.io/en/latest/examples/mm.html
- **Spacetime 分析**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`
- **实现分析**: `docs/SPACETIME_IMPLEMENTATION_ANALYSIS.md`
- **测试结果**: `docs/SPACETIME_TEST_ANALYSIS.md`

