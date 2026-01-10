#!/bin/bash
# generate_autosa_reference.sh - Generate AutoSA reference code for comparison
#
# This script runs AutoSA to generate reference implementations for different
# space-time configurations, matching our mlir-systolic test cases.
#
# Usage: ./generate_autosa_reference.sh [--st-mode N] [--clean]

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

# Paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
AUTOSA_DIR="${PROJECT_ROOT}/third_party/AutoSA"
AUTOSA_BIN="${AUTOSA_DIR}/autosa"
OUTPUT_DIR="${PROJECT_ROOT}/test/output/autosa_reference"
TEST_KERNEL="${AUTOSA_DIR}/autosa_tests/mm/kernel.c"
CONFIG_JSON="${AUTOSA_DIR}/autosa_config/autosa_config.json"
SIMD_INFO="${AUTOSA_DIR}/autosa_tests/mm/simd_info.json"

# Options
TEST_SPECIFIC_MODE=""
CLEAN_MODE=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --st-mode)
            TEST_SPECIFIC_MODE="$2"
            shift 2
            ;;
        --clean)
            CLEAN_MODE=1
            shift
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Check AutoSA binary
if [ ! -f "$AUTOSA_BIN" ]; then
    echo -e "${RED}ERROR: AutoSA binary not found at $AUTOSA_BIN${NC}"
    exit 1
fi

if [ ! -f "$TEST_KERNEL" ]; then
    echo -e "${RED}ERROR: Test kernel not found at $TEST_KERNEL${NC}"
    exit 1
fi

