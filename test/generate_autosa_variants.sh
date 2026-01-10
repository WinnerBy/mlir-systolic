#!/bin/bash
# generate_autosa_variants.sh - Generate diverse AutoSA kernel variants
#
# Usage: ./generate_autosa_variants.sh [--kernel KERNEL] [--clean]

set -e

RED='\033[0;31m'; GREEN='\033[0;32m'; YELLOW='\033[1;33m'
BLUE='\033[0;34m'; CYAN='\033[0;36m'; NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
AUTOSA_DIR="${PROJECT_ROOT}/third_party/AutoSA"
AUTOSA_BIN="$AUTOSA_DIR/autosa"
CONFIG_JSON="$AUTOSA_DIR/autosa_config/autosa_config.json"
OUTPUT_BASE="${PROJECT_ROOT}/autosa_reference_samples"

TEST_KERNEL=""
CLEAN_MODE=0

while [[ $# -gt 0 ]]; do
    case $1 in
        --kernel) TEST_KERNEL="$2"; shift 2 ;;
        --clean) CLEAN_MODE=1; shift ;;
        *) echo "Unknown: $1"; exit 1 ;;
    esac
done

if [ $CLEAN_MODE -eq 1 ]; then
    echo -e "${YELLOW}Cleaning output directory...${NC}"
    rm -rf "$OUTPUT_BASE"
fi

mkdir -p "$OUTPUT_BASE"

echo -e "${CYAN}========================================"
echo "AutoSA Kernel Variants Generation"
echo "Generating diverse parameter configurations"
echo -e "========================================${NC}\n"

# Kernel variant configurations
# Format: kernel_name:variant_id:space_time:array_part:latency:simd:description:extra_flags
KERNEL_VARIANTS=(
    # ===== Matrix Multiplication (6 space-time modes) =====
    "mm:st0:0:32,32,32:8,8:2:MM-ST0 1D-i output-stationary:"
    "mm:st1:1:32,32,32:8,8:2:MM-ST1 1D-j output-stationary:"
    "mm:st2:2:4,32,32:8,8:2:MM-ST2 1D-k reduction:--local-reduce --reduce-op=+ --simd-touch-space --array-contraction"
    "mm:st3:3:16,16,16:8,8:2:MM-ST3 2D-ij output-stationary:"
    "mm:st4:4:32,4,32:16,16:2:MM-ST4 2D-ik horizontal-reduction:--local-reduce --reduce-op=+ --simd-touch-space --array-contraction"
    "mm:st5:5:32,4,32:16,16:2:MM-ST5 2D-jk vertical-reduction:--local-reduce --reduce-op=+ --simd-touch-space --array-contraction"
    
    # MM with different array partitions (ST3 variants)
    "mm:st3_ap_auto:3:32,32,32:8,8:2:MM-ST3 larger-array-32x32x32:"
    "mm:st3_ap_260_256_512:3:260,256,512:20,16:64:MM-ST3 huge-array (from large):"
    "mm:st3_ap4_lat2_simd2:3:4,32,32:2,8:2:MM-ST3 smaller-array-4x32x32:"
    
    # MM with different SIMD factors
    "mm:st3_simd1:3:16,16,16:8,8:1:MM-ST3 SIMD-1:"
    "mm:st3_simd4:3:16,16,16:8,8:4:MM-ST3 SIMD-4:"
    "mm:st3_simd8:3:16,16,16:8,8:8:MM-ST3 SIMD-8:"
    
    # MM with different latency factors
    "mm:st3_lat4_4:3:16,16,16:4,4:2:MM-ST3 latency-4x4:"
    "mm:st3_lat16_16:3:16,16,16:16,16:2:MM-ST3 latency-16x16:"
    "mm:st3_lat32:3:16,16,16:32:2:MM-ST3 latency-32 (1D):"
    
    # ===== CNN (10 space-time modes documented) =====
    "cnn:st0:0:8,4,4,8:4,2,4:1,1,1,2:CNN-ST0 1D-o output-stationary:--host-serialize --no-reverse-order"
    "cnn:st1:1:4,8,4,8:2,4,4:1,1,1,2:CNN-ST1 1D-r output-stationary:--host-serialize --no-reverse-order"
    "cnn:st2:2:4,4,8,8:2,2,4:1,1,1,2:CNN-ST2 1D-c output-stationary:--host-serialize --no-reverse-order"
    "cnn:st3:3:4,4,4,8:2,2,2:1,1,1,2:CNN-ST3 1D-i reduction:--host-serialize --no-reverse-order --local-reduce --reduce-op=+"
    "cnn:st4:4:8,8,4,8:4,2,4:1,1,1,2:CNN-ST4 2D-or output-stationary:--host-serialize --no-reverse-order"
    "cnn:st5:5:8,4,8,8:4,2,4:1,1,1,2:CNN-ST5 2D-oc output-stationary:--host-serialize --no-reverse-order"
    "cnn:st6:6:4,8,8,8:2,4,4:1,1,1,2:CNN-ST6 2D-rc output-stationary:--host-serialize --no-reverse-order"
    "cnn:st7:7:8,4,4,8:4,2,2:1,1,1,2:CNN-ST7 2D-oi reduction:--host-serialize --no-reverse-order --local-reduce --reduce-op=+"
    "cnn:st8:8:4,8,4,8:2,4,2:1,1,1,2:CNN-ST8 2D-ri reduction:--host-serialize --no-reverse-order --local-reduce --reduce-op=+"
    "cnn:st9:9:4,4,8,8:2,2,4:1,1,1,2:CNN-ST9 2D-ci reduction:--host-serialize --no-reverse-order --local-reduce --reduce-op=+"
    
    # CNN with different SIMD configurations
    "cnn:st4_simd_1_1_1_1:4:8,8,4,8:4,2,4:1,1,1,1:CNN-ST4 SIMD-all-1:--host-serialize --no-reverse-order"
    "cnn:st4_simd_2_2_2_4:4:8,8,4,8:4,2,4:2,2,2,4:CNN-ST4 SIMD-2x2x2x4:--host-serialize --no-reverse-order"
    "cnn:st4_simd_1_1_8:4:8,8,4,8:4,2,4:1,1,8:CNN-ST4 SIMD-simplified:--host-serialize --no-reverse-order"
    
    # ===== MTTKRP (Tensor decomposition) =====
    "mttkrp:st3_ap128_128_2_lat16_8_simd8_1:3:128,128,2:16,8:8,1:MTTKRP-ST3 standard:--local-reduce --reduce-op=+"
    "mttkrp:st3_ap64_64_2_lat8_4_simd4_1:3:64,64,2:8,4:4,1:MTTKRP-ST3 smaller:--local-reduce --reduce-op=+"
    "mttkrp:st3_ap256_256_4_lat32_16_simd16_2:3:256,256,4:32,16:16,2:MTTKRP-ST3 larger:--local-reduce --reduce-op=+"
    
    # ===== TTMC (Tensor-Tensor Matrix Chain) =====
    "ttmc:st4_ap16_64_16_32_lat1_8_8_simd8_1:4:16,64,16,32:1,8,8:8,1:TTMC-ST4 standard:"
    "ttmc:st4_ap8_32_8_16_lat1_4_4_simd4_1:4:8,32,8,16:1,4,4:4,1:TTMC-ST4 smaller:"
    "ttmc:st4_ap32_128_32_64_lat2_16_16_simd16_2:4:32,128,32,64:2,16,16:16,2:TTMC-ST4 larger:"
)

