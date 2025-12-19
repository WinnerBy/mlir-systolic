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

**生成的用例**：
- `mm_st0_*` - Spacetime=0: [i] - 1D
- `mm_st1_*` - Spacetime=1: [j] - 1D
- `mm_st2_*` - Spacetime=2: [k] - 1D (需要 reduction)
- `mm_st3_*` - Spacetime=3: [i,j] - 2D ✅ **当前支持**
- `mm_st4_*` - Spacetime=4: [i,k] - 2D (需要 reduction)
- `mm_st5_*` - Spacetime=5: [j,k] - 2D (需要 reduction)

### 1.2 其他 Kernel 类型

```bash
# 运行脚本
./scripts/generate_other_kernels.sh
```

**生成的用例**：
- `cnn_*` - 卷积神经网络
- `dnn_ops_*` - 深度神经网络操作
- `mttkrp_*` - 张量矩阵乘法（注意随机读取问题）
- `ttmc_*` - 张量链乘法（注意随机读取问题）

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

**可能存在的问题**：
- ✅ MTTKRP - 已确认并修复
- ⚠️ TTMc - 需要检查
- ⚠️ CNN - 需要检查

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

- **测试生成指南**: `docs/AUTOSA_TEST_GENERATION_GUIDE.md`
- **随机读取分析**: `docs/RANDOM_ACCESS_ISSUE_ANALYSIS.md`
- **Spacetime 分析**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`
- **三个问题解决方案**: `docs/THREE_QUESTIONS_SOLUTION.md`

---

## 5. 常用命令

```bash
# 生成所有测试用例
./scripts/generate_autosa_tests.sh
./scripts/generate_other_kernels.sh

# 检查随机读取问题
./scripts/check_random_access.sh /path/to/kernel.cpp

# 测试 mlir-systolic
./test/test_all_configs.sh
```

