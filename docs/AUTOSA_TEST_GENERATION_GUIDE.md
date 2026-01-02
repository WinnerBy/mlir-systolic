# AutoSA 测试用例生成指南

> **最后更新**: 2024-12  
> **目的**: AutoSA 测试用例参数配置参考

---

## 快速开始

**生成脚本**: `scripts/generate_autosa_tests.sh`

**使用方法**:
```bash
export AUTOSA_ROOT=/home/user/work/AutoSA
export OUTPUT_DIR=./autosa_reference_samples
./scripts/generate_autosa_tests.sh
```

**整理 samples**:
```bash
./scripts/organize_reference_samples.sh
```

详细说明请参考: `docs/SCRIPTS.md`

---

## 参数配置表

### 矩阵乘法（MM）不同 Spacetime 参数

| Spacetime | 空间循环 | array_part | latency | simd | 特殊参数 |
|-----------|----------|------------|---------|------|----------|
| **0** | `[i]` | `[32,32,32]` | `[8,8]` | `[2]` | - |
| **1** | `[j]` | `[32,32,32]` | `[8,8]` | `[2]` | - |
| **2** | `[k]` | `[4,32,32]` | `[2,2]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |
| **3** | `[i,j]` | `[16,16,16]` | `[8,8]` | `[2]` | - |
| **4** | `[i,k]` | `[32,4,32]` | `[16,16]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |
| **5** | `[j,k]` | `[32,4,32]` | `[16,16]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |

### 其他 Kernel 类型参数

#### CNN
```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}"
```

#### MTTKRP
```bash
--sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}"
```

#### TTMc
```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}"
```

#### TTM
```bash
--sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}"
```

#### LU
```bash
--sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[-1,-1,-1];kernel[]->latency[]}"
--use-cplusplus-template --no-reschedule --live-range-reordering
```

---

## 重要说明

1. **已删除 `--hls` 参数**: 生成的 HLS C++ 文件自包含头文件信息
2. **自动收集**: 所有 HLS 文件自动收集到 `collected_hls_files/` 目录
3. **整理 samples**: 使用 `organize_reference_samples.sh` 整理到 `test/reference-samples/`

---

## 相关文档

- **Scripts 说明**: `docs/SCRIPTS.md`
- **Reference Samples**: `test/reference-samples/README.md`
- **Spacetime 分析**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`