if [ -n "$TEST_KERNEL" ]; then
    TEMP_VARIANTS=()
    for variant in "${KERNEL_VARIANTS[@]}"; do
        IFS=':' read -r kname vid st ap lat simd desc extra <<< "$variant"
        if [ "$kname" = "$TEST_KERNEL" ]; then
            TEMP_VARIANTS+=("$variant")
        fi
    done
    KERNEL_VARIANTS=("${TEMP_VARIANTS[@]}")
    if [ ${#KERNEL_VARIANTS[@]} -eq 0 ]; then
        echo -e "${RED}Error: Kernel '$TEST_KERNEL' not found${NC}"
        exit 1
    fi
fi

TOTAL=${#KERNEL_VARIANTS[@]}
SUCCESS=0
FAILED=0
CURRENT=0

for variant in "${KERNEL_VARIANTS[@]}"; do
    IFS=':' read -r kernel_name variant_id st_mode array_part latency simd description extra_flags <<< "$variant"
    
    CURRENT=$((CURRENT + 1))
    echo -e "${BLUE}========================================"
    echo "[$CURRENT/$TOTAL] $description"
    echo -e "========================================${NC}"
    
    # Determine kernel path and simd_info
    case $kernel_name in
        mm)
            KERNEL_PATH="mm"
            SIMD_INFO="mm"
            ;;
        cnn)
            KERNEL_PATH="cnn"
            SIMD_INFO="cnn"
            ;;
        mttkrp)
            KERNEL_PATH="large/mttkrp"
            SIMD_INFO="large/mttkrp"
            ;;
        ttmc)
            KERNEL_PATH="large/ttmc"
            SIMD_INFO="large/ttmc"
            ;;
        *)
            echo -e "${RED}Unknown kernel: $kernel_name${NC}\n"
            FAILED=$((FAILED + 1))
            continue
            ;;
    esac
    
    KERNEL_FILE="$AUTOSA_DIR/autosa_tests/$KERNEL_PATH/kernel.c"
    SIMD_INFO_FILE="$AUTOSA_DIR/autosa_tests/$SIMD_INFO/simd_info.json"
    
    # Create output directory with descriptive name
    OUTPUT_DIR="$OUTPUT_BASE/${kernel_name}_${variant_id}"
    mkdir -p "$OUTPUT_DIR/src" "$OUTPUT_DIR/latency_est" "$OUTPUT_DIR/resource_est" "$OUTPUT_DIR/tuning"
    
    # Build sa-sizes parameter
    SA_SIZES="kernel[]->space_time[$st_mode]"
    if [ -n "$array_part" ] && [ "$array_part" != "-" ]; then
        SA_SIZES="$SA_SIZES;kernel[]->array_part[$array_part]"
    fi
    if [ -n "$latency" ] && [ "$latency" != "-" ]; then
        SA_SIZES="$SA_SIZES;kernel[]->latency[$latency]"
    fi
    if [ -n "$simd" ] && [ "$simd" != "-" ]; then
        SA_SIZES="$SA_SIZES;kernel[]->simd[$simd]"
    fi
    
    # Build command
    AUTOSA_CMD="$AUTOSA_BIN $KERNEL_FILE \
        --config=$CONFIG_JSON \
        --target=autosa_hls_c \
        --output-dir=$OUTPUT_DIR \
        --sa-sizes={$SA_SIZES}"
    
    if [ -f "$SIMD_INFO_FILE" ]; then
        AUTOSA_CMD="$AUTOSA_CMD --simd-info=$SIMD_INFO_FILE"
    fi
    
    if [ -n "$extra_flags" ]; then
        AUTOSA_CMD="$AUTOSA_CMD $extra_flags"
    fi
    
    echo -e "${YELLOW}Configuration:${NC}"
    echo "  Space-Time: $st_mode"
    echo "  Array Part: $array_part"
    echo "  Latency: $latency"
    echo "  SIMD: $simd"
    echo ""
    
    # Run AutoSA
    cd "$AUTOSA_DIR"
    if $AUTOSA_CMD > "$OUTPUT_DIR/autosa.log" 2>&1; then
        cd "$PROJECT_ROOT"
        
        # Check for generated kernel
        if [ -f "$OUTPUT_DIR/src/kernel_kernel.cpp" ] && [ -s "$OUTPUT_DIR/src/kernel_kernel.cpp" ]; then
            KERNEL_SIZE=$(wc -l < "$OUTPUT_DIR/src/kernel_kernel.cpp")
            
            if [ "$KERNEL_SIZE" -gt 10 ]; then
                echo -e "${GREEN}✓ SUCCESS${NC}"
                echo "  Generated: ${KERNEL_SIZE} lines"
                
                # Copy kernel
                cp "$OUTPUT_DIR/src/kernel_kernel.cpp" "$OUTPUT_DIR/kernel.cpp"
                
                # Extract statistics
                PE_COUNT=$(grep -c "void PE" "$OUTPUT_DIR/kernel.cpp" 2>/dev/null || echo 0)
                IO_COUNT=$(grep -c "_IO_L\|_drain\|_in\|_out" "$OUTPUT_DIR/kernel.cpp" 2>/dev/null || echo 0)
                PRAGMA_COUNT=$(grep -c "#pragma" "$OUTPUT_DIR/kernel.cpp" 2>/dev/null || echo 0)
                FIFO_COUNT=$(grep -c "hls::stream" "$OUTPUT_DIR/kernel.cpp" 2>/dev/null || echo 0)
                
                echo "  PE functions: $PE_COUNT"
                echo "  I/O modules: $IO_COUNT"
                echo "  HLS pragmas: $PRAGMA_COUNT"
                echo "  FIFO channels: $FIFO_COUNT"
                
                # Create metadata
                cat > "$OUTPUT_DIR/metadata.json" << EOF
{
  "kernel": "$kernel_name",
  "variant_id": "$variant_id",
  "description": "$description",
  "space_time": $st_mode,
  "array_part": "$array_part",
  "latency": "$latency",
  "simd": "$simd",
  "extra_flags": "$extra_flags",
  "code_lines": $KERNEL_SIZE,
  "pe_functions": $PE_COUNT,
  "io_modules": $IO_COUNT,
  "hls_pragmas": $PRAGMA_COUNT,
  "fifo_channels": $FIFO_COUNT,
  "generation_date": "$(date -u +"%Y-%m-%dT%H:%M:%SZ")"
}
EOF
                
                SUCCESS=$((SUCCESS + 1))
            else
                echo -e "${YELLOW}⚠ WARNING: Kernel too small ($KERNEL_SIZE lines)${NC}"
                FAILED=$((FAILED + 1))
            fi
        else
            echo -e "${RED}✗ FAILED: No kernel generated${NC}"
            tail -10 "$OUTPUT_DIR/autosa.log" | sed 's/^/  /'
            FAILED=$((FAILED + 1))
        fi
    else
        cd "$PROJECT_ROOT"
        echo -e "${RED}✗ FAILED: AutoSA error${NC}"
        tail -10 "$OUTPUT_DIR/autosa.log" | sed 's/^/  /'
        FAILED=$((FAILED + 1))
    fi
    echo ""
done

echo -e "${CYAN}========================================"
echo "Summary"
echo -e "========================================${NC}"
echo ""
echo "Total variants: $TOTAL"
echo -e "${GREEN}Success: $SUCCESS${NC}"
if [ $FAILED -gt 0 ]; then
    echo -e "${RED}Failed: $FAILED${NC}"
fi
echo ""
echo "Output: $OUTPUT_BASE"
echo ""

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}✓ All variants generated successfully!${NC}"
else
    echo -e "${YELLOW}⚠ Some variants failed (see logs in output directories)${NC}"
fi
