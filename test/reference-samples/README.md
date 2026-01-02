# AutoSA Reference Samples

这个目录包含使用 AutoSA 生成的 HLS C++ kernel 代码，用于与 mlir-systolic 生成的代码进行对比验证。

**样本来源**：
- 所有 samples 来自 `autosa_reference_samples/collected_hls_files/`

**重要更新**：
- ✅ **新生成的 HLS C++ 文件包含头文件信息**：不再需要额外的头文件（如 `kernel.h`）
- ✅ **文件自包含**：每个 `kernel_kernel.cpp` 文件都包含必要的 `#include` 语句和类型定义
- ✅ **删除了 `--hls` 参数**：生成的代码更加完整和独立

**注意**：已排除无效配置：
- `mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8` - array part 比问题规模大
- `mm_st3_I64_J64_K64_ap32_lat11_32_simd64` - 代码生成失败
- `dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2` - 代码生成失败

## 命名规则

### 矩阵乘法 (MM)
```
mm_st{ST}_I{I}_J{J}_K{K}_ap{AP}_lat{LAT}_simd{SIMD}_kernel.cpp
```

### 其他 Kernel
```
{kernel_type}_st{ST}_ap{AP}_lat{LAT}_simd{SIMD}_kernel.cpp
```

**参数说明**：
- `st{ST}`: space-time 参数
  - `st0`: 1D [i]
  - `st1`: 1D [j]
  - `st2`: 1D [k] (需要 reduction)
  - `st3`: 2D [i,j] ✅ **当前主要支持**
  - `st4`: 2D [i,k] (需要 reduction)
  - `st5`: 2D [j,k] (需要 reduction)
- `I/J/K`: 矩阵尺寸（仅 MM）
- `ap{AP}`: array_part 参数（可以是单个值或多个值，如 `ap8_8_4_8`）
- `lat{LAT}`: latency 参数（可以是单个值或多个值，如 `lat4_2_4`）
- `simd{SIMD}`: SIMD 因子（可以是单个值或多个值，如 `simd1_1_1_2`）

## 样本配置

### 矩阵乘法 (MM) - Space-time=3

| 样本文件 | 矩阵尺寸 | array_part | latency | simd | PE 阵列 | 代码行数 |
|----------|----------|------------|---------|------|---------|----------|
| `mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp` | 32×32 | 8 | 4 | 1 | 2×2 | 1499 |
| `mm_st3_I32_J32_K32_ap8_lat4_simd2_kernel.cpp` | 32×32 | 8 | 4 | 2 | 2×2 | 1538 |
| `mm_st3_I32_J32_K32_ap16_lat8_simd2_kernel.cpp` | 32×32 | 16 | 8 | 2 | 2×2 | 1540 |
| `mm_st3_I64_J64_K64_ap16_lat4_simd1_kernel.cpp` | 64×64 | 16 | 4 | 1 | 4×4 | 2015 |
| `mm_st3_I64_J64_K64_ap16_lat8_simd1_kernel.cpp` | 64×64 | 16 | 8 | 1 | 2×2 | 1501 |
| `mm_st3_I64_J64_K64_ap16_lat8_simd2_kernel.cpp` | 64×64 | 16 | 8 | 2 | 2×2 | 1540 |
| `mm_st3_I64_J64_K64_ap16_lat8_simd4_kernel.cpp` | 64×64 | 16 | 8 | 4 | 2×2 | 1544 |
| `mm_st3_I64_J64_K64_ap32_lat8_simd1_kernel.cpp` | 64×64 | 32 | 8 | 1 | 4×4 | 2045 |
| `mm_st3_I64_J64_K64_ap32_lat16_simd1_kernel.cpp` | 64×64 | 32 | 16 | 1 | 2×2 | 1517 |
| `mm_st3_I128_J128_K128_ap32_lat16_simd1_kernel.cpp` | 128×128 | 32 | 16 | 1 | 2×2 | 1517 |
| `mm_st3_I128_J128_K128_ap32_lat16_simd2_kernel.cpp` | 128×128 | 32 | 16 | 2 | 2×2 | 1556 |
| `mm_st3_I128_J128_K128_ap64_lat16_simd1_kernel.cpp` | 128×128 | 64 | 16 | 1 | 4×4 | 2045 |

