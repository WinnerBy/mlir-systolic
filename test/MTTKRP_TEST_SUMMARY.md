# MTTKRP 写时重排测试总结

> **创建时间**: 2026-01-05  
> **基于**: [AutoSA MTTKRP kernel](https://github.com/UCLA-VAST/AutoSA/blob/master/autosa_tests/large/mttkrp/kernel.c)

---

## 测试文件

### 1. `test/mttkrp_kernel.mlir`
**描述**: 基础的 MTTKRP kernel，包含完整的计算逻辑
- **计算**: `D(i,j) += A(i,k,l) * B(k,j) * C(j,l)`
- **数组**: A[128][64][128], B[64][64], C[64][128], D[128][64]
- **访问模式**: 线性访问（无非线性索引）

**用途**: 测试基础功能，验证代码生成器能正确处理 MTTKRP kernel

### 2. `test/mttkrp_with_nonlinear.mlir`
**描述**: 包含非线性访问模式的 MTTKRP kernel
- **模拟 AutoSA 生成的模式**: `local_A[c8][c5][8 * c6 / 16]`
- **非线性索引**: `(l * 8) floordiv 16` 在第三维度
- **用途**: 测试写时重排分析器是否能检测到非线性访问

**关键模式**:
```mlir
// 写入: 线性访问
affine.store %val, %A[%i, %k, %l] : memref<128x64x128xf32>

// 读取: 非线性访问
%idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%l)
%val = affine.load %A[%i, %k, %idx] : memref<128x64x128xf32>
```

### 3. `test/mttkrp_simple_test.mlir`
**描述**: 简化的测试用例，专注于测试非线性访问检测
- **数组**: A[16][2][16]
- **非线性模式**: `8 * c6 / 16` (第三维度)
- **用途**: 快速验证分析器功能

---

## AutoSA 生成的代码模式

从 `autosa_hls_output/kernel_mttkrp.cpp` 中发现的非线性访问模式：

### 1. A 数组的非线性访问
```cpp
// A_IO_L2_in_intra_trans
void A_IO_L2_in_intra_trans(..., A_t16 local_A[16][2][16], ...) {
  for (ap_uint<4> c8 = 0; c8 <= 7; c8 += 1) {
    for (ap_uint<2> c5 = 0; c5 <= 1; c5 += 1) {
      for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
        in_data = local_A[c8][c5][8 * c6 / 16];  // ⚠️ 非线性索引
      }
    }
  }
}
```

**分析**:
- 第三维度使用 `8 * c6 / 16`
- 当 `c6` 从 0 到 15 变化时，索引值是：`0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1`
- 导致非顺序访问模式

### 2. C 数组的非线性访问
```cpp
// C_IO_L2_in_intra_trans
void C_IO_L2_in_intra_trans(..., C_t16 local_C[8][16], ...) {
  for (ap_uint<4> c7 = 0; c7 <= 7; c7 += 1) {
    for (ap_uint<5> c6 = 0; c6 <= 15; c6 += 1) {
      in_data = local_C[c7][8 * c6 / 16];  // ⚠️ 非线性索引
    }
  }
}
```

**分析**:
- 第二维度使用 `8 * c6 / 16`
- 类似的非顺序访问模式

---

## 预期重排方案

### A 数组 (3D: [16][2][16])
**原始布局**: `local_A[16][2][16]`
**非线性维度**: 第三维度 (dim 2)
**重排后布局**: `local_A[2][16][16]`
**置换**: `[1, 2, 0]` (将 dim 2 移到中间)

### C 数组 (2D: [8][16])
**当前状态**: 2D 数组重排尚未实现
**未来工作**: 需要扩展支持 2D 数组的重排

---

## 测试脚本

### `test/test_mttkrp_reordering.sh`
**功能**:
1. 测试基础 MTTKRP kernel
2. 测试包含非线性访问的 MTTKRP kernel
3. 检查重排属性生成
4. 生成 HLS 代码并验证数组声明

**运行**:
```bash
./test/test_mttkrp_reordering.sh
```

---

## 当前状态

### ✅ 已完成
- 创建了 MTTKRP 测试文件
- 识别了 AutoSA 生成代码中的非线性访问模式
- 创建了测试脚本

### ⚠️ 待解决
- Pass 运行时出现段错误（需要调试）
- 需要验证分析器是否能正确检测非线性访问
- 需要验证重排属性是否正确生成

### 🔄 下一步
1. 调试 pass 运行时的段错误
2. 验证分析器功能（可能需要独立测试）
3. 检查生成的 HLS 代码中的数组声明
4. 对比 AutoSA 生成的代码

---

## 相关文档

- [随机读取问题分析](../docs/RANDOM_ACCESS_ANALYSIS.md)
- [写时重排实现](../docs/WRITE_TIME_REORDERING_IMPLEMENTATION.md)
- [基于多面体模型的实现](../docs/WRITE_TIME_REORDERING_POLYHEDRAL.md)
- [AutoSA MTTKRP kernel](https://github.com/UCLA-VAST/AutoSA/blob/master/autosa_tests/large/mttkrp/kernel.c)

---

## AutoSA 命令参考

根据 `autosa_cmd.md`，MTTKRP 的 AutoSA 命令：
```bash
./autosa ./autosa_tests/large/mttkrp/kernel.c \
  --config=./autosa_config/autosa_config.json \
  --target=autosa_hls_c \
  --output-dir=./autosa.tmp/output \
  --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}" \
  --simd-info=./autosa_tests/large/mttkrp/simd_info.json \
  --host-serialize
```

**参数**:
- `space_time=3`
- `array_part=[128,128,2]`
- `latency=[16,8]`
- `simd=[8,1]`
- `problem_size`: I=256, J=336, K=256, L=256


