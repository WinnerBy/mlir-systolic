# AutoSA 测试用例生成指南

> **最后更新**: 2024-12  
> **目的**: 提供 AutoSA 测试用例生成脚本和参数配置

---

## 1. 问题 1：生成不同 Spacetime 和参数的测试用例

### 1.1 生成脚本

创建脚本 `scripts/generate_autosa_tests.sh`：

```bash
#!/bin/bash
# generate_autosa_tests.sh - 生成 AutoSA 测试用例

set -e

AUTOSA_ROOT="${AUTOSA_ROOT:-/home/user/work/AutoSA}"
OUTPUT_DIR="${OUTPUT_DIR:-./autosa_reference_samples}"

mkdir -p "$OUTPUT_DIR"

# 矩阵乘法（MM）测试用例
generate_mm_tests() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/mm/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/mm/simd_info.json"
    
    # Spacetime=0: [i] - 1D
    echo "Generating MM spacetime=0..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st0_I32_J32_K32_ap32_lat8_simd2" \
        --sa-sizes="{kernel[]->space_time[0];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls
    
    # Spacetime=1: [j] - 1D
    echo "Generating MM spacetime=1..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st1_I32_J32_K32_ap32_lat8_simd2" \
        --sa-sizes="{kernel[]->space_time[1];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls
    
    # Spacetime=2: [k] - 1D (需要 reduction)
    echo "Generating MM spacetime=2..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st2_I32_J32_K32_ap4_lat2_simd2" \
        --sa-sizes="{kernel[]->space_time[2];kernel[]->array_part[4,32,32];kernel[]->latency[2,2];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction
    
    # Spacetime=3: [i,j] - 2D (已有)
    echo "Generating MM spacetime=3..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st3_I32_J32_K32_ap16_lat8_simd2" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls
    
    # Spacetime=4: [i,k] - 2D (需要 reduction)
    echo "Generating MM spacetime=4..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st4_I32_J32_K32_ap32_lat16_simd2" \
        --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction
    
    # Spacetime=5: [j,k] - 2D (需要 reduction)
    echo "Generating MM spacetime=5..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st5_I32_J32_K32_ap32_lat16_simd2" \
        --sa-sizes="{kernel[]->space_time[5];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction
}

# 多维参数测试用例
generate_mm_multidim_tests() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/mm/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/mm/simd_info.json"
    
    # 不同 array_part
    echo "Generating MM with different array_part..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[260,256,512];kernel[]->latency[20,16];kernel[]->simd[8]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls
    
    # 不同 latency
    echo "Generating MM with different latency..."
    ./autosa "$kernel_file" \
        --config="$AUTOSA_ROOT/autosa_config/autosa_config.json" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap32_lat11_32_simd64" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[32,32,32];kernel[]->latency[11,32];kernel[]->simd[64]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls
}

# 主函数
main() {
    echo "=========================================="
    echo "AutoSA 测试用例生成脚本"
    echo "=========================================="
    echo ""
    
    cd "$AUTOSA_ROOT"
    
    # 检查 AutoSA 是否存在
    if [ ! -f "./autosa" ]; then
        echo "ERROR: AutoSA executable not found at $AUTOSA_ROOT/autosa"
        exit 1
    fi
    
    # 生成测试用例
    generate_mm_tests
    generate_mm_multidim_tests
    
    echo ""
    echo "=========================================="
    echo "测试用例生成完成"
    echo "输出目录: $OUTPUT_DIR"
    echo "=========================================="
}

main "$@"
```

### 1.2 参数配置表

#### 矩阵乘法（MM）不同 Spacetime 参数

| Spacetime | 空间循环 | array_part | latency | simd | 特殊参数 |
|-----------|----------|------------|---------|------|----------|
| **0** | `[i]` | `[32,32,32]` | `[8,8]` | `[2]` | - |
| **1** | `[j]` | `[32,32,32]` | `[8,8]` | `[2]` | - |
| **2** | `[k]` | `[4,32,32]` | `[2,2]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |
| **3** | `[i,j]` | `[16,16,16]` | `[8,8]` | `[2]` | - |
| **4** | `[i,k]` | `[32,4,32]` | `[16,16]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |
| **5** | `[j,k]` | `[32,4,32]` | `[16,16]` | `[2]` | `--local-reduce --reduce-op="+" --simd-touch-space --array-contraction` |

#### 多维参数测试用例

