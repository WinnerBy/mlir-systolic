# AutoSA Reference Samples (space-time=3)

这个目录包含使用 AutoSA 生成的 HLS C++ kernel 代码，用于与 ScaleHLS 生成的代码进行对比验证。

**所有样本使用 `space-time=3` (2D systolic array, i/j 作为空间维度)**

## 命名规则

```
mm_st{ST}_I{I}_J{J}_K{K}_ap{AP}_lat{LAT}_simd{SIMD}_kernel.cpp
```

- `st{ST}`: space-time 参数 (当前全部为 st3)
- `I/J/K`: 矩阵尺寸
- `ap{AP}`: array_part 参数
- `lat{LAT}`: latency 参数
- `simd{SIMD}`: SIMD 因子

## 样本配置

| 样本文件 | 矩阵尺寸 | array_part | latency | simd | PE 阵列 |
|----------|----------|------------|---------|------|---------|
| `mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp` | 32×32 | 8 | 4 | 1 | 2×2 |
| `mm_st3_I32_J32_K32_ap8_lat4_simd2_kernel.cpp` | 32×32 | 8 | 4 | 2 | 2×2 |
| `mm_st3_I64_J64_K64_ap16_lat8_simd1_kernel.cpp` | 64×64 | 16 | 8 | 1 | 2×2 |
| `mm_st3_I64_J64_K64_ap16_lat8_simd2_kernel.cpp` | 64×64 | 16 | 8 | 2 | 2×2 |
| `mm_st3_I64_J64_K64_ap16_lat8_simd4_kernel.cpp` | 64×64 | 16 | 8 | 4 | 2×2 |
| `mm_st3_I64_J64_K64_ap16_lat4_simd1_kernel.cpp` | 64×64 | 16 | 4 | 1 | 4×4 |
| `mm_st3_I64_J64_K64_ap32_lat8_simd1_kernel.cpp` | 64×64 | 32 | 8 | 1 | 4×4 |
| `mm_st3_I64_J64_K64_ap32_lat16_simd1_kernel.cpp` | 64×64 | 32 | 16 | 1 | 2×2 |
| `mm_st3_I128_J128_K128_ap32_lat16_simd1_kernel.cpp` | 128×128 | 32 | 16 | 1 | 2×2 |
| `mm_st3_I128_J128_K128_ap32_lat16_simd2_kernel.cpp` | 128×128 | 32 | 16 | 2 | 2×2 |
| `mm_st3_I128_J128_K128_ap64_lat16_simd1_kernel.cpp` | 128×128 | 64 | 16 | 1 | 4×4 |

## PE 阵列大小公式

```
numPE = array_part / latency
```

例如：
- `array_part=16, latency=8` → `16/8 = 2` → 2×2 PE 阵列
- `array_part=32, latency=8` → `32/8 = 4` → 4×4 PE 阵列

## SIMD 类型映射

| SIMD | AutoSA HLS C++ 类型 | ScaleHLS MLIR 类型 |
|------|---------------------|-------------------|
| 1 | `float` | `f32` |
| 2 | `ap_uint<64>` | `vector<2xf32>` |
| 4 | `ap_uint<128>` | `vector<4xf32>` |

## 生成命令

这些样本使用以下 AutoSA 命令生成：

```bash
./autosa mm.c \
    --config=autosa_config/autosa_config.json \
    --target=autosa_hls_c \
    --output-dir=output \
    --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}" \
    --simd-info=autosa_tests/mm/simd_info.json \
    --host-serialize
```

注意：`--host-serialize` 参数是生成 packed 类型（如 `A_t2 = ap_uint<64>`）所必需的。

## 关键代码结构

### PE 结构

```cpp
void PE(/* streams */) {
  // Local accumulation buffer
  C_t1 local_C[LAT_I][LAT_J];
  
  // SIMD buffers
  A_t1 local_A[1][SIMD];
  B_t1 local_B[1][SIMD];
  
  // Main loop nest
  for (int c0 = 0; c0 < TILE_I; c0++)
    for (int c1 = 0; c1 < TILE_J; c1++)
      for (int c2 = 0; c2 < TILE_K; c2++)
        for (int c3 = 0; c3 < LAT_I; c3++)
          for (int c4 = 0; c4 < LAT_J; c4++)
            for (int c5 = 0; c5 < K / SIMD; c5++) {  // SIMD reduction
              // Read from streams
              // Unpack SIMD elements
              // MAC operations
              // Forward to neighbors
            }
}
```

### 数据打包/解包

```cpp
// 打包 (pack): float → ap_uint<64>
ap_uint<64> fifo_data = 0;
for (int n = 0; n < 2; n++) {
  union { unsigned int ui; float ut; } u;
  u.ut = local_A[0][n];
  fifo_data = fifo_data | (ap_uint<32>(u.ui) << (n * 32));
}

// 解包 (unpack): ap_uint<64> → float
ap_uint<64> fifo_data = fifo_A_in.read();
for (int n = 0; n < 2; n++) {
  union { unsigned int ui; float ut; } u;
  u.ui = fifo_data(31, 0);
  local_A[0][n] = u.ut;
  fifo_data = fifo_data >> 32;
}
```

## 用途

这些样本用于：

1. **验证 ScaleHLS PE 结构**：确认生成的 MLIR 与 AutoSA 结构一致
2. **验证 EmitHLSCpp**：对比生成的 HLS C++ 代码
3. **回归测试**：确保未来修改不破坏现有功能
