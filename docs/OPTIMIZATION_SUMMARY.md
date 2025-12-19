# HLS 代码生成优化总结

> **最后更新**: 2024-12  
> **状态**: 显著改进，更接近 AutoSA 输出

---

## 优化成果

### 代码行数对比

| 版本 | 代码行数 | 改进 |
|------|----------|------|
| 优化前 | 447 行 | - |
| 优化后 | 1055 行 | +608 行 (+136%) |
| AutoSA | 1499 行 | 目标 |

### PIPELINE Pragma 对比

| 版本 | PIPELINE 数量 | 改进 |
|------|---------------|------|
| 优化前 | 7 个 | - |
| 优化后 | 15 个 | +8 个 (+114%) |
| AutoSA | 24 个 | 目标 |

### 函数数量对比

| 版本 | 函数数量 | 改进 |
|------|----------|------|
| 优化前 | 18 个 | - |
| 优化后 | 36 个 | +18 个 (+100%) |
| AutoSA | ~30 个 | 接近 |

---

## 主要优化内容

### 1. IO_L2_in 模块完整实现

#### A_IO_L2_in_intra_trans
- **功能**：数据解包，从 packed 类型（A_t8）解包到 float
- **关键特性**：
  - 使用 `data_split` 数组存储解包后的数据
  - 使用 `union` 进行类型转换（unsigned int ↔ float）
  - 添加 `UNROLL` pragma 优化解包循环
  - 添加 `PIPELINE` pragma 优化流水线

#### A_IO_L2_in_inter_trans
- **功能**：数据加载，从 FIFO 读取数据到本地缓冲区
- **关键特性**：
  - 根据模块 ID 决定是存储还是转发
  - 添加 `PIPELINE` pragma 优化数据传输
  - 支持数据转发到下一个 IO 模块

#### A_IO_L2_in_inter_trans_boundary
- **功能**：边界模块的数据加载
- **关键特性**：
  - 只处理当前模块的数据加载
  - 不转发数据（因为是边界）

#### A_IO_L2_in
- **功能**：双缓冲主函数，协调 inter_trans 和 intra_trans
- **关键特性**：
  - ping-pong 缓冲区切换（使用 `arb` 变量）
  - inter_trans 和 intra_trans 的交替执行
  - 使用 `c0_prev, c1_prev, c2_prev` 跟踪上一次迭代
  - 正确处理最后一个 tile 的数据传输

#### A_IO_L2_in_boundary
- **功能**：边界模块的双缓冲处理
- **关键特性**：
  - 使用 `inter_trans_boundary` 而不是 `inter_trans`
  - 其他逻辑与 `A_IO_L2_in` 相同

### 2. 双缓冲逻辑

**Ping-Pong 机制**：
```cpp
bool arb = 0;  // 0 = ping active, 1 = pong active
if (arb == 0) {
  // Load to pong, send from ping
  inter_trans(..., local_pong, ...);
  intra_trans(..., local_ping, ...);
} else {
  // Load to ping, send from pong
  inter_trans(..., local_ping, ...);
  intra_trans(..., local_pong, ...);
}
arb = !arb;  // Toggle
```

**使能控制**：
```cpp
bool inter_trans_en = 1;  // Enable loading
bool intra_trans_en = 0;  // Initially disabled
// After first iteration:
intra_trans_en = 1;  // Enable sending
```

### 3. 数据解包逻辑

**从 packed 类型解包到 float**：
```cpp
ap_uint<32> data_split[8];
#pragma HLS ARRAY_PARTITION variable=data_split complete

// Unpack loop
for (ap_uint<4> n = 0; n < 8; n++) {
  #pragma HLS UNROLL
  data_split[n] = in_data(31, 0);
  in_data = in_data >> 32;
}

// Type conversion
union {unsigned int ui; float ut;} u;
u.ui = (unsigned int)data_split[split_idx];
out_data = u.ut;
```

---

## 与 AutoSA 的对比

### 已实现的功能

✅ **基本结构**：
- kernel0 函数
- PE 函数
- IO_L3_in 模块
- IO_L2_in 模块（完整实现）
- 双缓冲逻辑
- 数据解包逻辑

✅ **优化 pragma**：
- PIPELINE pragma（15 个）
- UNROLL pragma
- ARRAY_PARTITION pragma
- RESOURCE pragma

### 待完善的功能

⚠️ **Drain 模块**：
- C_drain_IO_L1_out 模块需要完善
- C_drain_IO_L2_out 模块需要实现
- C_drain_IO_L3_out 模块需要实现
- C_drain 序列化逻辑需要完善

⚠️ **更多 PIPELINE pragma**：
- 目标：24 个（当前 15 个）
- 需要在更多内层循环添加

⚠️ **边界处理**：
- 某些边界情况可能需要特殊处理

---

## 测试结果

### 配置：I32_J32_K32_ap8_lat4_simd1

**对比结果**：
- ✅ kernel0 函数存在
- ✅ PE 函数存在
- ✅ IO 模块存在（包括完整的 IO_L2_in）
- ✅ 数据类型定义存在
- ✅ 双缓冲逻辑存在
- ✅ 数据解包逻辑存在

**统计信息**：
- 代码行数：1055（AutoSA：1499）
- PIPELINE pragma：15（AutoSA：24）
- 函数数量：36（AutoSA：~30）

---

## 下一步优化方向

### 优先级 1：完善 Drain 模块

1. **C_drain_IO_L1_out**：
   - 实现数据打包逻辑（float → C_t4）
   - 实现数据转发逻辑

2. **C_drain_IO_L2_out**：
   - 实现双缓冲逻辑
   - 实现数据转发

3. **C_drain_IO_L3_out**：
   - 实现序列化逻辑（C_t4 → C_t16）
   - 实现内存写入逻辑

### 优先级 2：添加更多 PIPELINE Pragma

1. **IO 模块**：
   - 在更多内层循环添加 PIPELINE
   - 优化数据传输流水线

2. **Drain 模块**：
   - 在数据打包循环添加 PIPELINE
   - 在内存写入循环添加 PIPELINE

### 优先级 3：代码质量优化

1. **边界处理**：
   - 完善边界模块的特殊处理
   - 确保所有边界情况都正确处理

2. **代码结构**：
   - 确保代码结构与 AutoSA 一致
   - 优化代码可读性

---

## 相关文件

- `tools/systolic-translate/systolic-translate.cpp` - HLS 代码生成器
- `test/test_against_autosa.sh` - 测试脚本
- `test/TEST_RESULTS.md` - 测试结果文档

---

## 参考文档

- [AutoSA 参考样本](../test/reference-samples/README.md)
- [测试结果](../test/TEST_RESULTS.md)
- [项目进展总结](PROJECT_STATUS.md)

