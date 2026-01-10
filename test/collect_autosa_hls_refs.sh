#!/bin/bash
# collect_autosa_hls_refs.sh - Collect and rename AutoSA HLS references with complete parameter encoding
#
# This script intelligently extracts HLS files from autosa_reference_samples using metadata
# and renames them with complete parameter information for easy identification and testing.
#
# Naming convention: {kernel}_{prob_size}_{st}{space_time}_ap{array_parts}_lat{latencies}_simd{simd}.cpp
# Example: mm_512_st3_ap16x16x16_lat8x8_simd2.cpp

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REF_SAMPLES="$PROJECT_ROOT/autosa_reference_samples"
OUTPUT_DIR="$PROJECT_ROOT/test/autosa_hls_refs"

mkdir -p "$OUTPUT_DIR"

GREEN='\033[0;32m'; RED='\033[0;31m'; CYAN='\033[0;36m'; NC='\033[0m'

log_ok() { echo -e "${GREEN}✓${NC} $1"; }
log_err() { echo -e "${RED}✗${NC} $1"; }
log_header() { echo -e "\n${CYAN}$1${NC}"; }

success=0; fail=0

# Helper: Convert comma-separated to x-separated
fmt() { echo "$1" | tr ',' 'x'; }

# Extract info from metadata.json and copy/rename HLS file
# Uses actual metadata parameters for accurate filename encoding
process_sample() {
    local sample_dir=$1
    
    [ ! -f "$sample_dir/metadata.json" ] && return 0
    
    # Extract parameters from metadata using grep
    local kernel=$(grep '"kernel"' "$sample_dir/metadata.json" | head -1 | sed 's/.*"kernel": *"\([^"]*\).*/\1/')
    local st=$(grep '"space_time"' "$sample_dir/metadata.json" | head -1 | sed 's/.*"space_time": *\([^,}]*\).*/\1/')
    local ap=$(grep '"array_part"' "$sample_dir/metadata.json" | head -1 | sed 's/.*"array_part": *"\([^"]*\).*/\1/')
    local lat=$(grep '"latency"' "$sample_dir/metadata.json" | head -1 | sed 's/.*"latency": *"\([^"]*\).*/\1/')
    local simd=$(grep '"simd"' "$sample_dir/metadata.json" | head -1 | sed 's/.*"simd": *"\([^"]*\).*/\1/')
    local desc=$(grep '"description"' "$sample_dir/metadata.json" | head -1 | sed 's/.*"description": *"\([^"]*\).*/\1/')
    
    # Find HLS file
    local src=""
    if [ -f "$sample_dir/src/kernel_kernel.cpp" ]; then
        src="$sample_dir/src/kernel_kernel.cpp"
    elif [ -f "$sample_dir/src/kernel_kernel_modules.cpp" ]; then
        src="$sample_dir/src/kernel_kernel_modules.cpp"
    elif [ -f "$sample_dir/kernel.cpp" ]; then
        src="$sample_dir/kernel.cpp"
    else
        return 0
    fi
    
    # Determine problem size from kernel name or use size indicator
    local prob_size="default"
    case "$kernel" in
        mm)
            if [ "$ap" = "16,16,16" ] || [ "$ap" = "4,16,16" ]; then prob_size="512"; fi
            if [ "$ap" = "32,32,32" ] || [ "$ap" = "32,4,32" ]; then prob_size="1024"; fi
            if [ "$ap" = "64,64,64" ]; then prob_size="2048"; fi
            ;;
        cnn)
            # Check sample name for size hints
            if [[ "$(basename "$sample_dir")" == *"small"* ]] || [ "$ap" = "8,8,4,8" ]; then prob_size="small"; fi
            if [[ "$(basename "$sample_dir")" == *"large"* ]] || [ "$ap" = "64,56,14,64" ]; then prob_size="large"; fi
            ;;
        mttkrp)
            if [ "$ap" = "32,32,2" ]; then prob_size="medium"; fi
            if [ "$ap" = "64,64,2" ]; then prob_size="large"; fi
            if [ "$ap" = "128,128,2" ]; then prob_size="xlarge"; fi
            ;;
        ttmc)
            if [ "$ap" = "8,32,8,16" ] || [ "$ap" = "4,16,4,8" ]; then prob_size="small"; fi
            if [ "$ap" = "8,32,8,16" ]; then prob_size="medium"; fi
            if [ "$ap" = "16,64,16,32" ]; then prob_size="large"; fi
            if [ "$ap" = "32,128,32,64" ]; then prob_size="xlarge"; fi
            ;;
        lu)
            if [[ "$(basename "$sample_dir")" == *"32" ]]; then prob_size="32"; fi
            if [[ "$(basename "$sample_dir")" == *"64" ]]; then prob_size="64"; fi
            ;;
    esac
    
    # Build filename with complete parameters
    local ap_fmt=$(fmt "$ap")
    local lat_fmt=$(fmt "$lat")
    local simd_fmt=$(fmt "$simd")
    local outfile="${kernel}_${prob_size}_st${st}_ap${ap_fmt}_lat${lat_fmt}_simd${simd_fmt}.cpp"
    
    # Skip if file already exists (avoid duplicates)
    if [ -f "$OUTPUT_DIR/$outfile" ]; then
        return 0
    fi
    
    # Copy file
    cp "$src" "$OUTPUT_DIR/$outfile"
    local lines=$(wc -l < "$OUTPUT_DIR/$outfile")
    log_ok "$outfile ($lines lines)"
    
    ((success++))
    return 0
}

log_header "== Collecting AutoSA HLS References =="
log_header "(Organizing from: $REF_SAMPLES)"
echo ""

# Process all samples
for sample_dir in "$REF_SAMPLES"/*; do
    [ -d "$sample_dir" ] && process_sample "$sample_dir"
done

echo ""
log_header "== Summary =="

if [ $success -gt 0 ]; then
    num_files=$(ls -1 "$OUTPUT_DIR"/*.cpp 2>/dev/null | wc -l)
    total_lines=$(wc -l "$OUTPUT_DIR"/*.cpp 2>/dev/null | tail -1 | awk '{print $1}')
    
    log_ok "Collected $num_files unique reference files"
    echo "Total code lines: $total_lines"
    echo "Output directory: $OUTPUT_DIR"
    echo ""
    
    # Show statistics by kernel
    echo "Files per kernel:"
    for k in mm cnn mttkrp ttmc lu; do
        count=$(ls -1 "$OUTPUT_DIR/${k}_"*.cpp 2>/dev/null | wc -l) || count=0
        [ $count -gt 0 ] && echo "  $k: $count files"
    done
    
    echo ""
    echo "All collected files:"
    ls -1 "$OUTPUT_DIR"/*.cpp 2>/dev/null | xargs -n1 basename 2>/dev/null | sort
    
    exit 0
else
    log_err "No reference files collected!"
    exit 1
fi