| 测试用例 | array_part | latency | simd | 说明 |
|----------|------------|---------|------|------|
| **mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8** | `[260,256,512]` | `[20,16]` | `[8]` | 不同 array_part 和 latency |
| **mm_st3_I64_J64_K64_ap32_lat11_32_simd64** | `[32,32,32]` | `[11,32]` | `[64]` | 不同 latency 和大 SIMD |

---

## 2. 问题 2：其他 Kernel 类型验证

### 2.1 AutoSA 支持的 Kernel 类型

| Kernel 类型 | 路径 | 说明 | 复杂度 |
|-------------|------|------|--------|
| **MM** | `autosa_tests/mm/` | 矩阵乘法 | 简单（3 个循环） |
| **CNN** | `autosa_tests/cnn/` | 卷积神经网络 | 中等（5 个循环） |
| **LU** | `autosa_tests/lu/` | LU 分解 | 复杂（需要特殊处理） |
| **MTTKRP** | `autosa_tests/large/mttkrp/` | 张量矩阵乘法 | 复杂（4 个循环，3D 数组） |
| **TTMc** | `autosa_tests/large/ttmc/` | 张量链乘法 | 复杂（5 个循环，3D 数组） |
| **DNN Ops** | `autosa_tests/dnn_ops/` | 深度神经网络操作 | 中等 |

### 2.2 推荐验证用例

#### 优先级 1：简单用例（推荐先验证）

1. **CNN（卷积神经网络）**
   - **路径**: `autosa_tests/cnn/kernel.c`
   - **特点**: 5 个循环（o, r, c, i, p, q），4D 数组
   - **示例命令**:
   ```bash
   ./autosa ./autosa_tests/cnn/kernel.c \
       --config=./autosa_config/autosa_config.json \
       --target=autosa_hls_c \
       --output-dir=./output \
       --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}" \
       --simd-info=./autosa_tests/cnn/simd_info.json \
       --host-serialize \
       --hls
   ```

2. **DNN Ops（深度神经网络操作）**
   - **路径**: `autosa_tests/dnn_ops/kernel.c`
   - **特点**: 多种操作（linear, relu, etc.）
   - **示例命令**:
   ```bash
   ./autosa ./autosa_tests/dnn_ops/kernel.c \
       --config=./autosa_config/autosa_config.json \
       --target=autosa_hls_c \
       --output-dir=./output \
       --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,4,4];kernel[]->simd[1,1,1,2]}" \
       --simd-info=./autosa_tests/dnn_ops/simd_info.json \
       --host-serialize \
       --hls
   ```

#### 优先级 2：复杂用例（需要特殊处理）

3. **MTTKRP（张量矩阵乘法）**
   - **路径**: `autosa_tests/large/mttkrp/kernel.c`
   - **特点**: 4 个循环（i, j, k, l），3D 数组 `A[I][K][L]`
   - **注意**: 存在随机读取问题（见问题 3）
   - **示例命令**:
   ```bash
   ./autosa ./autosa_tests/large/mttkrp/kernel.c \
       --config=./autosa_config/autosa_config.json \
       --target=autosa_hls_c \
       --output-dir=./output \
       --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}" \
       --simd-info=./autosa_tests/large/mttkrp/simd_info.json \
       --host-serialize \
       --hls
   ```

4. **TTMc（张量链乘法）**
   - **路径**: `autosa_tests/large/ttmc/kernel.c`
   - **特点**: 5 个循环（i, j, k, l, m），3D 数组 `A[I][L][M]`, `D[I][J][K]`
   - **注意**: 可能存在随机读取问题
   - **示例命令**:
   ```bash
   ./autosa ./autosa_tests/large/ttmc/kernel.c \
       --config=./autosa_config/autosa_config.json \
       --target=autosa_hls_c \
       --output-dir=./output \
       --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}" \
       --simd-info=./autosa_tests/large/ttmc/simd_info.json \
       --host-serialize \
       --hls
   ```

5. **LU（LU 分解）**
   - **路径**: `autosa_tests/lu/kernel.c`
   - **特点**: 需要特殊参数（`--use-cplusplus-template`, `--no-reschedule`）
   - **示例命令**:
   ```bash
   ./autosa ./autosa_tests/lu/kernel.c \
       --config=./autosa_config/autosa_config.json \
       --target=autosa_hls_c \
       --output-dir=./output \
       --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[-1,-1,-1];kernel[]->latency[]}" \
       --simd-info=./autosa_tests/lu/simd_info.json \
       --use-cplusplus-template \
       --no-reschedule \
       --hls
   ```

### 2.3 验证脚本

