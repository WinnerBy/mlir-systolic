#!/bin/bash
# organize_reference_samples.sh - 整理 AutoSA 生成的 samples 到 test/reference-samples 目录

set -e

# 颜色输出
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REF_DIR="${PROJECT_ROOT}/test/reference-samples"
SOURCE_DIR="${PROJECT_ROOT}/autosa_reference_samples/collected_hls_files"

echo "=========================================="
echo "整理 AutoSA Reference Samples"
echo "=========================================="
echo ""

# 确保目标目录存在
mkdir -p "$REF_DIR"

# 计数器
copied_count=0
skipped_count=0

# 安全的计数器递增函数
increment_copied() {
    copied_count=$((copied_count + 1))
}

increment_skipped() {
    skipped_count=$((skipped_count + 1))
}

# 排除的配置
EXCLUDED_CONFIGS=(
    "mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8"  # array part 比问题规模大，没有实际意义
    "mm_st3_I64_J64_K64_ap32_lat11_32_simd64"          # 代码生成失败，所有文件都为空
    "dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2"      # 代码生成失败，所有文件都为空
)

# 检查是否在排除列表中
is_excluded() {
    local config=$1
    for excluded in "${EXCLUDED_CONFIGS[@]}"; do
        if [ "$config" == "$excluded" ]; then
            return 0
        fi
    done
    return 1
}

# 函数：复制 kernel 文件
copy_kernel_file() {
    local source_dir=$1
    local target_name=$2
    
    # 查找 kernel 文件（优先使用 kernel_kernel.cpp）
    local kernel_file=""
    if [ -f "$source_dir/kernel_kernel.cpp" ]; then
        kernel_file="$source_dir/kernel_kernel.cpp"
        # 检查文件是否为空
        local file_size=$(wc -l < "$kernel_file" 2>/dev/null || echo "0")
        if [ "$file_size" -eq 0 ]; then
            echo -e "${YELLOW}警告: $source_dir/kernel_kernel.cpp 是空文件，跳过${NC}"
            increment_skipped
            return 1
        fi
    elif [ -f "$source_dir/kernel_kernel_modules.cpp" ]; then
        # 如果只有 kernel_kernel_modules.cpp，检查是否为空
        local file_size=$(wc -l < "$source_dir/kernel_kernel_modules.cpp" 2>/dev/null || echo "0")
        if [ "$file_size" -eq 0 ]; then
            echo -e "${YELLOW}警告: $source_dir/kernel_kernel_modules.cpp 是空文件，跳过${NC}"
            increment_skipped
            return 1
        fi
        kernel_file="$source_dir/kernel_kernel_modules.cpp"
    else
        echo -e "${YELLOW}警告: 在 $source_dir 中未找到 kernel 文件，跳过${NC}"
        increment_skipped
        return 1
    fi
    
    local target_file="$REF_DIR/$target_name"
    
    # 如果文件已存在，询问是否覆盖（这里直接覆盖，因为用户要求重新整理）
    cp "$kernel_file" "$target_file"
    echo -e "${GREEN}✓ 复制: $target_name ($(wc -l < "$kernel_file") 行)${NC}"
    increment_copied
}

# 整理所有 samples
echo "整理 AutoSA samples..."
echo ""

if [ -d "$SOURCE_DIR" ]; then
    for sample_dir in "$SOURCE_DIR"/*; do
        if [ ! -d "$sample_dir" ]; then
            continue
        fi
        
        dir_name=$(basename "$sample_dir")
        
        # 检查是否在排除列表中
        if is_excluded "$dir_name"; then
            echo -e "${YELLOW}跳过排除的配置: $dir_name${NC}"
            increment_skipped
            continue
        fi
        
        # 生成目标文件名
        # 格式: {kernel_type}_st{ST}_..._kernel.cpp
        target_name="${dir_name}_kernel.cpp"
        
        copy_kernel_file "$sample_dir" "$target_name"
    done
else
    echo -e "${YELLOW}警告: samples 目录不存在: $SOURCE_DIR${NC}"
    exit 1
fi

echo ""
echo "=========================================="
echo "整理完成"
echo "=========================================="
echo "复制文件数: $copied_count"
echo "跳过文件数: $skipped_count"
echo "目标目录: $REF_DIR"
echo ""
