# 快速参考指南

> **最后更新**: 2024-12  
> **目的**: 提供快速参考，方便查找关键信息

---

## 1. 生成 AutoSA 测试用例

### 1.1 不同 Spacetime 的 MM 用例

```bash
# 设置环境变量
export AUTOSA_ROOT=/home/user/work/AutoSA
export OUTPUT_DIR=./autosa_reference_samples

# 运行脚本
./scripts/generate_autosa_tests.sh
```

**输出说明**：
- 每个测试用例在 `OUTPUT_DIR/` 下创建独立目录，包含 AutoSA 要求的文件夹结构
- 所有 HLS C++ 文件自动收集到 `OUTPUT_DIR/collected_hls_files/` 目录，方便提取和对比
- **脚本会自动修改 kernel.h 以设置不同的问题规模（I, J, K）**

**生成的用例**：
- `mm_st0_I32_*` - Spacetime=0: [i] - 1D, I=32, J=32, K=32
- `mm_st1_I32_*` - Spacetime=1: [j] - 1D, I=32, J=32, K=32
- `mm_st2_I32_*` - Spacetime=2: [k] - 1D (需要 reduction), I=32, J=32, K=32
- `mm_st3_I32_*` - Spacetime=3: [i,j] - 2D, I=32, J=32, K=32 ✅ **当前支持**
- `mm_st3_I64_*` - Spacetime=3: [i,j] - 2D, I=64, J=64, K=64 ✅ **当前支持**
- `mm_st4_I32_*` - Spacetime=4: [i,k] - 2D (需要 reduction), I=32, J=32, K=32
- `mm_st5_I32_*` - Spacetime=5: [j,k] - 2D (需要 reduction), I=32, J=32, K=32

### 1.2 其他 Kernel 类型

**重要更新**：其他 kernel 类型的生成已合并到统一脚本 `scripts/generate_autosa_tests.sh`

**生成的用例**：
- `cnn_*` - 卷积神经网络（⚠️ 需要检查随机读取问题）✅
- `dnn_ops_*` - 深度神经网络操作（❌ 生成失败）
- `mttkrp_*` - 张量矩阵乘法（🔴 已确认随机读取问题）✅
- `ttmc_*` - 张量链乘法（🔴 高度可能存在随机读取问题）✅
- `ttm_*` - 张量矩阵乘法（🟡 可能存在随机读取问题）✅
- `lu_*` - LU 分解（🟡 可能存在随机读取问题）✅

---

## 2. 检查随机读取问题

```bash
# 检查单个文件
./scripts/check_random_access.sh /path/to/kernel.cpp

# 检查目录
./scripts/check_random_access.sh /path/to/autosa/output/src

# 详细分析 MTTKRP
./scripts/check_random_access.sh /path/to/mttkrp_kernel.cpp --analyze-mttkrp
```

**问题特征**：
- 非线性索引表达式：`4 * c6 / 16`, `c6 / 4`, `c6 % 4`
- 出现在 `local_*[...][...][...]` 访问中
- 在 `IO_L2_in_intra_trans` 函数中

**可能存在的问题**（基于理论分析）：
- 🔴 **MTTKRP** - 已确认并修复
- 🔴 **TTMc** - 高度可能存在（与 MTTKRP 相似）
- 🟡 **TTM** - 可能存在（与 TTMc 相似）
- 🟡 **CNN** - 需要检查（4 维张量 + 滑动窗口）
- 🟡 **LU** - 可能存在（3 维临时数组 + 不规则访问）
- 🟢 **DNN Ops** - 需要检查（取决于具体操作）

**详细分析**：参见 [随机读取问题分析](RANDOM_ACCESS_ANALYSIS.md)

---

## 3. 关键参数配置

### 3.1 矩阵乘法（MM）

| Spacetime | array_part | latency | simd | 特殊参数 |
|-----------|------------|---------|------|----------|
| 0 | `[32,32,32]` | `[8,8]` | `[2]` | - |
| 1 | `[32,32,32]` | `[8,8]` | `[2]` | - |
| 2 | `[4,32,32]` | `[2,2]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |
| 3 | `[16,16,16]` | `[8,8]` | `[2]` | - |
| 4 | `[32,4,32]` | `[16,16]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |
| 5 | `[32,4,32]` | `[16,16]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |

### 3.2 其他 Kernel

**CNN**:
```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}"
```

**MTTKRP**:
```bash
--sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}"
```

**TTMc**:
```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}"
```

---

## 4. 相关文档

- **Scripts 说明**: `docs/SCRIPTS.md` - 各个脚本的用途和使用方法
- **测试生成指南**: `docs/AUTOSA_TEST_GENERATION_GUIDE.md` - 参数配置参考
- **随机读取问题分析**: `docs/RANDOM_ACCESS_ANALYSIS.md` - 随机读取问题分析和修复方法
- **Spacetime 分析**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`
- **Reference Samples**: `docs/REFERENCE_SAMPLES.md` - Reference samples 说明

---

## 5. 常用命令

```bash
# 生成所有测试用例（统一脚本，包含所有 kernel 类型）
./scripts/generate_autosa_tests.sh

# 整理生成的 samples 到 test/reference-samples/
./scripts/organize_reference_samples.sh

# 检查随机读取问题
./scripts/check_random_access.sh /path/to/kernel.cpp

# 测试 mlir-systolic
./test/test_all_configs.sh
```

