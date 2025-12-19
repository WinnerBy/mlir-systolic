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

# 生成矩阵乘法（MM）不同 spacetime 的测试用例
generate_mm_spacetime_tests() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/mm/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/mm/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating MM spacetime tests...${NC}"
    
    # Spacetime=0: [i] - 1D
    echo "  Generating MM spacetime=0..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st0_I32_J32_K32_ap32_lat8_simd2" \
        --sa-sizes="{kernel[]->space_time[0];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st0.log" || echo -e "${RED}Failed${NC}"
    
    # Spacetime=1: [j] - 1D
    echo "  Generating MM spacetime=1..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st1_I32_J32_K32_ap32_lat8_simd2" \
        --sa-sizes="{kernel[]->space_time[1];kernel[]->array_part[32,32,32];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st1.log" || echo -e "${RED}Failed${NC}"
    
    # Spacetime=2: [k] - 1D (需要 reduction)
    echo "  Generating MM spacetime=2..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st2_I32_J32_K32_ap4_lat2_simd2" \
        --sa-sizes="{kernel[]->space_time[2];kernel[]->array_part[4,32,32];kernel[]->latency[2,2];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction 2>&1 | tee "$OUTPUT_DIR/mm_st2.log" || echo -e "${RED}Failed${NC}"
    
    # Spacetime=3: [i,j] - 2D (已有，但生成一个示例)
    echo "  Generating MM spacetime=3..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st3_I32_J32_K32_ap16_lat8_simd2" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[16,16,16];kernel[]->latency[8,8];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_st3.log" || echo -e "${RED}Failed${NC}"
    
    # Spacetime=4: [i,k] - 2D (需要 reduction)
    echo "  Generating MM spacetime=4..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st4_I32_J32_K32_ap32_lat16_simd2" \
        --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction 2>&1 | tee "$OUTPUT_DIR/mm_st4.log" || echo -e "${RED}Failed${NC}"
    
    # Spacetime=5: [j,k] - 2D (需要 reduction)
    echo "  Generating MM spacetime=5..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st5_I32_J32_K32_ap32_lat16_simd2" \
        --sa-sizes="{kernel[]->space_time[5];kernel[]->array_part[32,4,32];kernel[]->latency[16,16];kernel[]->simd[2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls \
        --local-reduce \
        --reduce-op="+" \
        --simd-touch-space \
        --array-contraction 2>&1 | tee "$OUTPUT_DIR/mm_st5.log" || echo -e "${RED}Failed${NC}"
}

# 生成多维参数的测试用例
generate_mm_multidim_tests() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/mm/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/mm/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating MM multidim parameter tests...${NC}"
    
    # 不同 array_part
    echo "  Generating MM with different array_part..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap260_256_512_lat20_16_simd8" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[260,256,512];kernel[]->latency[20,16];kernel[]->simd[8]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_multidim1.log" || echo -e "${RED}Failed${NC}"
    
    # 不同 latency
    echo "  Generating MM with different latency..."
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mm_st3_I64_J64_K64_ap32_lat11_32_simd64" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[32,32,32];kernel[]->latency[11,32];kernel[]->simd[64]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mm_multidim2.log" || echo -e "${RED}Failed${NC}"
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
    
    # 生成测试用例
    generate_mm_spacetime_tests
    generate_mm_multidim_tests
    
    echo ""
    echo "=========================================="
    echo -e "${GREEN}测试用例生成完成${NC}"
    echo "输出目录: $OUTPUT_DIR"
    echo "=========================================="
}

main "$@"

