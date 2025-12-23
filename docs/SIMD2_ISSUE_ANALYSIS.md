# SIMD=2 代码生成问题分析

> **最后更新**: 2024-12  
> **问题**: 生成的 SIMD=2 代码与 AutoSA 参考代码存在关键差异

---

## 问题概述

对比生成的 `mm_st3_I32_J32_K32_ap8_lat4_simd2_kernel.cpp` 和 AutoSA 参考代码，发现以下关键问题：

1. **缺少 A_t2 和 B_t2 数据类型定义**
2. **PE 模块未实现 SIMD 展开**
3. **IO_L2_in_intra_trans 输出类型错误**
4. **L3 数据读取量差异（参考文件可能有问题）**
5. **数组索引模式差异**

---

## 1. 数据类型定义问题

### 参考文件（AutoSA）
```cpp
typedef ap_uint<64> A_t2;  // 64 bits = 2 个 float (SIMD=2)
typedef ap_uint<64> B_t2;  // 64 bits = 2 个 float (SIMD=2)
```

### 生成文件（当前实现）
```cpp
// 缺失 A_t2 和 B_t2 定义
```

**影响**：
- 无法在 PE 中实现 SIMD 展开
- IO_L2_in_intra_trans 无法输出打包的数据类型

**修复方案**：
- 在 `emitDataTypes()` 中，当 `simd > 1` 时，添加 `A_t{simd}` 和 `B_t{simd}` 定义
- 对于 SIMD=2：`typedef ap_uint<64> A_t2;`（64 bits = 2 * 32 bits）

---

## 2. PE 模块 SIMD 展开问题

### 参考文件（AutoSA）
```cpp
void PE(int idx, int idy, 
        hls::stream<A_t2> &fifo_A_in, hls::stream<A_t2> &fifo_A_out,
        hls::stream<B_t2> &fifo_B_in, hls::stream<B_t2> &fifo_B_out,
        hls::stream<float> &fifo_C_drain_out) {
  A_t1 local_A[1][2];  // SIMD=2，2 个元素
  B_t1 local_B[1][2];  // SIMD=2，2 个元素
  
  // ...
  {
    // 从 A_t2 解包成 2 个 float
    A_t2 fifo_data = fifo_A_in.read();
    for (ap_uint<2> n = 0; n < 2; n++) {
    #pragma HLS UNROLL
      union {unsigned int ui; float ut;} u;
      u.ui = (unsigned int)fifo_data(31, 0);
      local_A[0][n] = u.ut;
      fifo_data = fifo_data >> 32;
    }
    
    // 从 B_t2 解包成 2 个 float
    B_t2 fifo_data = fifo_B_in.read();
    for (ap_uint<2> n = 0; n < 2; n++) {
    #pragma HLS UNROLL
      union {unsigned int ui; float ut;} u;
      u.ui = (unsigned int)fifo_data(31, 0);
      local_B[0][n] = u.ut;
      fifo_data = fifo_data >> 32;
    }
    
    // SIMD 展开循环
    for (ap_uint<2> c8 = 0; c8 <= 1; c8 += 1) {
    #pragma HLS UNROLL
      local_C[c7][c6] = (local_C[c7][c6] + (local_A[0][c8] * local_B[0][c8]));
    }
    
    // 打包回 A_t2 和 B_t2
    // ...
  }
}
```

### 生成文件（当前实现）
```cpp
void PE(int idx, int idy,
        hls::stream<float> &fifo_A_in, hls::stream<float> &fifo_A_out,
        hls::stream<float> &fifo_B_in, hls::stream<float> &fifo_B_out,
        hls::stream<float> &fifo_C_drain_out) {
  A_t1 local_A[1][1];  // 只有 1 个元素，无法 SIMD
  B_t1 local_B[1][1];  // 只有 1 个元素，无法 SIMD
  
  // ...
  {
    local_A[0][0] = fifo_A_in.read();  // 标量读取
    local_B[0][0] = fifo_B_in.read();  // 标量读取
    local_C[c7][c6] = (local_C[c7][c6] + (local_A[0][0] * local_B[0][0]));  // 标量计算
    // 没有 SIMD 展开
  }
}
```

