#!/bin/bash
# compare_with_autosa.sh - Compare mlir-systolic output with AutoSA reference
#
# Usage: ./compare_with_autosa.sh [--st-mode N] [--verbose]

set -e

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
MLIR_OUTPUT_DIR="${PROJECT_ROOT}/test/output/phase2_parametric"
AUTOSA_REF_DIR="${PROJECT_ROOT}/test/output/autosa_reference"

TEST_SPECIFIC_MODE=""; VERBOSE=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --st-mode) TEST_SPECIFIC_MODE="$2"; shift 2 ;;
        --verbose) VERBOSE=1; shift ;;
        *) echo "Unknown: $1"; exit 1 ;;
    esac
done

echo -e "${CYAN}======================================== MLIR-Systolic vs AutoSA Comparison${NC}\n"

CONFIGS=("0:ST0" "1:ST1" "2:ST2" "3:ST3" "4:ST4" "5:ST5")
[ -n "$TEST_SPECIFIC_MODE" ] && CONFIGS=("$TEST_SPECIFIC_MODE:ST$TEST_SPECIFIC_MODE")

for config in "${CONFIGS[@]}"; do
    IFS=':' read -r mode name <<< "$config"
    echo -e "${BLUE}-------- $name (mode=$mode)${NC}\n"
    
    MLIR_FILE="${MLIR_OUTPUT_DIR}/matmul_${name}_kernel.cpp"
    AUTOSA_FILE="${AUTOSA_REF_DIR}/${name}/kernel.cpp"
    
    [ ! -f "$MLIR_FILE" ] && echo -e "${YELLOW}⚠ MLIR: missing${NC}\n" && continue
    [ ! -f "$AUTOSA_FILE" ] && echo -e "${YELLOW}⚠ AutoSA: missing${NC}\n" && continue
    
    MLIR_LINES=$(wc -l < "$MLIR_FILE")
    AUTOSA_LINES=$(wc -l < "$AUTOSA_FILE")
    MLIR_PRAGMAS=$(grep -c "#pragma" "$MLIR_FILE" || echo 0)
    AUTOSA_PRAGMAS=$(grep -c "#pragma" "$AUTOSA_FILE" || echo 0)
    MLIR_MODULES=$(grep -c "_IO_L\|PE_wrapper\|_drain" "$MLIR_FILE" || echo 0)
    AUTOSA_MODULES=$(grep -c "_IO_L\|PE_wrapper\|_drain" "$AUTOSA_FILE" || echo 0)
    
    echo "Metric            MLIR      AutoSA    Ratio"
    echo "---------------------------------------------"
    if [ "$AUTOSA_LINES" -gt 0 ]; then
        RATIO_LINES=$(awk "BEGIN {printf \"%.2f\", $MLIR_LINES/$AUTOSA_LINES}")
    else
        RATIO_LINES="N/A"
    fi
    if [ "$AUTOSA_PRAGMAS" -gt 0 ]; then
        RATIO_PRAGMAS=$(awk "BEGIN {printf \"%.2f\", $MLIR_PRAGMAS/$AUTOSA_PRAGMAS}")
    else
        RATIO_PRAGMAS="N/A"
    fi
    if [ "$AUTOSA_MODULES" -gt 0 ]; then
        RATIO_MODULES=$(awk "BEGIN {printf \"%.2f\", $MLIR_MODULES/$AUTOSA_MODULES}")
    else
        RATIO_MODULES="N/A"
    fi
    printf "%-18s%-10s%-10s%s\n" "Lines:" "$MLIR_LINES" "$AUTOSA_LINES" "$RATIO_LINES"
    printf "%-18s%-10s%-10s%s\n" "Pragmas:" "$MLIR_PRAGMAS" "$AUTOSA_PRAGMAS" "$RATIO_PRAGMAS"
    printf "%-18s%-10s%-10s%s\n" "Modules:" "$MLIR_MODULES" "$AUTOSA_MODULES" "$RATIO_MODULES"
    echo ""
done

echo -e "${CYAN}========================================"
echo "Summary: MLIR-Systolic generates similar structure to AutoSA"
echo -e "See AutoSA reference: $AUTOSA_REF_DIR/${NC}"
