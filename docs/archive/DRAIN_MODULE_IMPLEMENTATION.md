# Drain 模块实现总结

> **最后更新**: 2024-12  
> **状态**: 完成 ✅

---

## 实现成果

### 代码质量对比

| 指标 | 实现前 | 实现后 | AutoSA | 改进 |
|------|--------|--------|--------|------|
| **代码行数** | 1055 | 1565 | 1499 | +510 (+48%) |
| **PIPELINE pragma** | 15 | 24 | 24 | +9 (+60%) |
| **函数数量** | 36 | 55 | 43 | +19 (+53%) |

### 主要实现

#### 1. C_drain_IO_L1_out 模块 ✅

**C_drain_IO_L1_out_intra_trans**：
- **功能**：从 PE 收集 float 数据，打包成 C_t4
- **关键特性**：
  - 从 `fifo_C_drain_local_in` 读取 float 数据
  - 读取 `local_C[c7][c6 / 4]` 的当前值
  - 将 C_t4 解包到 `data_split` 数组
  - 使用 union 将 float 转换为 unsigned int
  - 更新 `data_split[split_idx]`
  - 重新打包并写回 `local_C`
  - 添加 PIPELINE 和 UNROLL pragma

**C_drain_IO_L1_out_inter_trans**：
- **功能**：转发 C_t4 数据
- **关键特性**：
  - 根据模块 ID（idy）决定是存储还是转发
  - 如果 `c4 == p1`：从 `local_C` 读取并写入 FIFO
  - 否则：从 FIFO 读取并转发到下一个模块
  - 添加 PIPELINE pragma

**C_drain_IO_L1_out_inter_trans_boundary**：
- **功能**：边界处理
- **关键特性**：
  - 只处理当前模块的数据（不转发）
  - 从 `local_C` 读取并写入 FIFO

**C_drain_IO_L1_out**：
- **功能**：主函数，协调 intra_trans 和 inter_trans
- **关键特性**：
  - 声明 `local_C[4][1]` 缓冲区
  - 添加 RESOURCE pragma（RAM_2P_BRAM）
  - 在循环中调用 intra_trans 和 inter_trans

**C_drain_IO_L1_out_wrapper 和 boundary_wrapper**：
- **功能**：包装器函数
- **关键特性**：
  - 简化函数调用接口

#### 2. C_drain_IO_L2_out 模块 ✅

**C_drain_IO_L2_out**：
- **功能**：L2 级数据收集和转发
- **关键特性**：
  - 从 L1 模块收集数据
  - 根据模块 ID（idx）决定是存储还是转发
  - 添加 PIPELINE pragma

**C_drain_IO_L2_out_boundary**：
- **功能**：边界处理
- **关键特性**：
  - 只处理当前模块的数据

#### 3. C_drain_IO_L3_out 模块 ✅

**C_drain_IO_L3_out**：
- **功能**：L3 级数据收集
- **关键特性**：
  - 从 L2 模块收集数据
  - 添加 PIPELINE pragma

**C_drain_IO_L3_out_serialize**：
- **功能**：序列化到内存（C_t4 → C_t16）
- **关键特性**：
  - 从 FIFO 读取 C_t4 数据
  - 打包成 C_t16（4 个 C_t4）
  - 写入内存数组
  - 添加 PIPELINE 和 ARRAY_PARTITION pragma

---

## 实现细节

### 数据打包逻辑（intra_trans）

```cpp
// 1. 读取当前值
out_data = local_C[c7][c6 / 4];

// 2. 解包到 data_split
for (ap_uint<3> n = 0; n < 4; n++) {
  #pragma HLS UNROLL
  data_split[n] = out_data(31, 0);
  out_data = out_data >> 32;
}

// 3. 更新 split_idx 位置
union {unsigned int ui; float ut;} u;
u.ut = in_data;  // in_data 是 float
data_split[split_idx] = ap_uint<32>(u.ui);

// 4. 重新打包
out_data = (data_split[3], data_split[2], data_split[1], data_split[0]);
local_C[c7][c6 / 4] = out_data;
```

### 数据序列化逻辑（L3_out_serialize）

```cpp
// 1. 读取 4 个 C_t4 数据
for (ap_uint<3> p = 0; p < 4; p++) {
  fifo_data = fifo_C_drain_local_in.read();
  mem_data_split[p] = fifo_data;
}

// 2. 打包成 C_t16
mem_data = (mem_data_split[3], mem_data_split[2], mem_data_split[1], mem_data_split[0]);

// 3. 写入内存
C[i] = mem_data;
```

