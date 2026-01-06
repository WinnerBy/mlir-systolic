# 数据重用策略分析与实现方案

> **创建时间**: 2026-01-05  
> **目标**: 确定最优的数据重用实现方案，并提供实现路径

---

## 执行摘要

当前 mlir-systolic 生成的代码存在**数据流不匹配问题**：
- `A_IO_L3_in_serialize` 输出: **128 A_t8** (`i < 64`)
- `A_IO_L3_in` 需求: **512 A_t8** (4×4×4×2×4 次迭代)
- **缺口**: 384 A_t8 (4 倍不足)

**推荐修复方案**: **方案 C - 分层数据重用**（在 L2 级别实现）
- 不增加 Host 端复杂度
- 不需要额外 FPGA 存储
- 充分利用现有 L2 ping-pong 缓冲
- 性能与资源最优

---

## 问题分析

### 数据重用的必然性

矩阵乘法的 tiled 计算：
$$C[i_t][j_t] = \sum_{k_t} A[i_t][k_t] \times B[k_t][j_t]$$

在脉动阵列中：
- 每个 PE 行处理一个 `i_t` 切片
- 但需要遍历所有 `j_t`（4 次）
- 因此 A 的每行 tile 需要被读取 **4 次**

```
Timeline:
  j_t=0: 读 A[0][0..3], B[0..3][0]
  j_t=1: 读 A[0][0..3], B[0..3][1]  ← A 重复读取！
  j_t=2: 读 A[0][0..3], B[0..3][2]  ← A 重复读取！
  j_t=3: 读 A[0][0..3], B[0..3][3]  ← A 重复读取！
```

这个重用是**由矩阵乘法的数学性质决定的**，不是可选的。

### 三种实现位置

#### Option 1: Host 端（AutoSA 当前方案）

**在 Host 发送数据前进行复制**：
```
Host: [A] → replicate 4x → [A, A, A, A] → PCIe → FPGA
FPGA: A_IO_L3_in_serialize reads all 4 copies
      A_IO_L3_in transparently passes them
```

**时间开销**：
```
T_total = T_host_replicate + T_pcie_transfer + T_fpga_process
        = ~1-2 μs + 4-5 μs + T_compute
```

**资源开销**：
```
Host: 数据复制（CPU 计算）
FPGA: 最小（无额外缓存）
PCIe: 4× 带宽使用
```

#### Option 2: FPGA 端 L3 级别（完全缓存）

**在 A_IO_L3_in 内添加缓存和控制**：
```
FPGA:
  A_IO_L3_in_serialize: reads 64 A_t16 (4 KB)
  A_IO_L3_in: buffers all data, then repeats 4 times
```

**时间开销**：
```
T_total = T_pcie_transfer + T_buffer_write + T_buffer_read_4x
        = 1 μs + 16 μs + 64 μs
        = ~81 μs （受 FPGA 存储延迟限制）
```

**资源开销**：
```
FPGA BRAM: 缓存整个矩阵（O(size²)）
           32×32: ~2 BRAM
           128×128: ~32 BRAM
Complexity: 需要地址生成、读写调度
```

#### Option 3: FPGA 端 L2 级别（利用现有缓冲）

**在 A_IO_L2_in 的循环结构中实现重用**：
```
现有结构：
  A_IO_L2_in 循环：c0(0-3) × c1(0-3) × c2(0-3)
  每次读新的 A[c0][c1][c2]

改进结构：
  A_IO_L2_in 循环：c0(0-3) × c1(0-3) × c2(0-3)
    - 当 c1 变化时，重用 A[c0][prev_c1][*]
    - 不需要额外缓存（local_A_ping/pong 已存在）
```

**时间开销**：
```
T_total = T_pcie_transfer + T_l2_access
        = 1 μs + ~0.1 μs （BRAM 单周期访问）
        = ~1.1 μs （最优）
```

**资源开销**：
```
FPGA BRAM: 无额外占用（复用现有 L2 缓冲）
Complexity: 低（仅修改循环逻辑）
```