**问题**：
- PE 接收的是 `float` 而不是 `A_t2`/`B_t2`
- `local_A` 和 `local_B` 只有 1 个元素，无法展开
- 没有 SIMD 循环

**修复方案**：
- 修改 `emitPE()` 函数：
  1. 当 `simd > 1` 时，PE 签名使用 `A_t{simd}` 和 `B_t{simd}`
  2. `local_A` 和 `local_B` 声明为 `[1][simd]`
  3. 添加解包逻辑（从 `A_t{simd}` 解包成 `simd` 个 `float`）
  4. 添加 SIMD 展开循环（`for (ap_uint<X> c8 = 0; c8 < simd; c8++)`）
  5. 添加打包逻辑（将 `simd` 个 `float` 打包回 `A_t{simd}`）

---

## 3. IO_L2_in_intra_trans 输出类型问题

### 参考文件（AutoSA）
```cpp
void A_IO_L2_in_intra_trans(..., hls::stream<A_t2> &fifo_A_local_out, ...) {
  A_t2 data_split[4];  // 4 个 A_t2
  // ...
  in_data = local_A[c7][2 * c5 / 8];  // 非线性索引
  for (ap_uint<3> n = 0; n < 4; n++) {
  #pragma HLS UNROLL
    data_split[n] = in_data(63, 0);  // 64 bits = A_t2
    in_data = in_data >> 64;
  }
  int split_idx = (c5) % 4;
  out_data = data_split[split_idx];  // 输出 A_t2
  fifo_A_local_out.write(out_data);
}
```

### 生成文件（当前实现）
```cpp
void A_IO_L2_in_intra_trans(..., hls::stream<float> &fifo_A_local_out, ...) {
  ap_uint<32> data_split[8];  // 8 个 32-bit
  // ...
  in_data = local_A[c7][0];  // 线性索引
  for (ap_uint<4> n = 0; n < 8; n++) {
  #pragma HLS UNROLL
    data_split[n] = in_data(31, 0);  // 32 bits = float
    in_data = in_data >> 32;
  }
  int split_idx = (c5) % 8;
  union {unsigned int ui; float ut;} u;
  u.ui = (unsigned int)data_split[split_idx];
  out_data = u.ut;  // 输出 float
  fifo_A_local_out.write(out_data);
}
```

**问题**：
- 输出类型是 `float` 而不是 `A_t2`
- 数据拆分粒度是 32 bits（单个 float）而不是 64 bits（2 个 float）
- 数组索引是线性的 `[c7][0]` 而不是非线性的 `[c7][2 * c5 / 8]`

**修复方案**：
- 修改 `emitIOL2InIntraTrans()` 函数：
  1. 当 `simd > 1` 时，输出类型改为 `A_t{simd}`
  2. 数据拆分粒度改为 `simd * 32` bits
  3. 拆分数组大小改为 `arrayPart / simd`
  4. 索引计算需要考虑 SIMD 和可能的转置

---

## 4. L3 数据读取量问题

### 参考文件（AutoSA）
```cpp
void A_IO_L3_in_serialize(A_t16 *A, hls::stream<A_t8> &fifo_A_local_out) {
  for (ap_uint<9> i = 0; i < 256; i++) {  // 读取 256 个 A_t16
    mem_data = A[i];
    for (ap_uint<2> p = 0; p < 2; p++) {
      fifo_data = mem_data(255, 0);
      mem_data = mem_data >> 256;
      fifo_A_local_out.write(fifo_data);
    }
  }
}
```

**计算**：
- 32×32 矩阵 = 1024 个 float
- 每个 A_t16 = 512 bits = 16 个 float
- 需要 1024 / 16 = 64 个 A_t16
- **但参考文件读取 256 个，是 4 倍！**

**可能的原因**：
1. AutoSA 的 bug
2. 考虑了某种数据复用或分块
3. 与 B 矩阵转置有关（转置后数据布局不同）

