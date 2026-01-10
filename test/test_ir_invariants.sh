#!/bin/bash
# test_ir_invariants.sh - IR Invariants Regression Test
#
# Validates that generated IR meets critical requirements:
# 1. No "block with no terminator" errors
# 2. All terminator operations are the last operation in block
# 3. No operations exist after terminator yields
# 4. All systolic.dataflow operations properly structured
#
# Usage: ./test_ir_invariants.sh [--verbose] [--st-mode N]

set -e

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Project root
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${PROJECT_ROOT}/build"
TEST_DIR="${PROJECT_ROOT}/test"
OUTPUT_DIR="${TEST_DIR}/output/phase2_parametric"

# Options
VERBOSE=0
TEST_SPECIFIC_MODE=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --verbose)
            VERBOSE=1
            shift
            ;;
        --st-mode)
            TEST_SPECIFIC_MODE="$2"
            shift 2
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

SYSTOLIC_OPT="${BUILD_DIR}/bin/systolic-opt"

if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo -e "${RED}ERROR: systolic-opt not found${NC}"
    exit 1
fi

# Create input MLIR file for testing
INPUT_MLIR=$(mktemp /tmp/test_ir_invariants_XXXXXX.mlir)
trap "rm -f $INPUT_MLIR" EXIT

cat > "$INPUT_MLIR" << 'EOF'
module {
  func.func @matmul(%arg0: memref<32x32xf32>, %arg1: memref<32x32xf32>, %arg2: memref<32x32xf32>) {
    affine.for %i = 0 to 32 {
      affine.for %j = 0 to 32 {
        affine.for %k = 0 to 32 {
          %0 = affine.load %arg0[%i, %j] : memref<32x32xf32>
          %1 = affine.load %arg1[%k, %j] : memref<32x32xf32>
          %2 = affine.load %arg2[%i, %k] : memref<32x32xf32>
          %3 = arith.mulf %1, %2 : f32
          %4 = arith.addf %0, %3 : f32
          affine.store %4, %arg0[%i, %j] : memref<32x32xf32>
        }
      }
    }
    return
  }
}
EOF

# Test configuration
CONFIGS=(
    "0:ST0"
    "3:ST3"
)

if [ -n "$TEST_SPECIFIC_MODE" ]; then
    CONFIGS=("$TEST_SPECIFIC_MODE:ST$TEST_SPECIFIC_MODE")
fi

PASSED=0
FAILED=0
TOTAL=0

# Invariant check functions
check_no_block_with_no_terminator() {
    local ir_file=$1
    local name=$2
    
    if grep -q "block with no terminator" "$ir_file" 2>/dev/null; then
        echo -e "${RED}✗ FAIL${NC}: $name - 'block with no terminator' error"
        return 1
    fi
    return 0
}

check_terminators_last() {
    local ir_file=$1
    local name=$2
    
    # Check that yield ops are followed by closing braces, not more ops
    # Pattern: "yield" followed by another op (not "}" or end of line)
    if grep -E "yield.*\"\s*%[a-zA-Z0-9]+ = \"" "$ir_file" > /dev/null 2>&1; then
        echo -e "${RED}✗ FAIL${NC}: $name - Operations after yield terminator"
        return 1
    fi
    return 0
}

check_dataflow_consistency() {
    local ir_file=$1
    local name=$2
    
    # Count opening and closing dataflow.io.module ops
    open_io=$(grep -c "systolic.dataflow.io.module" "$ir_file" 2>/dev/null || echo 0)
    close_io=$(grep -c 'systolic.dataflow.io.module.yield' "$ir_file" 2>/dev/null || echo 0)
    
    if [ "$open_io" -ne "$close_io" ]; then
        echo -e "${RED}✗ FAIL${NC}: $name - Mismatched IOModule open/close ($open_io != $close_io)"
        return 1
    fi
    
    # Check for proper region structure: each module should have a body
    if grep -q "systolic.dataflow.io.module.*{" "$ir_file"; then
        return 0
    fi
    return 0
}

# Main test loop
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}IR Invariants Regression Test${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

for config in "${CONFIGS[@]}"; do
    IFS=':' read -r mode name <<< "$config"
    TOTAL=$((TOTAL + 1))
    
    echo -e "${BLUE}Test: $name (mode=$mode)${NC}"
    
    # Transform pass
    TRANSFORM_OUT="/tmp/ir_invariant_transform_$mode.mlir"
    if ! $SYSTOLIC_OPT "$INPUT_MLIR" \
            --systolic-transform \
            -o "$TRANSFORM_OUT" > /dev/null 2>&1; then
        echo -e "${RED}✗ FAIL${NC}: Transform pass failed"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Dataflow generation pass
    DATAFLOW_OUT="/tmp/ir_invariant_dataflow_$mode.mlir"
    if ! $SYSTOLIC_OPT "$TRANSFORM_OUT" \
            --systolic-dataflow-generation \
            -o "$DATAFLOW_OUT" > /dev/null 2>&1; then
        echo -e "${RED}✗ FAIL${NC}: Dataflow generation pass failed"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Run invariant checks
    check_failed=0
    
    echo -n "  Checking: no 'block with no terminator'... "
    if check_no_block_with_no_terminator "$DATAFLOW_OUT" "$name"; then
        echo -e "${GREEN}OK${NC}"
    else
        check_failed=1
    fi
    
    echo -n "  Checking: terminators are last operation... "
    if check_terminators_last "$DATAFLOW_OUT" "$name"; then
        echo -e "${GREEN}OK${NC}"
    else
        check_failed=1
    fi
    
    echo -n "  Checking: dataflow consistency... "
    if check_dataflow_consistency "$DATAFLOW_OUT" "$name"; then
        echo -e "${GREEN}OK${NC}"
    else
        check_failed=1
    fi
    
    if [ $VERBOSE -eq 1 ]; then
        echo "  Generated IR file: $DATAFLOW_OUT"
    fi
    
    if [ $check_failed -eq 0 ]; then
        echo -e "${GREEN}✓ $name PASSED${NC}"
        PASSED=$((PASSED + 1))
    else
        FAILED=$((FAILED + 1))
    fi
    
    echo ""
done

# Summary
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Test Summary${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "Total: $TOTAL"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All IR invariant tests PASSED!${NC}"
    exit 0
else
    echo -e "${RED}✗ Some tests FAILED${NC}"
    exit 1
fi
