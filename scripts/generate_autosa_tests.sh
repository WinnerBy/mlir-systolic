#!/bin/bash
# generate_autosa_tests.sh - 生成 AutoSA 测试用例（不同 spacetime 和参数）

set -e

# 配置
AUTOSA_ROOT="${AUTOSA_ROOT:-/home/user/work/AutoSA}"
OUTPUT_DIR="${OUTPUT_DIR:-./autosa_reference_samples}"

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

mkdir -p "$OUTPUT_DIR"

# 检查 AutoSA 是否存在
check_autosa() {
    if [ ! -f "$AUTOSA_ROOT/autosa" ]; then
        echo -e "${RED}ERROR: AutoSA executable not found at $AUTOSA_ROOT/autosa${NC}"
        echo "Please set AUTOSA_ROOT environment variable or install AutoSA"
        exit 1
    fi
}

# 创建 AutoSA 要求的目录结构
create_autosa_dirs() {
    local output_dir=$1
    mkdir -p "$output_dir/src"
    mkdir -p "$output_dir/latency_est"
    mkdir -p "$output_dir/resource_est"
    mkdir -p "$output_dir/tuning"
}

# 收集 HLS C 文件到统一目录
collect_hls_files() {
    local output_dir=$1
    local test_name=$2
    local collected_dir="$OUTPUT_DIR/collected_hls_files"
    
    mkdir -p "$collected_dir"
    
    # 查找所有 .cpp 和 .h 文件
    local hls_files=$(find "$output_dir/src" -name "*.cpp" -o -name "*.h" 2>/dev/null)
    
    if [ -n "$hls_files" ]; then
        # 创建测试用例子目录
        local test_dir="$collected_dir/$test_name"
        mkdir -p "$test_dir"
        
        # 拷贝文件
        for file in $hls_files; do
            if [ -f "$file" ]; then
                cp "$file" "$test_dir/"
            fi
        done
        
        echo "  Collected HLS files to: $test_dir"
    fi
}

# 临时修改 kernel.h 以设置问题规模
setup_kernel_size() {
    local kernel_h="$AUTOSA_ROOT/autosa_tests/mm/kernel.h"
    local I=$1
    local J=$2
    local K=$3
    
    # 备份原文件
    cp "$kernel_h" "$kernel_h.bak"
    
    # 修改定义
    sed -i "s/^#define I .*/#define I $I/" "$kernel_h"
    sed -i "s/^#define J .*/#define J $J/" "$kernel_h"
    sed -i "s/^#define K .*/#define K $K/" "$kernel_h"
}

# 恢复 kernel.h
restore_kernel_size() {
    local kernel_h="$AUTOSA_ROOT/autosa_tests/mm/kernel.h"
    if [ -f "$kernel_h.bak" ]; then
        mv "$kernel_h.bak" "$kernel_h"
    fi
}