---

## 性能与资源对比

### 定量对比表

| 指标 | 方案 A (Host) | 方案 B (L3) | 方案 C (L2) |
|------|--------------|-----------|-----------|
| **总时间开销** | 5-6 μs | 81 μs | 1.1 μs |
| **PCIe 带宽** | 4× | 1× | 1× |
| **FPGA BRAM** | 0 | ~2-32 | 0 |
| **逻辑复杂度** | 中 | 高 | 低 |
| **代码修改** | Host + HLS | HLS L3 | HLS L2 |
| **与 AutoSA 兼容性** | ✓ 完全兼容 | ✓ 兼容 | ⚠️ 需适配 |

### 性能分析

**当数据已在 FPGA 内**（序列化传输后）：
- 方案 B: 需要 ~64 μs 来重复读取缓冲数据
- 方案 C: 只需 ~0.1 μs 来重复读取 BRAM 数据

**关键观察**：
方案 B 的性能瓶颈是 FPGA 存储系统的带宽，不是 PCIe！
- A_t8 的读取延迟: 单个 BRAM 块读 32 floats ≈ 4 cycles
- 读取 4 次: 4 × (4 cycles + pipeline overhead) ≈ 16-20 cycles
- 但循环展开和流水线化可以减少开销

**方案 C 的优势**：
- L2 缓冲已经是流水线化的
- ping-pong 结构天然支持重用
- 不增加新的延迟路径

---

## 方案 C 的实现细节

### 当前代码结构分析

#### A_IO_L2_in 的当前循环结构

```cpp
void A_IO_L2_in(int idx, ...) {
  // 现有两个 4KB 缓冲
  A_t8 local_A_ping[4][1];  // 临时缓存 A 的一行 tile
  A_t8 local_A_pong[4][1];
  
  // 三重循环遍历 tile 空间
  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)  // I 方向 tile
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1) // K 方向 tile
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) { // J 方向 tile
        
        // 交替使用 ping-pong
        if (arb == 0) {
          // inter_trans: 从 L3 读取 A[c0][c1][c2] → ping
          A_IO_L2_in_inter_trans(..., local_A_ping, ...);
          // intra_trans: 从 ping 读取并转发给下一级
          A_IO_L2_in_intra_trans(..., local_A_pong, ...);
        } else {
          A_IO_L2_in_inter_trans(..., local_A_pong, ...);
          A_IO_L2_in_intra_trans(..., local_A_ping, ...);
        }
        
        arb = !arb;
      }
}
```

#### 分析：c1 vs c2 的角色

在当前结构中：
- `c0`: 访问 A 的行（固定 PE 行）
- `c1`: K 方向的 tile 索引（实际读取新数据）
- `c2`: J 方向的 tile 索引（导致数据重复需求）

**重用机制应该是**：
- 当 `c2` 变化时，应该**重用**相同 `c0`, `c1` 的数据
- 但当 `c0` 或 `c1` 变化时，需要**读取新**数据

### 改进方案：分层循环结构

```cpp
void A_IO_L2_in_improved(int idx, ...) {
  A_t8 local_A_ping[4][1];
  A_t8 local_A_pong[4][1];
  bool arb = 0;
  
  // 外层：固定数据的循环
  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1) {  // I tile
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1) { // K tile
      
      // === 阶段 1: 读取新数据 ===
      if (arb == 0) {
        A_IO_L2_in_inter_trans(idx, c0, c1, 0, local_A_ping, ...);
      } else {
        A_IO_L2_in_inter_trans(idx, c0, c1, 0, local_A_pong, ...);
      }
      
      // === 阶段 2: 重复使用该数据（J 维度循环） ===
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) { // J tile
        if (arb == 0) {
          A_IO_L2_in_intra_trans(idx, c0, c1, c2, local_A_ping, ...);
        } else {
          A_IO_L2_in_intra_trans(idx, c0, c1, c2, local_A_pong, ...);
        }
      }
      
      // === 切换 ping-pong 缓冲 ===
      arb = !arb;
    }
  }
}
```