### 生成文件（当前实现）
```cpp
void A_IO_L3_in_serialize(A_t16 *A, hls::stream<A_t8> &fifo_A_local_out) {
  for (ap_uint<7> i = 0; i < 64; i++) {  // 读取 64 个 A_t16（正确）
    // ...
  }
}
```

**结论**：
- 生成文件的读取量是正确的（64 个 A_t16）
- 参考文件的读取量可能是错误的，或者是 AutoSA 的特殊处理

---

## 5. 数组索引模式问题

### 参考文件（AutoSA）
```cpp
in_data = local_A[c7][2 * c5 / 8];
```

**分析**：
- `c5` 范围：0-3（`c5Bound = arrayPart / simd = 8 / 2 = 4`）
- `2 * c5 / 8`：
  - c5=0: 0
  - c5=1: 0
  - c5=2: 0
  - c5=3: 0
- 实际上总是访问 `local_A[c7][0]`

**但为什么这样写？**
- 可能与 SIMD 和转置有关
- 当 SIMD=2 时，需要从 `A_t8`（256 bits）中提取 2 个 `A_t2`（64 bits）
- `2 * c5 / 8` 可能用于计算在 `A_t8` 中的偏移

### 生成文件（当前实现）
```cpp
in_data = local_A[c7][0];  // 线性索引
```

**问题**：
- 没有考虑 SIMD 和转置的影响
- 当 SIMD=2 时，可能需要非线性索引来正确提取数据

---

## 6. B 矩阵转置的影响

用户提到：**AutoSA 处理的矩阵乘法 C 代码，已经对 B 矩阵进行转置（行列互换）**

这意味着：
- 原始计算：`C[i][j] += A[i][k] * B[k][j]`
- AutoSA 处理：`C[i][j] += A[i][k] * B[j][k]`（B 已转置）

**影响**：
- 数据访问模式不同
- 数组索引计算不同
- 可能需要不同的数据打包/解包逻辑

**当前实现**：
- 我们的输入 MLIR 可能没有考虑 B 矩阵转置
- 这可能导致数据访问模式不匹配

---

## 修复建议

### 优先级 1：修复 SIMD 支持

1. **添加 A_t{simd} 和 B_t{simd} 类型定义**
   ```cpp
   if (simd > 1) {
     os << "typedef ap_uint<" << (simd * 32) << "> A_t" << simd << ";\n";
     os << "typedef ap_uint<" << (simd * 32) << "> B_t" << simd << ";\n";
   }
   ```

2. **修改 IO_L2_in_intra_trans 输出类型**
   - 当 `simd > 1` 时，输出 `A_t{simd}` 而不是 `float`
   - 修改数据拆分逻辑（64 bits 而不是 32 bits）

3. **修改 PE 模块实现 SIMD**
   - PE 签名使用 `A_t{simd}` 和 `B_t{simd}`
   - 添加解包/打包逻辑
   - 添加 SIMD 展开循环

### 优先级 2：验证 L3 数据读取量

- 检查 AutoSA 参考文件是否真的有问题
- 或者是否有特殊的数据复用逻辑

### 优先级 3：处理 B 矩阵转置

- 确认输入 MLIR 是否需要转置 B 矩阵
- 或者在我们的代码生成中处理转置

---

## 相关文件

- `tools/systolic-translate/systolic-translate.cpp`：
  - `emitDataTypes()` - 需要添加 A_t{simd} 和 B_t{simd}
  - `emitIOL2InIntraTrans()` - 需要修改输出类型和拆分逻辑
  - `emitPE()` - 需要实现 SIMD 展开
  - `emitPEWrapper()` - 需要修改签名

---

## 下一步行动

1. ✅ 分析问题（本文档）
2. ⏳ 修复数据类型定义
3. ⏳ 修复 IO_L2_in_intra_trans
4. ⏳ 修复 PE 模块 SIMD 支持
5. ⏳ 验证 L3 数据读取量
6. ⏳ 处理 B 矩阵转置问题