### FIFO 声明

```cpp
// L1 FIFOs: numPE x (numPE + 1)
for (unsigned i = 0; i < numPE; i++) {
  for (unsigned j = 0; j <= numPE; j++) {
    hls::stream<C_t4> fifo_C_drain_C_drain_IO_L1_out_i_j;
    #pragma HLS STREAM variable=... depth=2
    #pragma HLS RESOURCE variable=... core=FIFO_SRL
  }
}

// L2 FIFOs: (numPE + 1)
for (unsigned i = 0; i <= numPE; i++) {
  hls::stream<C_t4> fifo_C_drain_C_drain_IO_L2_out_i;
  ...
}
```

### 模块调用顺序

1. **L1 模块**（按列，从右到左）：
   - 对于每列 j（从 numPE-1 到 1）：
     - 如果是右most列（j == numPE-1）：调用 boundary_wrapper
     - 调用 regular wrapper（j-1）

2. **L2 模块**：
   - 先调用 boundary（最右）
   - 然后调用 regular（从右到左）

3. **L3 模块**：
   - C_drain_IO_L3_out
   - C_drain_IO_L3_out_serialize

---

## 测试结果

### 配置：I32_J32_K32_ap8_lat4_simd1

**对比结果**：
- ✅ kernel0 函数存在
- ✅ PE 函数存在
- ✅ IO 模块存在
- ✅ Drain 模块存在（L1, L2, L3）
- ✅ PIPELINE pragma：24（AutoSA：24，完全一致！）
- ✅ 数据类型定义存在

**统计信息**：
- 代码行数：1565（AutoSA：1499，差距 +4%）
- PIPELINE pragma：24（AutoSA：24，完全一致！）
- 函数数量：55（AutoSA：43）

---

## 关键发现

### 1. 数据打包逻辑

**关键点**：
- 使用 `c6 / 4` 来确定数组索引（因为 C_t4 包含 4 个 float）
- 使用 `c6 % 4` 来确定 split_idx（在 C_t4 中的位置）
- 使用 union 进行类型转换（float ↔ unsigned int）

### 2. 模块调用顺序

**关键点**：
- L1 模块按列处理（从右到左）
- 每列先处理 boundary，再处理 regular
- L2 和 L3 模块按顺序处理

### 3. FIFO 命名

**关键点**：
- L1 FIFO：`fifo_C_drain_C_drain_IO_L1_out_{i}_{j}`
- L2 FIFO：`fifo_C_drain_C_drain_IO_L2_out_{i}`
- L3 FIFO：`fifo_C_drain_C_drain_IO_L3_out_serialize`

---

## 相关文件

- **代码生成器**：`tools/systolic-translate/systolic-translate.cpp`
- **参考代码**：`test/reference-samples/mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp`
- **测试脚本**：`test/test_against_autosa.sh`

---

## 下一步工作

### 已完成 ✅

1. ✅ 实现 C_drain_IO_L1_out 模块（intra_trans, inter_trans, boundary）
2. ✅ 实现 C_drain_IO_L2_out 模块
3. ✅ 实现 C_drain_IO_L3_out 模块
4. ✅ 实现数据打包和序列化逻辑
5. ✅ 添加 FIFO 声明和模块调用

### 待优化 ⚠️

1. ⚠️ 代码行数：1565 vs 1499（+4%）
   - 可能需要优化某些循环结构
   - 可能需要合并某些函数

2. ⚠️ 函数数量：55 vs 43（+28%）
   - 可能需要合并某些 wrapper 函数

3. ⚠️ 代码细节：
   - 某些注释的位置
   - 某些变量的初始化
   - 某些循环的边界条件

---

## 总结

通过实现完整的 Drain 模块，我们已经：

1. ✅ **实现了完整的数据流**：从 PE → L1 → L2 → L3 → 内存
2. ✅ **添加了所有必要的 PIPELINE pragma**：24 个（与 AutoSA 完全一致）
3. ✅ **实现了数据打包和序列化**：float → C_t4 → C_t16 → 内存
4. ✅ **实现了完整的模块调用**：按正确的顺序调用所有模块

**当前状态**：
- 代码行数：1565（AutoSA：1499，差距 +4%）
- PIPELINE pragma：24（AutoSA：24，完全一致！）
- 所有关键结构都已生成

**下一步**：
- 优化代码细节，减少代码行数
- 优化函数数量
- 确保代码结构与 AutoSA 完全一致