### 矩阵乘法 (MM) - 其他 Space-time

| 样本文件 | Space-time | 矩阵尺寸 | array_part | latency | simd | 说明 | 代码行数 |
|----------|------------|----------|------------|---------|------|------|----------|
| `mm_st0_I32_J32_K32_ap32_lat8_simd2_kernel.cpp` | 0 (1D [i]) | 32×32 | 32 | 8 | 2 | 1D 脉动阵列 | 1082 |
| `mm_st1_I32_J32_K32_ap32_lat8_simd2_kernel.cpp` | 1 (1D [j]) | 32×32 | 32 | 8 | 2 | 1D 脉动阵列 | 1082 |
| `mm_st2_I32_J32_K32_ap4_lat2_simd2_kernel.cpp` | 2 (1D [k]) | 32×32 | 4 | 2 | 2 | 1D 脉动阵列（需要 reduction） | 1105 |
| `mm_st4_I32_J32_K32_ap32_lat16_simd2_kernel.cpp` | 4 (2D [i,k]) | 32×32 | 32 | 16 | 2 | 2D 脉动阵列（需要 reduction） | 1528 |
| `mm_st5_I32_J32_K32_ap32_lat16_simd2_kernel.cpp` | 5 (2D [j,k]) | 32×32 | 32 | 16 | 2 | 2D 脉动阵列（需要 reduction） | 1520 |

### 其他 Kernel 类型

| 样本文件 | Kernel 类型 | Space-time | 说明 | 代码行数 |
|----------|-------------|------------|------|----------|
| `cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2_kernel.cpp` | CNN | 4 | 卷积神经网络 | 1845 |
| `mttkrp_st3_ap128_128_2_lat16_8_simd8_1_kernel.cpp` | MTTKRP | 3 | 张量矩阵乘法（已确认随机读取问题） | 7883 |
| `ttm_st4_ap16_64_16_32_lat1_8_8_simd8_1_kernel.cpp` | TTM | 4 | 张量矩阵乘法 | 7754 |
| `ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1_kernel.cpp` | TTMc | 4 | 张量链乘法（可能存在随机读取问题） | 7656 |
| `lu_st3_ap_auto_lat_auto_kernel.cpp` | LU | 3 | LU 分解 | 29828 |

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

## 文件结构特点

### 自包含的 HLS C++ 文件

新生成的 `kernel_kernel.cpp` 文件是**自包含**的，包含：

1. **必要的头文件**：
   ```cpp
   #include <ap_int.h>
   #include <hls_stream.h>
   ```

2. **类型定义**：
   ```cpp
   typedef float A_t1;
   typedef ap_uint<512> A_t16;
   typedef ap_uint<64> A_t2;
   // ...
   ```

3. **函数声明和实现**：
   - 所有模块函数（IO_L2_in, IO_L3_in, PE_wrapper 等）
   - 主 kernel 函数（kernel0）

**优势**：
- ✅ 不需要额外的头文件（如 `kernel.h`）
- ✅ 可以直接编译，无需依赖其他文件
- ✅ 更适合作为参考样本进行对比验证

## 整理说明

这些 samples 是通过 `scripts/organize_reference_samples.sh` 脚本从 AutoSA 生成的代码中整理而来的。

**整理规则**：
1. 从 `autosa_reference_samples/collected_hls_files/` 提取 `kernel_kernel.cpp` 文件
2. 重命名为符合命名规则的格式：`{kernel}_st{ST}_..._kernel.cpp`
3. 排除无效配置：
   - `mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8` - array part 比问题规模大
   - `mm_st3_I64_J64_K64_ap32_lat11_32_simd64` - 代码生成失败
   - `dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2` - 代码生成失败
4. 自动跳过空文件（某些配置可能未成功生成代码）

**生成命令**：

这些样本使用 AutoSA 生成（**删除了 `--hls` 参数**），详见：
- `scripts/generate_autosa_tests.sh` - **统一的生成脚本**，包含矩阵乘法和其他 kernel 类型
- `docs/THREE_QUESTIONS_SOLUTION.md` - 详细说明
- `docs/AUTOSA_TEST_GENERATION_GUIDE.md` - 完整的生成指南

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