# 生成矩阵乘法（MM）不同 spacetime 的测试用例
generate_mm_spacetime_tests() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/mm/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/mm/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating MM spacetime tests...${NC}"
    
    # Spacetime=0: [i] - 1D, I=32
    echo "  Generating MM spacetime=0 (I=32, J=32, K=32)..."
    setup_kernel_size 32 32 32
    local output_dir="$OUTPUT_DIR/mm_st0_I32_J32_K32_ap32_lat8_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[0];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st0.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st0_I32_J32_K32_ap32_lat8_simd2"
    
    # Spacetime=1: [j] - 1D, I=32
    echo "  Generating MM spacetime=1 (I=32, J=32, K=32)..."
    setup_kernel_size 32 32 32
    local output_dir="$OUTPUT_DIR/mm_st1_I32_J32_K32_ap32_lat8_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[1];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st1.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st1_I32_J32_K32_ap32_lat8_simd2"
    
    # Spacetime=2: [k] - 1D (需要 reduction), I=32
    echo "  Generating MM spacetime=2 (I=32, J=32, K=32)..."
    setup_kernel_size 32 32 32
    local output_dir="$OUTPUT_DIR/mm_st2_I32_J32_K32_ap4_lat2_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[2];kernel[]->array_part[4,32,32];kernel[]->latency[2,2];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction 2>&1 | tee "$OUTPUT_DIR/mm_st2.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st2_I32_J32_K32_ap4_lat2_simd2"
    
    # Spacetime=3: [i,j] - 2D, I=32
    echo "  Generating MM spacetime=3 (I=32, J=32, K=32)..."
    setup_kernel_size 32 32 32
    local output_dir="$OUTPUT_DIR/mm_st3_I32_J32_K32_ap16_lat8_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st3.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st3_I32_J32_K32_ap16_lat8_simd2"
    
    # Spacetime=4: [i,k] - 2D (需要 reduction), I=32
    echo "  Generating MM spacetime=4 (I=32, J=32, K=32)..."
    setup_kernel_size 32 32 32
    local output_dir="$OUTPUT_DIR/mm_st4_I32_J32_K32_ap32_lat16_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction 2>&1 | tee "$OUTPUT_DIR/mm_st4.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st4_I32_J32_K32_ap32_lat16_simd2"
    
    # Spacetime=5: [j,k] - 2D (需要 reduction), I=32
    echo "  Generating MM spacetime=5 (I=32, J=32, K=32)..."
    setup_kernel_size 32 32 32
    local output_dir="$OUTPUT_DIR/mm_st5_I32_J32_K32_ap32_lat16_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[5];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction 2>&1 | tee "$OUTPUT_DIR/mm_st5.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st5_I32_J32_K32_ap32_lat16_simd2"
    
    # Spacetime=3: [i,j] - 2D, I=64
    echo "  Generating MM spacetime=3 (I=64, J=64, K=64)..."
    setup_kernel_size 64 64 64
    local output_dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap16_lat8_simd2"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st3_I64.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st3_I64_J64_K64_ap16_lat8_simd2"
    
    # 恢复原始 kernel.h
    restore_kernel_size
}

# 生成多维参数的测试用例
generate_mm_multidim_tests() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/mm/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/mm/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating MM multidim parameter tests...${NC}"
    
    # 不同 array_part, I=64
    echo "  Generating MM with different array_part (I=64, J=64, K=64)..."
    setup_kernel_size 64 64 64
    local output_dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[260,256,512];kernel[]->latency[20,16];kernel[]->simd[8]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_multidim1.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8"
    
    # 不同 latency, I=64
    echo "  Generating MM with different latency (I=64, J=64, K=64)..."
    setup_kernel_size 64 64 64
    local output_dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap32_lat11_32_simd64"
    create_autosa_dirs "$output_dir"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$output_dir" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[32,32,32];kernel[]->latency[11,32];kernel[]->simd[64]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_multidim2.log" || echo -e "${RED}Failed${NC}"
    collect_hls_files "$output_dir" "mm_st3_I64_J64_K64_ap32_lat11_32_simd64"
    
    # 恢复原始 kernel.h
    restore_kernel_size
}

# 主函数
main() {
    echo "=========================================="
    echo "AutoSA 测试用例生成脚本"
    echo "=========================================="
    echo ""
    echo "AUTOSA_ROOT: $AUTOSA_ROOT"
    echo "OUTPUT_DIR: $OUTPUT_DIR"
    echo ""
    
    check_autosa
    
    # 设置退出时恢复 kernel.h
    trap restore_kernel_size EXIT
    
    # 生成测试用例
    generate_mm_spacetime_tests
    generate_mm_multidim_tests
    
    # 恢复 kernel.h
    restore_kernel_size
    
    echo ""
    echo "=========================================="
    echo -e "${GREEN}测试用例生成完成${NC}"
    echo "输出目录: $OUTPUT_DIR"
    echo "收集的 HLS 文件目录: $OUTPUT_DIR/collected_hls_files"
    echo "=========================================="
}

main "$@"

