# 三个问题的解决方案总结

> **最后更新**: 2024-12  
> **目的**: 总结三个关键问题的解决方案

---

## 问题 1：生成不同 Spacetime 和参数的测试用例

### 解决方案

已创建脚本：`scripts/generate_autosa_tests.sh`

**功能**：
- 生成不同 spacetime 值的 MM 测试用例（0, 1, 2, 3, 4, 5）
- 生成多维参数的测试用例（不同 array_part, latency, simd）

**使用方法**：
```bash
# 设置环境变量（可选）
export AUTOSA_ROOT=/home/user/work/AutoSA
export OUTPUT_DIR=./autosa_reference_samples

# 运行脚本
./scripts/generate_autosa_tests.sh
```

**生成的测试用例**：

| Spacetime | 输出目录 | 说明 |
|-----------|----------|------|
| 0 | `mm_st0_I32_J32_K32_ap32_lat8_simd2` | 1D [i] |
| 1 | `mm_st1_I32_J32_K32_ap32_lat8_simd2` | 1D [j] |
| 2 | `mm_st2_I32_J32_K32_ap4_lat2_simd2` | 1D [k] (需要 reduction) |
| 3 | `mm_st3_I32_J32_K32_ap16_lat8_simd2` | 2D [i,j] |
| 4 | `mm_st4_I32_J32_K32_ap32_lat16_simd2` | 2D [i,k] (需要 reduction) |
| 5 | `mm_st5_I32_J32_K32_ap32_lat16_simd2` | 2D [j,k] (需要 reduction) |

**多维参数测试用例**：
- `mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8` - 不同 array_part 和 latency
- `mm_st3_I64_J64_K64_ap32_lat11_32_simd64` - 不同 latency 和大 SIMD

**参数配置表**：

详见 `docs/AUTOSA_TEST_GENERATION_GUIDE.md` 第 1.2 节

---

## 问题 2：其他 Kernel 类型验证

### 解决方案

已创建脚本：`scripts/generate_other_kernels.sh`

**支持的 Kernel 类型**：

| Kernel | 路径 | 复杂度 | 优先级 |
|--------|------|--------|--------|
| **CNN** | `autosa_tests/cnn/` | 中等（5 个循环） | P1 |
| **DNN Ops** | `autosa_tests/dnn_ops/` | 中等 | P1 |
| **MTTKRP** | `autosa_tests/large/mttkrp/` | 复杂（4 个循环，3D 数组） | P2 |
| **TTMc** | `autosa_tests/large/ttmc/` | 复杂（5 个循环，3D 数组） | P2 |
| **LU** | `autosa_tests/lu/` | 复杂（需要特殊处理） | P3 |

**使用方法**：
```bash
# 设置环境变量（可选）
export AUTOSA_ROOT=/home/user/work/AutoSA
export OUTPUT_DIR=./autosa_reference_samples

# 运行脚本
./scripts/generate_other_kernels.sh
```

**生成的测试用例**：
- `cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2`
- `dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2`
- `mttkrp_st3_ap128_128_2_lat16_8_simd8_1`
- `ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1`

**详细参数配置**：

详见 `docs/AUTOSA_TEST_GENERATION_GUIDE.md` 第 2 节

---

## 问题 3：随机读取问题分析

### 问题确认

#### MTTKRP（已确认）

**问题位置**：
1. `A_IO_L2_in_intra_trans:77`: `local_A[c8][c5][4 * c6 / 16]`
2. `C_IO_L2_in_intra_trans:789`: `local_C[c7][4 * c6 / 16]`

**问题特征**：
- 非线性索引表达式：`4 * c6 / 16`
- 导致非顺序访问模式
- 影响 HLS 性能和资源使用

**修复方法**：
- 数组维度重排：`[8][2][4]` → `[2][4][8]`
- 写入时重排：`local_A[c5][c6][c4]`
- 读取时使用重排后的布局：`local_A[c5][4 * c6 / 16][c8]`

**性能提升**：
- 已在板卡上验证，有性能提升

### 其他 Kernel 检查

#### 检查方法

已创建脚本：`scripts/check_random_access.sh`

**使用方法**：
```bash
# 检查单个文件
./scripts/check_random_access.sh /path/to/kernel.cpp

# 检查目录
./scripts/check_random_access.sh /path/to/autosa/output/src

# 详细分析 MTTKRP
./scripts/check_random_access.sh /path/to/mttkrp_kernel.cpp --analyze-mttkrp
```

#### 可能存在的问题

| Kernel | 数组维度 | 可能的问题 | 检查方法 |
|--------|----------|------------|----------|
| **MTTKRP** | `A[I][K][L]` | ✅ **已确认** | 已修复 |
| **TTMc** | `A[I][L][M]`, `D[I][J][K]` | ⚠️ **可能** | 生成代码后检查 |
| **CNN** | `cin[R+K-1][C+K-1][I]`, `w[O][K][K][I]` | ⚠️ **可能** | 生成代码后检查 |
| **LU** | `A[N][N]` | ❌ **不太可能** | 2D 数组 |
| **MM** | `A[I][K]`, `B[K][J]`, `C[I][J]` | ❌ **不太可能** | 2D 数组 |

#### 检查清单

**生成代码后**：
1. 运行 `check_random_access.sh` 脚本
2. 检查所有 `IO_L2_in_intra_trans` 函数
3. 查找 `local_*[...][...][...]` 访问中的非线性索引
4. 检查模式：`*`, `/`, `%` 在索引表达式中

**发现问题后**：
1. 分析访问模式
2. 确定重排方案
3. 修改写入代码（`inter_trans`）
4. 修改读取代码（`intra_trans`）
5. 验证功能正确性
6. 测试性能提升

### 详细分析

详见 `docs/RANDOM_ACCESS_ISSUE_ANALYSIS.md`

---

## 总结

### 已完成的工具

1. ✅ **生成脚本**：
   - `scripts/generate_autosa_tests.sh` - 生成不同 spacetime 和参数的测试用例
   - `scripts/generate_other_kernels.sh` - 生成其他 Kernel 类型的测试用例

2. ✅ **检查脚本**：
   - `scripts/check_random_access.sh` - 检查随机读取问题

3. ✅ **文档**：
   - `docs/AUTOSA_TEST_GENERATION_GUIDE.md` - 测试用例生成指南
   - `docs/RANDOM_ACCESS_ISSUE_ANALYSIS.md` - 随机读取问题分析
   - `docs/AUTOSA_SPACETIME_ANALYSIS.md` - Spacetime 参数分析

### 下一步行动

1. ⚠️ **运行生成脚本**（需要 AutoSA 环境）：
   - 生成不同 spacetime 的 MM 用例
   - 生成其他 Kernel 类型的用例

2. ⚠️ **检查随机读取问题**：
   - 对生成的 TTMc 和 CNN 代码进行检查
   - 如果发现问题，应用修复方法

3. ⚠️ **验证和测试**：
   - 验证生成的代码功能正确性
   - 测试性能提升（如果有修复）

---

## 相关文件

- **生成脚本**: 
  - `scripts/generate_autosa_tests.sh`
  - `scripts/generate_other_kernels.sh`
- **检查脚本**: 
  - `scripts/check_random_access.sh`
- **文档**: 
  - `docs/AUTOSA_TEST_GENERATION_GUIDE.md`
  - `docs/RANDOM_ACCESS_ISSUE_ANALYSIS.md`
  - `docs/AUTOSA_SPACETIME_ANALYSIS.md`
  - `docs/THREE_QUESTIONS_SOLUTION.md`（本文档）

