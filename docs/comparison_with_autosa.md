# mlir-systolic vs AutoSA: HLS C++ 生成对比分析

## 配置参数

两者使用相同的配置：
- **问题规模**: 32x32x32 矩阵乘法
- **Array Partition (array_part)**: 8
- **Latency**: 4
- **SIMD**: 1
- **PE 阵列大小**: 2x2

## 代码行数对比

| 指标 | mlir-systolic | AutoSA | 差异 |
|------|---------------|--------|------|
| 总行数 | 334 | 1499 | 4.5x 更精简 |
| 模块数量 | 17 | 43 | 更简洁的设计 |

## 结构对比

### 1. 数据类型定义 ✅ **完全匹配**

**两者完全相同:**
```cpp
typedef float A_t1;
typedef float B_t1;
typedef float C_t1;
typedef ap_uint<512> A_t16;
typedef ap_uint<256> A_t8;
typedef ap_uint<512> B_t16;
typedef ap_uint<256> B_t8;
typedef ap_uint<512> C_t16;
typedef ap_uint<128> C_t4;
```

### 2. PE 模块 ✅ **完全匹配**

**两者的 PE 模块代码逐行相同:**
```cpp
void PE(int idx, int idy, hls::stream<float> &fifo_A_in, ...) {
#pragma HLS INLINE OFF
  int p0 = idx, p1 = idy; // module id
  A_t1 local_A[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_A dim=0 complete
  B_t1 local_B[1][1];
  #pragma HLS ARRAY_PARTITION variable=local_B dim=0 complete
  C_t1 local_C[4][4];
  #pragma HLS RESOURCE variable=local_C core=RAM_2P_BRAM
  
  for (ap_uint<3> c0 = 0; c0 <= 3; c0 += 1)
    for (ap_uint<3> c1 = 0; c1 <= 3; c1 += 1)
      for (ap_uint<3> c2 = 0; c2 <= 3; c2 += 1) {
        // array, pe
        for (ap_uint<4> c5 = 0; c5 <= 7; c5 += 1) {
          // latency
          for (ap_uint<3> c6 = 0; c6 <= 3; c6 += 1) {
            // latency
            for (ap_uint<3> c7 = 0; c7 <= 3; c7 += 1) {
            #pragma HLS PIPELINE II=1
              {
                local_A[0][0] = fifo_A_in.read();
                local_B[0][0] = fifo_B_in.read();
                local_C[c7][c6] = (local_C[c7][c6] + (local_A[0][0] * local_B[0][0]));
                if (c2 == 3 && c5 == 7)
                  fifo_C_drain_out.write(local_C[c7][c6]);
                fifo_B_out.write(local_B[0][0]);
                fifo_A_out.write(local_A[0][0]);
              }
            }
          }
        }
      }
}
```

### 3. kernel0 接口 ✅ **完全匹配**

**两者的接口 pragma 完全相同:**
```cpp
#pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem_A
#pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem_B
#pragma HLS INTERFACE m_axi port=C offset=slave bundle=gmem_C
#pragma HLS INTERFACE s_axilite port=A bundle=control
#pragma HLS INTERFACE s_axilite port=B bundle=control
#pragma HLS INTERFACE s_axilite port=C bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control
#pragma HLS DATAFLOW
```

### 4. 循环边界 ✅ **完全匹配**

| 循环 | mlir-systolic | AutoSA | 含义 |
|------|---------------|--------|------|
| c0, c1, c2 | 0 to 3 | 0 to 3 | Tile 迭代 (32/(4*2)=4) |
| c5 | 0 to 7 | 0 to 7 | PE 内迭代 (8/1=8) |
| c6, c7 | 0 to 3 | 0 to 3 | Latency 隐藏 (4) |

### 5. HLS Pragma 对比

| Pragma | mlir-systolic | AutoSA |
|--------|---------------|--------|
| PIPELINE II=1 | 7 | 24 |
| INLINE OFF | 5 | 20 |
| DATAFLOW | 1 | 1 |
| FIFO_SRL | 22 | 31 |