# Clean output directory if requested
if [ $CLEAN_MODE -eq 1 ]; then
    echo -e "${YELLOW}Cleaning output directory...${NC}"
    rm -rf "$OUTPUT_DIR"
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}AutoSA Reference Code Generation${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""

# ST configurations matching our test cases
# Format: mode:name:space_time:array_part:latency:simd:description
# Note: ST2,ST4,ST5 use reduction and need --local-reduce flags (added automatically)
CONFIGS=(
    "0:ST0:0:32,32,32:8,8:2:1D row array (single space at i)"
    "1:ST1:1:32,32,32:8,8:2:1D column array (single space at j)"
    "2:ST2:2:4,32,32:8,8:2:1D reduction array (single space at k)"
    "3:ST3:3:16,16,16:8,8:2:2D output-stationary (i,j)"
    "4:ST4:4:32,4,32:16,16:2:2D with horizontal reduction (i,k)"
    "5:ST5:5:32,4,32:16,16:2:2D with vertical reduction (j,k)"
)

if [ -n "$TEST_SPECIFIC_MODE" ]; then
    # Filter to specific mode
    TEMP_CONFIGS=()
    for config in "${CONFIGS[@]}"; do
        IFS=':' read -r mode name st ap lat simd desc <<< "$config"
        if [ "$mode" = "$TEST_SPECIFIC_MODE" ]; then
            TEMP_CONFIGS+=("$config")
        fi
    done
    CONFIGS=("${TEMP_CONFIGS[@]}")
fi

PASSED=0
FAILED=0

for config in "${CONFIGS[@]}"; do
    IFS=':' read -r mode name st_mode array_part latency simd description <<< "$config"
    
    echo -e "${BLUE}----------------------------------------${NC}"
    echo -e "${BLUE}Generating $name (mode=$mode)${NC}"
    echo -e "${BLUE}$description${NC}"
    echo ""
    
    # Create output subdirectory with required AutoSA structure
    MODE_OUTPUT_DIR="$OUTPUT_DIR/$name"
    mkdir -p "$MODE_OUTPUT_DIR/src"
    mkdir -p "$MODE_OUTPUT_DIR/latency_est"
    mkdir -p "$MODE_OUTPUT_DIR/resource_est"
    mkdir -p "$MODE_OUTPUT_DIR/tuning"
    
    # Build AutoSA command
    # Format: --sa-sizes="{kernel[]->space_time[N];kernel[]->array_part[X,Y,Z];kernel[]->latency[L1,L2];kernel[]->simd[S]}"
    SA_SIZES="kernel[]->space_time[$st_mode];kernel[]->array_part[$array_part];kernel[]->latency[$latency];kernel[]->simd[$simd]"
    
    # ST2, ST4, ST5 involve reduction loop k and need extra flags
    EXTRA_FLAGS=""
    if [ "$mode" = "2" ] || [ "$mode" = "4" ] || [ "$mode" = "5" ]; then
        EXTRA_FLAGS="--local-reduce --reduce-op=+ --simd-touch-space --array-contraction"
    fi
    
    AUTOSA_CMD="$AUTOSA_BIN $TEST_KERNEL \
        --config=$CONFIG_JSON \
        --target=autosa_hls_c \
        --output-dir=$MODE_OUTPUT_DIR \
        --sa-sizes={$SA_SIZES} \
        --simd-info=$SIMD_INFO \
        --host-serialize \
        $EXTRA_FLAGS"
    
    echo -e "${YELLOW}Command:${NC}"
    echo "$AUTOSA_CMD"
    echo ""
    
    # Run AutoSA (must be run from AutoSA directory)
    cd "$AUTOSA_DIR"
    if $AUTOSA_CMD > "$MODE_OUTPUT_DIR/autosa.log" 2>&1; then
        cd "$PROJECT_ROOT/test"
        echo -e "${GREEN}✓ AutoSA generation succeeded${NC}"
        
        # Check for generated kernel
        if [ -f "$MODE_OUTPUT_DIR/src/kernel_kernel.cpp" ]; then
            KERNEL_SIZE=$(wc -l < "$MODE_OUTPUT_DIR/src/kernel_kernel.cpp")
            echo -e "  Generated kernel: ${KERNEL_SIZE} lines"
            
            # Copy kernel to easy-access location
            cp "$MODE_OUTPUT_DIR/src/kernel_kernel.cpp" "$MODE_OUTPUT_DIR/kernel.cpp"
            
            # Extract key statistics
            PE_COUNT=$(grep -c "PE_wrapper" "$MODE_OUTPUT_DIR/kernel.cpp" || echo 0)
            IO_COUNT=$(grep -c "_IO_L" "$MODE_OUTPUT_DIR/kernel.cpp" || echo 0)
            PRAGMA_COUNT=$(grep -c "#pragma" "$MODE_OUTPUT_DIR/kernel.cpp" || echo 0)
            
            echo -e "  PE wrappers: $PE_COUNT"
            echo -e "  I/O modules: $IO_COUNT"
            echo -e "  HLS pragmas: $PRAGMA_COUNT"
            
            # Create metadata file
            cat > "$MODE_OUTPUT_DIR/metadata.json" << EOF
{
  "st_mode": $mode,
  "space_time": $st_mode,
  "array_part": "$array_part",
  "latency": "$latency",
  "simd": $simd,
  "description": "$description",
  "kernel_lines": $KERNEL_SIZE,
  "pe_wrappers": $PE_COUNT,
  "io_modules": $IO_COUNT,
  "hls_pragmas": $PRAGMA_COUNT
}
EOF
            
            PASSED=$((PASSED + 1))
        else
            cd "$PROJECT_ROOT/test"
            echo -e "${RED}✗ Kernel file not generated${NC}"
            FAILED=$((FAILED + 1))
        fi
    else
        cd "$PROJECT_ROOT/test"
        echo -e "${RED}✗ AutoSA generation failed${NC}"
        echo -e "  See log: $MODE_OUTPUT_DIR/autosa.log"
        tail -20 "$MODE_OUTPUT_DIR/autosa.log"
        FAILED=$((FAILED + 1))
    fi
    
    echo ""
done

# Summary
echo -e "${CYAN}========================================${NC}"
echo -e "${CYAN}Summary${NC}"
echo -e "${CYAN}========================================${NC}"
echo ""
echo -e "Total configurations: $((PASSED + FAILED))"
echo -e "${GREEN}Succeeded: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo ""
echo -e "Output directory: $OUTPUT_DIR"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All reference code generated successfully!${NC}"
    exit 0
else
    echo -e "${YELLOW}⚠ Some configurations failed${NC}"
    exit 1
fi
