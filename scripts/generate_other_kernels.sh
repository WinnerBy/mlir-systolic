#!/bin/bash
# generate_other_kernels.sh - 生成其他 Kernel 类型的测试用例

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

# 生成 CNN 测试用例
generate_cnn() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/cnn/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/cnn/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating CNN...${NC}"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/cnn_st4_ap8_8_4_8_lat4_2_4_simd1_1_1_2" \
        --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,2,4];kernel[]->simd[1,1,1,2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/cnn.log" || echo -e "${RED}Failed${NC}"
}

# 生成 DNN Ops 测试用例
generate_dnn_ops() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/dnn_ops/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/dnn_ops/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating DNN Ops...${NC}"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/dnn_ops_st4_ap8_8_4_8_lat4_4_4_simd1_1_1_2" \
        --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[8,8,4,8];kernel[]->latency[4,4,4];kernel[]->simd[1,1,1,2]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/dnn_ops.log" || echo -e "${RED}Failed${NC}"
}

# 生成 MTTKRP 测试用例
generate_mttkrp() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/large/mttkrp/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/large/mttkrp/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating MTTKRP (注意：可能存在随机读取问题)...${NC}"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/mttkrp_st3_ap128_128_2_lat16_8_simd8_1" \
        --sa-sizes="{kernel[]->space_time[3];kernel[]->array_part[128,128,2];kernel[]->latency[16,8];kernel[]->simd[8,1]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/mttkrp.log" || echo -e "${RED}Failed${NC}"
}

# 生成 TTMc 测试用例
generate_ttmc() {
    local kernel_file="$AUTOSA_ROOT/autosa_tests/large/ttmc/kernel.c"
    local simd_info="$AUTOSA_ROOT/autosa_tests/large/ttmc/simd_info.json"
    local config_file="$AUTOSA_ROOT/autosa_config/autosa_config.json"
    
    echo -e "${YELLOW}Generating TTMc (注意：可能存在随机读取问题)...${NC}"
    "$AUTOSA_ROOT/autosa" "$kernel_file" \
        --config="$config_file" \
        --target=autosa_hls_c \
        --output-dir="$OUTPUT_DIR/ttmc_st4_ap16_64_16_32_lat1_8_8_simd8_1" \
        --sa-sizes="{kernel[]->space_time[4];kernel[]->array_part[16,64,16,32];kernel[]->latency[1,8,8];kernel[]->simd[8,1]}" \
        --simd-info="$simd_info" \
        --host-serialize \
        --hls 2>&1 | tee "$OUTPUT_DIR/ttmc.log" || echo -e "${RED}Failed${NC}"
}

# 主函数
main() {
    echo "=========================================="
    echo "AutoSA 其他 Kernel 生成脚本"
    echo "=========================================="
    echo ""
    echo "AUTOSA_ROOT: $AUTOSA_ROOT"
    echo "OUTPUT_DIR: $OUTPUT_DIR"
    echo ""
    
    check_autosa
    
    cd "$AUTOSA_ROOT"
    
    # 生成测试用例
    generate_cnn
    generate_dnn_ops
    generate_mttkrp
    generate_ttmc
    
    echo ""
    echo "=========================================="
    echo -e "${GREEN}所有 Kernel 生成完成${NC}"
    echo "输出目录: $OUTPUT_DIR"
    echo "=========================================="
}

main "$@"

