#!/bin/bash
# test_against_autosa.sh - 测试 mlir-systolic 生成的代码与 AutoSA 参考代码的对比

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

# 检查工具是否存在
SYSTOLIC_OPT="${BUILD_DIR}/bin/systolic-opt"
SYSTOLIC_TRANSLATE="${BUILD_DIR}/bin/systolic-translate"

if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo -e "${RED}ERROR: systolic-opt not found at $SYSTOLIC_OPT${NC}"
    echo "Please build the project first: ./scripts/build-systolic.sh"
    exit 1
fi

if [ ! -f "$SYSTOLIC_TRANSLATE" ]; then
    echo -e "${RED}ERROR: systolic-translate not found at $SYSTOLIC_TRANSLATE${NC}"
    echo "Please build the project first: ./scripts/build-systolic.sh"
    exit 1
fi

# 创建输出目录
OUTPUT_DIR="${TEST_DIR}/output"
mkdir -p "$OUTPUT_DIR"

echo "=========================================="
echo "mlir-systolic vs AutoSA 测试"
echo "=========================================="
echo ""

# 测试配置：从参考文件名解析参数
# 格式: mm_st3_I{I}_J{J}_K{K}_ap{AP}_lat{LAT}_simd{SIMD}_kernel.cpp
test_configs=(
    "I32_J32_K32_ap8_lat4_simd1"
    "I32_J32_K32_ap8_lat4_simd2"
    "I64_J64_K64_ap16_lat8_simd1"
    "I64_J64_K64_ap16_lat8_simd2"
    "I64_J64_K64_ap16_lat8_simd4"
)

# 从配置字符串解析参数
parse_config() {
    local config=$1
    # 提取 I, J, K
    I=$(echo "$config" | grep -oP 'I\K\d+')
    J=$(echo "$config" | grep -oP 'J\K\d+')
    K=$(echo "$config" | grep -oP 'K\K\d+')
    # 提取 ap, lat, simd
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

# 运行测试
run_test() {
    local config=$1
    echo -e "${YELLOW}Testing configuration: $config${NC}"
    
    # 解析参数
    read I J K AP LAT SIMD <<< "$(parse_config "$config")"
    
    echo "  Matrix size: ${I}x${J}x${K}"
    echo "  array_part: [${AP}, ${AP}, ${AP}]"
    echo "  latency: [${LAT}, ${LAT}]"
    echo "  simd: ${SIMD}"
    
    # 生成 MLIR 输入
    local mlir_input="${OUTPUT_DIR}/matmul_${config}.mlir"
    generate_mlir_input "$I" "$J" "$K" "$mlir_input"
    
    # 运行 systolic-opt 进行变换
    # 第一步：应用 SystolicTransform
    local step1_mlir="${OUTPUT_DIR}/matmul_${config}_step1.mlir"
    echo "  Step 1: Running systolic-transform..."
    if ! "$SYSTOLIC_OPT" "$mlir_input" \
        --systolic-transform \
        -o "$step1_mlir" 2>&1 | tee "${OUTPUT_DIR}/matmul_${config}_step1.log"; then
        echo -e "  ${RED}FAILED: systolic-transform failed${NC}"
        return 1
    fi
    
    # 运行 systolic-translate 生成 HLS C++
    # 注意：systolic-translate 直接接受 MLIR 输入并生成 HLS C++
    local output_cpp="${OUTPUT_DIR}/matmul_${config}_kernel.cpp"
    echo "  Step 2: Running systolic-translate..."
    if ! "$SYSTOLIC_TRANSLATE" "$step1_mlir" \
        --array-part="$AP" \
        --latency="$LAT" \
        --simd="$SIMD" \
        --size="$I" \
        -o "$output_cpp" 2>&1 | tee "${OUTPUT_DIR}/matmul_${config}_translate.log"; then
        echo -e "  ${RED}FAILED: systolic-translate failed${NC}"
        return 1
    fi
    
    # 检查输出文件是否存在
    if [ ! -f "$output_cpp" ]; then
        echo -e "  ${RED}FAILED: Output file not generated${NC}"
        return 1
    fi
    
    # 对比参考文件
    local ref_file="${REF_DIR}/mm_st3_${config}_kernel.cpp"
    if [ -f "$ref_file" ]; then
        echo "  Comparing with AutoSA reference..."
        compare_with_reference "$output_cpp" "$ref_file" "$config"
    else
        echo -e "  ${YELLOW}WARNING: Reference file not found: $ref_file${NC}"
    fi
    
    echo -e "  ${GREEN}SUCCESS${NC}"
    echo ""
}

# 对比生成的代码和参考代码
compare_with_reference() {
    local our_file=$1
    local ref_file=$2
    local config=$3
    
    echo "    === 对比结果 ==="
    
    # 文件行数
    our_lines=$(wc -l < "$our_file")
    ref_lines=$(wc -l < "$ref_file")
    echo "    行数: 我们的=${our_lines}, AutoSA=${ref_lines}"
    
    # 检查关键结构
    echo "    === 关键结构检查 ==="
    
    # 检查 kernel0 函数
    if grep -q "void kernel0" "$our_file"; then
        echo -e "    ${GREEN}✓${NC} kernel0 函数存在"
    else
        echo -e "    ${RED}✗${NC} kernel0 函数缺失"
    fi
    
    # 检查 PE 函数
    if grep -q "void PE\|void PE_wrapper" "$our_file"; then
        echo -e "    ${GREEN}✓${NC} PE 函数存在"
    else
        echo -e "    ${RED}✗${NC} PE 函数缺失"
    fi
    
    # 检查 IO 模块
    if grep -q "IO_L\|io_L" "$our_file"; then
        echo -e "    ${GREEN}✓${NC} IO 模块存在"
    else
        echo -e "    ${RED}✗${NC} IO 模块缺失"
    fi
    
    # 检查 HLS pragma
    pipeline_count=$(grep -c "PIPELINE" "$our_file" || echo "0")
    ref_pipeline_count=$(grep -c "PIPELINE" "$ref_file" || echo "0")
    echo "    PIPELINE pragma: 我们的=${pipeline_count}, AutoSA=${ref_pipeline_count}"
    
    # 检查数据类型定义
    if grep -q "typedef.*A_t\|typedef.*B_t\|typedef.*C_t" "$our_file"; then
        echo -e "    ${GREEN}✓${NC} 数据类型定义存在"
    else
        echo -e "    ${RED}✗${NC} 数据类型定义缺失"
    fi
    
    echo ""
}

# 主测试循环
echo "开始测试..."
echo ""

passed=0
failed=0

for config in "${test_configs[@]}"; do
    if run_test "$config"; then
        ((passed++))
    else
        ((failed++))
    fi
done

echo "=========================================="
echo "测试总结"
echo "=========================================="
echo -e "${GREEN}通过: $passed${NC}"
echo -e "${RED}失败: $failed${NC}"
echo ""

if [ $failed -eq 0 ]; then
    echo -e "${GREEN}所有测试通过！${NC}"
    exit 0
else
    echo -e "${RED}部分测试失败，请查看输出日志${NC}"
    exit 1
fi