创建脚本 `scripts/generate_other_kernels.sh`：

```bash
#!/bin/bash
# generate_other_kernels.sh - 生成其他 Kernel 类型的测试用例

set -e

AUTOSA_ROOT="${AUTOSA_ROOT:-/home/user/work/AutoSA}"
OUTPUT_DIR="${OUTPUT_DIR:-./autosa_reference_samples}"

mkdir -p "$OUTPUT_DIR"

cd "$AUTOSA_ROOT"

# CNN
echo "Generating CNN..."
./autosa ./autosa_tests/cnn/kernel.c \
    --config=./autosa_config/autosa_config.json \
    --target=autosa_hls_c \
    --output-dir="$OUTPUT_DIR/cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2" \
    --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}" \
    --simd-info=./autosa_tests/cnn/simd_info.json \
    --host-serialize \
    --hls

# DNN Ops
echo "Generating DNN Ops..."
./autosa ./autosa_tests/dnn_ops/kernel.c \
    --config=./autosa_config/autosa_config.json \
    --target=autosa_hls_c \
    --output-dir="$OUTPUT_DIR/dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2" \
    --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,4,4];kernel[]->simd[1,1,1,2]}" \
    --simd-info=./autosa_tests/dnn_ops/simd_info.json \
    --host-serialize \
    --hls

# MTTKRP
echo "Generating MTTKRP..."
./autosa ./autosa_tests/large/mttkrp/kernel.c \
    --config=./autosa_config/autosa_config.json \
    --target=autosa_hls_c \
    --output-dir="$OUTPUT_DIR/mttkrp_st3_ap128_128_2_lat16_8_simd8_1" \
    --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}" \
    --simd-info=./autosa_tests/large/mttkrp/simd_info.json \
    --host-serialize \
    --hls

# TTMc
echo "Generating TTMc..."
./autosa ./autosa_tests/large/ttmc/kernel.c \
    --config=./autosa_config/autosa_config.json \
    --target=autosa_hls_c \
    --output-dir="$OUTPUT_DIR/ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1" \
    --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}" \
    --simd-info=./autosa_tests/large/ttmc/simd_info.json \
    --host-serialize \
    --hls

echo "All kernels generated!"
```

---

## 3. 问题 3：随机读取问题分析

### 3.1 MTTKRP 随机读取问题

#### 问题描述

**原始代码**（`kernel_opencl.cpp:77`）：
```cpp
in_data = local_A[c8][c5][4 * c6 / 16];
```

**问题**：
- `4 * c6 / 16` 是一个**非线性索引表达式**
- 当 `c6` 从 0 到 15 变化时，`4 * c6 / 16` 的值是：`0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3`
- 这导致**非顺序访问模式**，在 HLS 中会导致随机读取，影响性能

**优化后代码**（`kernel_opencl_opt_shuffle_db.cpp:77`）：
```cpp
in_data = local_A[c5][4 * c6 / 16][c8];
```

**优化**：
- 通过**重排数组维度**（从 `[8][2][4]` 到 `[2][4][8]`）
- 将非线性索引移到中间维度
- 在写入时进行重排（`inter_trans` 中：`local_A[c5][c6][c4]`）
- 在读取时使用重排后的布局（`intra_trans` 中：`local_A[c5][4 * c6 / 16][c8]`）

#### 问题特征

**随机读取问题的特征**：
1. **非线性索引表达式**：如 `4 * c6 / 16`, `c6 / 4`, `c6 % 4` 等
2. **出现在 local buffer 访问中**：`local_A[...][...][...]`
3. **在 IO_L2_in_intra_trans 中**：数据解包时使用

### 3.2 其他 Kernel 的随机读取问题检查

#### 检查方法

1. **查找模式**：
   ```bash
   grep -n "local_.*\[.*\]\[.*\]\[.*\]" generated_code.cpp | grep -E "\[.*\*.*\]|\[.*/.*\]|\[.*%.*\]"
   ```

2. **检查 IO_L2_in_intra_trans 函数**：
   - 查找 `local_A[...][...][...]` 或 `local_B[...][...][...]` 的访问
   - 检查索引表达式中是否有乘法、除法、取模运算

#### 可能存在的问题

