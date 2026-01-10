#!/bin/bash
# generate_multi_kernel_reference.sh - Generate AutoSA reference for multiple kernels
#
# Usage: ./generate_multi_kernel_reference.sh [--kernel KERNEL] [--clean]

set -e

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
AUTOSA_DIR="${PROJECT_ROOT}/third_party/AutoSA"
AUTOSA_BIN="$AUTOSA_DIR/autosa"
CONFIG_JSON="$AUTOSA_DIR/autosa_config/autosa_config.json"
OUTPUT_DIR="${PROJECT_ROOT}/test/output/autosa_multi_kernel"

TEST_KERNEL=""
CLEAN_MODE=0
VERBOSE_MODE=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --kernel) TEST_KERNEL="$2"; shift 2 ;;
        --clean) CLEAN_MODE=1; shift ;;
        --verbose) VERBOSE_MODE=1; shift ;;
        *) echo "Unknown: $1"; exit 1 ;;
    esac
done

if [ $CLEAN_MODE -eq 1 ]; then
    echo -e "${YELLOW}Cleaning output directory...${NC}"
    rm -rf "$OUTPUT_DIR"
fi

mkdir -p "$OUTPUT_DIR"

echo -e "${CYAN}========================================"
echo "AutoSA Multi-Kernel Reference Generation"
echo -e "========================================${NC}\n"

# Kernel configurations based on autosa_hls_output/autosa_cmd.md
# Format: kernel_name:kernel_path:simd_path:space_time:array_part:latency:simd:extra_flags
KERNEL_CONFIGS=(
    "cnn_small:cnn:cnn:4:8,8,4,8:4,2,4:1,1,1,2:--host-serialize --no-reverse-order"
    "cnn_large:large/cnn:large/cnn:4:64,56,14,64:4,4,7:1,1,8:"
    "lu:lu:lu:3:-1,-1,-1:::--use-cplusplus-template --no-reschedule --live-range-reordering"
    "mttkrp:large/mttkrp:large/mttkrp:3:128,128,2:16,8:8,1:--local-reduce --reduce-op=+"
    "ttmc:large/ttmc:large/ttmc:4:16,64,16,32:1,8,8:8,1:"
)

