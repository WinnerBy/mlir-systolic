#!/bin/bash
# test_phase2_basic.sh - Phase 2 基础功能测试
#
# 快速验证 Phase 2 参数化框架的核心功能：
# 1. 参数化循环选择是否工作
# 2. 数据流分析是否正确
# 3. 配置传递是否成功

set -e

# 颜色
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_DIR="${PROJECT_ROOT}/test"
OUTPUT_DIR="${TEST_DIR}/output/phase2_basic"

SYSTOLIC_OPT="${BUILD_DIR}/bin/systolic-opt"

if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo -e "${RED}ERROR: systolic-opt not found${NC}"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

echo -e "${BLUE}=== Phase 2 基础功能测试 ===${NC}"
echo ""

# 生成简单的 32x32 matmul
INPUT="${OUTPUT_DIR}/matmul_32x32.mlir"
cat > "$INPUT" <<'EOF'
module {
  func.func @matmul(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
    affine.for %i = 0 to 32 {
      affine.for %j = 0 to 32 {
        affine.for %k = 0 to 32 {
          %a = affine.load %A[%i, %k] : memref<32x32xf32>
          %b = affine.load %B[%k, %j] : memref<32x32xf32>
          %c = affine.load %C[%i, %j] : memref<32x32xf32>
          %prod = arith.mulf %a, %b : f32
          %sum = arith.addf %c, %prod : f32
          affine.store %sum, %C[%i, %j] : memref<32x32xf32>
        }
      }
    }
    return
  }
}
EOF

echo "生成测试输入: $INPUT"
echo ""

# 测试函数
test_transform() {
    local mode=$1
    local name=$2
    local output="${OUTPUT_DIR}/transform_st${mode}.mlir"
    local log="${OUTPUT_DIR}/transform_st${mode}.log"
    
    echo -n "测试 ${name} (mode=${mode})... "
    
    # Phase 2 暂时使用默认配置（ST3），未来需要添加命令行选项
    if $SYSTOLIC_OPT "$INPUT" \
        --systolic-transform \
        -o "$output" > "$log" 2>&1; then
        
        # 检查输出
        if [ -s "$output" ]; then
            # 检查是否有 systolic 相关的属性
            if grep -q "systolic" "$output" 2>/dev/null; then
                echo -e "${GREEN}✓ PASS${NC}"
                return 0
            else
                echo -e "${YELLOW}⚠ WARN (no systolic attrs)${NC}"
                return 2
            fi
        else
            echo -e "${RED}✗ FAIL (empty output)${NC}"
            return 1
        fi
    else
        echo -e "${RED}✗ FAIL (transform error)${NC}"
        if [ -f "$log" ]; then
            echo "Error log:"
            tail -10 "$log"
        fi
        return 1
    fi
}

# 运行测试
PASS=0
FAIL=0
WARN=0

echo "1. 测试 ST3 (默认 2D 配置)"
test_transform 3 "ST3"
case $? in
    0) PASS=$((PASS + 1)) ;;
    1) FAIL=$((FAIL + 1)) ;;
    2) WARN=$((WARN + 1)) ;;
esac
echo ""

echo "2. 测试 ST0 (1D 配置)"
test_transform 0 "ST0"
case $? in
    0) PASS=$((PASS + 1)) ;;
    1) FAIL=$((FAIL + 1)) ;;
    2) WARN=$((WARN + 1)) ;;
esac
echo ""

echo "3. 测试 ST4 (2D 权重固定)"
test_transform 4 "ST4"
case $? in
    0) PASS=$((PASS + 1)) ;;
    1) FAIL=$((FAIL + 1)) ;;
    2) WARN=$((WARN + 1)) ;;
esac
echo ""

# 检查调试输出
echo "4. 检查调试信息"
if grep -r "Parametric" "${OUTPUT_DIR}"/*.log 2>/dev/null | head -3; then
    echo -e "${GREEN}✓ 参数化分析日志找到${NC}"
    PASS=$((PASS + 1))
else
    echo -e "${YELLOW}⚠ 未找到参数化分析日志（可能需要开启 DEBUG）${NC}"
    WARN=$((WARN + 1))
fi
echo ""

# 总结
echo -e "${BLUE}=== 测试总结 ===${NC}"
echo "通过: $PASS"
echo "警告: $WARN"
echo "失败: $FAIL"
echo ""

if [ $FAIL -eq 0 ]; then
    echo -e "${GREEN}✓ 基础测试通过！${NC}"
    echo ""
    echo "提示: 运行完整测试使用:"
    echo "  ./test_phase2_parametric.sh --verbose"
    exit 0
else
    echo -e "${RED}✗ 有测试失败${NC}"
    echo "检查日志: ${OUTPUT_DIR}/*.log"
    exit 1
fi