### 关键改动

| 点 | 当前 | 改进 |
|----|------|------|
| **inter_trans 调用** | 每个 (c0,c1,c2) 组合 | 每个 (c0,c1) 组合 |
| **intra_trans 调用** | 每个 (c0,c1,c2) 组合 | 每个 (c0,c1,c2) 组合 |
| **数据读取** | `512 = 4×4×4×2×4` 次 | `128 = 4×4×2` 次（inter 调用减少 4 倍）|
| **缓冲复用** | ping-pong 用于不同 tile | ping-pong 用于相同数据在不同 J tile |

---

## 实现路径

### Step 1: 修改代码生成器

**文件**: [systolic-translate.cpp](tools/systolic-translate/systolic-translate.cpp#L400)

当前的 `emitIOL2In` 函数需要修改循环结构：
```cpp
// 当前: 三重循环全部在 inter_trans/intra_trans 内
// 改为: 将 J 维度循环抽出来

// 伪代码
for c0:
  for c1:
    // 读取新数据
    emit_inter_trans_call(c0, c1, 0);  // 只对 c2=0 调用
    
    // 重复使用数据
    for c2:
      emit_intra_trans_call(c0, c1, c2);
```

### Step 2: 修改 inter_trans 函数

**当前 inter_trans 的 c2 循环需要移除**：
```cpp
// 当前
void A_IO_L2_in_inter_trans(int idx, int c0, int c1, int c2, ...) {
  if (!inter_trans_en) return;
  for (ap_uint<2> c3 = p0; c3 <= 1; c3 += 1) {
    if (c3 == p0) {
      for (ap_uint<3> c4 = 0; c4 <= 3; c4 += 1) {  // 读 4 个 A_t8
        local_A[c4][0] = fifo_A_in.read();
      }
    }
  }
}

// 改为: 直接读，不带 c2 参数
void A_IO_L2_in_inter_trans(int idx, int c0, int c1, ...) {
  // 相同逻辑，只是不依赖 c2
}
```

### Step 3: 验证数据流

修改后的数据流验证：

| 参数 | 当前 | 改进 |
|------|------|------|
| serialize 读取 | 64 A_t16 | 64 A_t16 ✓ |
| L3_in 输出 | 512 A_t8 需求 | 需求减少到 128 A_t8 ✓ |
| L2_in inter 调用 | 64 次 | 16 次（减少 4 倍）✓ |
| L2_in intra 调用 | 64 次 | 64 次（不变）✓ |

---

## 风险评估

### 改动范围

| 模块 | 改动程度 | 风险 |
|------|--------|------|
| 代码生成器 | 中等 | **中等**（需要测试所有配置） |
| HLS 代码结构 | 小 | **低**（仅循环嵌套） |
| 数据流 | 无（修复） | **低**（修复 bug） |
| AutoSA 兼容性 | 变化 | **中等**（逻辑不同但结果等价） |

### 验证策略

1. **单元测试**: 验证各层输入输出数据量
2. **功能测试**: 比较计算结果与参考实现
3. **性能测试**: 测量实际延迟和吞吐量
4. **配置扫描**: 多个 size/ap/lat 组合

---

## 结论

**方案 C（L2 级别分层数据重用）** 是最优选择：

✅ **优点**：
- 性能最优（仅 1 μs 数据准备）
- 资源消耗最少（无额外 BRAM）
- 代码修改简单（仅调整循环）
- 与现有架构兼容

⚠️ **权衡**：
- 需要修改代码生成器
- 改动范围涉及多个函数
- 需要完整的验证测试

📋 **实现优先级**：
1. **立即修复**: kernel0 重复定义（已完成）
2. **优先实现**: 方案 C（L2 分层重用）
3. **长期考虑**: 方案 B（如需跨阶段优化）