| Kernel 类型 | 数组维度 | 可能的问题 | 检查方法 |
|-------------|----------|------------|----------|
| **MTTKRP** | `A[I][K][L]` | ✅ **已确认** | `local_A[c8][c5][4 * c6 / 16]` |
| **TTMc** | `A[I][L][M]`, `D[I][J][K]` | ⚠️ **可能** | 需要检查生成的代码 |
| **CNN** | `cin[R+K-1][C+K-1][I]`, `w[O][K][K][I]` | ⚠️ **可能** | 需要检查生成的代码 |
| **LU** | `A[N][N]` | ❌ **不太可能** | 2D 数组，通常顺序访问 |
| **MM** | `A[I][K]`, `B[K][J]`, `C[I][J]` | ❌ **不太可能** | 2D 数组，通常顺序访问 |

### 3.3 检查脚本

创建脚本 `scripts/check_random_access.sh`：

```bash
#!/bin/bash
# check_random_access.sh - 检查生成的代码中的随机读取问题

set -e

# 检查文件
check_file() {
    local file=$1
    local kernel_name=$2
    
    echo "Checking $kernel_name: $file"
    
    # 查找 local buffer 访问中的非线性索引
    local issues=$(grep -n "local_.*\[.*\]\[.*\]\[.*\]" "$file" 2>/dev/null | \
        grep -E "\[.*\*.*\]|\[.*/.*\]|\[.*%.*\]" || true)
    
    if [ -n "$issues" ]; then
        echo "  ⚠️  Found potential random access issues:"
        echo "$issues" | sed 's/^/    /'
        return 1
    else
        echo "  ✅ No random access issues found"
        return 0
    fi
}

# 检查目录中的所有 kernel 文件
check_directory() {
    local dir=$1
    
    if [ ! -d "$dir" ]; then
        echo "ERROR: Directory not found: $dir"
        return 1
    fi
    
    # 查找所有 kernel 文件
    local kernel_files=$(find "$dir" -name "kernel*.cpp" -o -name "kernel*.c" 2>/dev/null)
    
    if [ -z "$kernel_files" ]; then
        echo "No kernel files found in $dir"
        return 1
    fi
    
    local total=0
    local issues=0
    
    for file in $kernel_files; do
        ((total++))
        local kernel_name=$(basename "$(dirname "$file")")
        if ! check_file "$file" "$kernel_name"; then
            ((issues++))
        fi
    done
    
    echo ""
    echo "Summary: $issues/$total files have potential random access issues"
    return $issues
}

# 主函数
main() {
    if [ $# -eq 0 ]; then
        echo "Usage: $0 <directory>"
        echo "Example: $0 /path/to/autosa/output/src"
        exit 1
    fi
    
    check_directory "$1"
}

main "$@"
```

### 3.4 修复建议

如果发现随机读取问题，可以考虑以下修复方法：

1. **数组维度重排**（如 MTTKRP 优化）：
   - 将非线性索引移到中间维度
   - 在写入时进行重排
   - 在读取时使用重排后的布局

2. **数据布局转换**：
   - 在 IO_L2_in_inter_trans 中写入时进行转换
   - 在 IO_L2_in_intra_trans 中读取时使用转换后的布局

3. **循环变换**：
   - 调整循环顺序，使访问模式更顺序化

---

## 4. 总结

### 4.1 测试用例生成优先级

| 优先级 | 任务 | 状态 |
|--------|------|------|
| **P0** | 生成不同 spacetime 的 MM 用例（0, 1, 2, 4, 5） | ⚠️ 待完成 |
| **P0** | 生成多维参数的 MM 用例 | ⚠️ 待完成 |
| **P1** | 生成 CNN 用例 | ⚠️ 待完成 |
| **P1** | 生成 DNN Ops 用例 | ⚠️ 待完成 |
| **P2** | 生成 MTTKRP 用例（注意随机读取） | ⚠️ 待完成 |
| **P2** | 生成 TTMc 用例（检查随机读取） | ⚠️ 待完成 |
| **P3** | 生成 LU 用例（需要特殊处理） | ⚠️ 待完成 |

### 4.2 随机读取问题检查优先级

| 优先级 | Kernel | 状态 |
|--------|--------|------|
| **P0** | MTTKRP | ✅ 已确认并修复 |
| **P1** | TTMc | ⚠️ 需要检查 |
| **P1** | CNN | ⚠️ 需要检查 |
| **P2** | 其他 3D+ 数组的 Kernel | ⚠️ 需要检查 |

---

## 5. 相关文件

- **生成脚本**: `scripts/generate_autosa_tests.sh`
- **其他 Kernel 脚本**: `scripts/generate_other_kernels.sh`
- **随机读取检查脚本**: `scripts/check_random_access.sh`
- **分析文档**: `docs/AUTOSA_SPACETIME_ANALYSIS.md`

