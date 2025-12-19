#!/bin/bash
# test_all_configs.sh - 测试所有配置，生成总结报告

set -e

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 项目根目录
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_DIR="${PROJECT_ROOT}/test"
REF_DIR="${TEST_DIR}/reference-samples"
OUTPUT_DIR="${TEST_DIR}/output"

# 检查工具是否存在
SYSTOLIC_OPT="${BUILD_DIR}/bin/systolic-opt"
SYSTOLIC_TRANSLATE="${BUILD_DIR}/bin/systolic-translate"

if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo -e "${RED}ERROR: systolic-opt not found${NC}"
    exit 1
fi

if [ ! -f "$SYSTOLIC_TRANSLATE" ]; then
    echo -e "${RED}ERROR: systolic-translate not found${NC}"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

# 所有可用的配置（从参考文件名提取）
all_configs=(
    "I32_J32_K32_ap8_lat4_simd1"
    "I32_J32_K32_ap8_lat4_simd2"
    "I64_J64_K64_ap16_lat4_simd1"
    "I64_J64_K64_ap16_lat8_simd1"
    "I64_J64_K64_ap16_lat8_simd2"
    "I64_J64_K64_ap16_lat8_simd4"
    "I64_J64_K64_ap32_lat8_simd1"
    "I64_J64_K64_ap32_lat16_simd1"
    "I128_J128_K128_ap32_lat16_simd1"
    "I128_J128_K128_ap32_lat16_simd2"
    "I128_J128_K128_ap64_lat16_simd1"
)

# 从配置字符串解析参数
parse_config() {
    local config=$1
    I=$(echo "$config" | grep -oP 'I\K\d+')
    J=$(echo "$config" | grep -oP 'J\K\d+')
    K=$(echo "$config" | grep -oP 'K\K\d+')
    AP=$(echo "$config" | grep -oP 'ap\K\d+')
    LAT=$(echo "$config" | grep -oP 'lat\K\d+')
    SIMD=$(echo "$config" | grep -oP 'simd\K\d+')
    echo "$I $J $K $AP $LAT $SIMD"
}

# 生成测试用的 MLIR 文件
generate_mlir_input() {
    local I=$1
    local J=$2
    local K=$3
    local mlir_file=$4
    
    cat > "$mlir_file" <<EOF
module {
  func.func @matmul(%A: memref<${I}x${K}xf32>, %B: memref<${K}x${J}xf32>, %C: memref<${I}x${J}xf32>) {
    affine.for %i = 0 to ${I} {
      affine.for %j = 0 to ${J} {
        affine.for %k = 0 to ${K} {
          %a = affine.load %A[%i, %k] : memref<${I}x${K}xf32>
          %b = affine.load %B[%k, %j] : memref<${K}x${J}xf32>
          %c = affine.load %C[%i, %j] : memref<${I}x${J}xf32>
          %prod = arith.mulf %a, %b : f32
          %sum = arith.addf %c, %prod : f32
          affine.store %sum, %C[%i, %j] : memref<${I}x${J}xf32>
        }
      }
    }
    return
  }
}
EOF
}

# 测试单个配置
test_config() {
    local config=$1
    local ref_file="${REF_DIR}/mm_st3_${config}_kernel.cpp"
    
    # 检查参考文件是否存在
    if [ ! -f "$ref_file" ]; then
        echo -e "${YELLOW}SKIP${NC}: $config (reference file not found)"
        return 2
    fi
    
    # 解析参数
    read I J K AP LAT SIMD <<< "$(parse_config "$config")"
    
    # 生成 MLIR 输入
    local mlir_input="${OUTPUT_DIR}/test_${config}.mlir"
    generate_mlir_input "$I" "$J" "$K" "$mlir_input"
    
    # 运行 systolic-opt
    local step1_mlir="${OUTPUT_DIR}/test_${config}_step1.mlir"
    if ! "$SYSTOLIC_OPT" "$mlir_input" --systolic-transform -o "$step1_mlir" >/dev/null 2>&1; then
        echo -e "${RED}FAIL${NC}: $config (systolic-opt failed)"
        return 1
    fi
    
    # 运行 systolic-translate
    local output_cpp="${OUTPUT_DIR}/test_${config}_kernel.cpp"
    if ! "$SYSTOLIC_TRANSLATE" "$step1_mlir" \
        --array-part="$AP" \
        --latency="$LAT" \
        --simd="$SIMD" \
        --size="$I" \
        -o "$output_cpp" >/dev/null 2>&1; then
        echo -e "${RED}FAIL${NC}: $config (systolic-translate failed)"
        return 1
    fi
    
    # 检查输出文件
    if [ ! -f "$output_cpp" ]; then
        echo -e "${RED}FAIL${NC}: $config (output file not generated)"
        return 1
    fi
    
    # 对比统计
    our_lines=$(wc -l < "$output_cpp")
    ref_lines=$(wc -l < "$ref_file")
    our_pipeline=$(grep -c "PIPELINE" "$output_cpp" || echo "0")
    ref_pipeline=$(grep -c "PIPELINE" "$ref_file" || echo "0")
    
    # 检查关键结构
    has_kernel0=$(grep -q "void kernel0" "$output_cpp" && echo "1" || echo "0")
    has_pe=$(grep -q "void PE\|void PE_wrapper" "$output_cpp" && echo "1" || echo "0")
    has_io=$(grep -q "IO_L\|io_L" "$output_cpp" && echo "1" || echo "0")
    
    # 输出结果
    if [ "$has_kernel0" = "1" ] && [ "$has_pe" = "1" ] && [ "$has_io" = "1" ]; then
        echo -e "${GREEN}PASS${NC}: $config"
        echo "  Lines: $our_lines (ref: $ref_lines, diff: $((our_lines - ref_lines)))"
        echo "  PIPELINE: $our_pipeline (ref: $ref_pipeline)"
        return 0
    else
        echo -e "${RED}FAIL${NC}: $config (missing key structures)"
        return 1
    fi
}

# 主测试循环
echo "=========================================="
echo "测试所有配置"
echo "=========================================="
echo ""

passed=0
failed=0
skipped=0

for config in "${all_configs[@]}"; do
    result=$(test_config "$config" 2>&1)
    echo "$result"
    case "${result:0:4}" in
        "PASS")
            ((passed++))
            ;;
        "FAIL")
            ((failed++))
            ;;
        "SKIP")
            ((skipped++))
            ;;
    esac
done

echo ""
echo "=========================================="
echo "测试总结"
echo "=========================================="
echo -e "${GREEN}通过: $passed${NC}"
echo -e "${RED}失败: $failed${NC}"
echo -e "${YELLOW}跳过: $skipped${NC}"
echo ""

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}所有测试通过！${NC}"
    exit 0
else
    echo -e "${RED}部分测试失败${NC}"
    exit 1
fi