if [ -n "$TEST_KERNEL" ]; then
    TEMP_CONFIGS=()
    for config in "${KERNEL_CONFIGS[@]}"; do
        IFS=':' read -r name kpath spath st ap lat simd extra <<< "$config"
        if [ "$name" = "$TEST_KERNEL" ]; then
            TEMP_CONFIGS+=("$config")
        fi
    done
    KERNEL_CONFIGS=("${TEMP_CONFIGS[@]}")
    if [ ${#KERNEL_CONFIGS[@]} -eq 0 ]; then
        echo -e "${RED}Error: Kernel '$TEST_KERNEL' not found${NC}"
        echo "Available kernels: cnn_small, cnn_large, lu, mttkrp, ttmc"
        exit 1
    fi
fi

PASSED=0
FAILED=0

for config in "${KERNEL_CONFIGS[@]}"; do
    IFS=':' read -r kernel_name kernel_path simd_path st_mode array_part latency simd extra_flags <<< "$config"
    
    echo -e "${BLUE}----------------------------------------"
    echo "Generating: $kernel_name"
    echo -e "----------------------------------------${NC}"
    
    KERNEL_FILE="$AUTOSA_DIR/autosa_tests/$kernel_path/kernel.c"
    SIMD_INFO="$AUTOSA_DIR/autosa_tests/$simd_path/simd_info.json"
    KERNEL_OUTPUT_DIR="$OUTPUT_DIR/$kernel_name"
    
    if [ ! -f "$KERNEL_FILE" ]; then
        echo -e "${RED}✗ Kernel file not found: $KERNEL_FILE${NC}\n"
        FAILED=$((FAILED + 1))
        continue
    fi
    
    # Create output structure
    mkdir -p "$KERNEL_OUTPUT_DIR/src"
    mkdir -p "$KERNEL_OUTPUT_DIR/latency_est"
    mkdir -p "$KERNEL_OUTPUT_DIR/resource_est"
    mkdir -p "$KERNEL_OUTPUT_DIR/tuning"
    
    # Build sa-sizes parameter
    SA_SIZES="kernel[]->space_time[$st_mode]"
    if [ "$array_part" != "-" ]; then
        SA_SIZES="$SA_SIZES;kernel[]->array_part[$array_part]"
    fi
    if [ -n "$latency" ] && [ "$latency" != "-" ]; then
        SA_SIZES="$SA_SIZES;kernel[]->latency[$latency]"
    fi
    if [ -n "$simd" ] && [ "$simd" != "-" ]; then
        SA_SIZES="$SA_SIZES;kernel[]->simd[$simd]"
    fi
    
    AUTOSA_CMD="$AUTOSA_BIN $KERNEL_FILE \
        --config=$CONFIG_JSON \
        --target=autosa_hls_c \
        --output-dir=$KERNEL_OUTPUT_DIR \
        --sa-sizes={$SA_SIZES}"
    
    if [ -f "$SIMD_INFO" ]; then
        AUTOSA_CMD="$AUTOSA_CMD --simd-info=$SIMD_INFO"
    fi
    
    if [ -n "$extra_flags" ]; then
        AUTOSA_CMD="$AUTOSA_CMD $extra_flags"
    fi
    
    if [ $VERBOSE_MODE -eq 1 ]; then
        AUTOSA_CMD="$AUTOSA_CMD --verbose"
    fi
    
    echo -e "${YELLOW}Command:${NC}"
    echo "$AUTOSA_CMD"
    echo ""
    
    # Run AutoSA
    cd "$AUTOSA_DIR"
    if $AUTOSA_CMD > "$KERNEL_OUTPUT_DIR/autosa.log" 2>&1; then
        cd "$PROJECT_ROOT/test"
        echo -e "${GREEN}✓ AutoSA generation succeeded${NC}"
        
        # Check for generated kernel
        if [ -f "$KERNEL_OUTPUT_DIR/src/kernel_kernel.cpp" ]; then
            KERNEL_SIZE=$(wc -l < "$KERNEL_OUTPUT_DIR/src/kernel_kernel.cpp")
            echo -e "  Generated kernel: ${KERNEL_SIZE} lines"
            
            # Copy kernel
            cp "$KERNEL_OUTPUT_DIR/src/kernel_kernel.cpp" "$KERNEL_OUTPUT_DIR/kernel.cpp"
            
            # Extract statistics
            PE_COUNT=$(grep -c "PE_wrapper" "$KERNEL_OUTPUT_DIR/kernel.cpp" || echo 0)
            IO_COUNT=$(grep -c "_IO_L" "$KERNEL_OUTPUT_DIR/kernel.cpp" || echo 0)
            PRAGMA_COUNT=$(grep -c "#pragma" "$KERNEL_OUTPUT_DIR/kernel.cpp" || echo 0)
            
            echo -e "  PE wrappers: $PE_COUNT"
            echo -e "  I/O modules: $IO_COUNT"
            echo -e "  HLS pragmas: $PRAGMA_COUNT"
            
            # Create metadata
            cat > "$KERNEL_OUTPUT_DIR/metadata.json" << EOF
{
  "kernel_name": "$kernel_name",
  "space_time": $st_mode,
  "array_part": "$array_part",
  "latency": "$latency",
  "simd": "$simd",
  "kernel_lines": $KERNEL_SIZE,
  "pe_wrappers": $PE_COUNT,
  "io_modules": $IO_COUNT,
  "hls_pragmas": $PRAGMA_COUNT
}
EOF
            
            PASSED=$((PASSED + 1))
        else
            echo -e "${RED}✗ Kernel file not generated${NC}"
            FAILED=$((FAILED + 1))
        fi
    else
        cd "$PROJECT_ROOT/test"
        echo -e "${RED}✗ AutoSA generation failed${NC}"
        echo -e "${YELLOW}See log: $KERNEL_OUTPUT_DIR/autosa.log${NC}"
        tail -20 "$KERNEL_OUTPUT_DIR/autosa.log"
        FAILED=$((FAILED + 1))
    fi
    echo ""
done

echo -e "${CYAN}========================================"
echo "Summary"
echo -e "========================================${NC}\n"

echo "Total kernels: $((PASSED + FAILED))"
echo -e "${GREEN}Succeeded: $PASSED${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED${NC}"
fi
echo ""
echo "Output directory: $OUTPUT_DIR"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All reference code generated successfully!${NC}"
else
    echo -e "${YELLOW}⚠ Some kernels failed${NC}"
fi
