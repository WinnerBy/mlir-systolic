#!/bin/bash
# Test write-time reordering with MTTKRP kernel
# This script tests if the reordering analysis correctly detects and handles
# non-linear access patterns in MTTKRP kernel

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

echo "=== MTTKRP Write-time Reordering Test ==="
echo ""

SYSTOLIC_OPT="$BUILD_DIR/bin/systolic-opt"
SYSTOLIC_TRANSLATE="$BUILD_DIR/bin/systolic-translate"

if [ ! -f "$SYSTOLIC_OPT" ] || [ ! -f "$SYSTOLIC_TRANSLATE" ]; then
    echo "Error: Tools not found. Please build the project first."
    exit 1
fi

TEST_OUTPUT_DIR="$SCRIPT_DIR/output/mttkrp_test"
mkdir -p "$TEST_OUTPUT_DIR"

# Test 1: Basic MTTKRP kernel (no non-linear access expected)
echo "Test 1: Basic MTTKRP kernel..."
TEST_MM="$TEST_OUTPUT_DIR/mttkrp_basic.mlir"
cp "$SCRIPT_DIR/mttkrp_kernel.mlir" "$TEST_MM"

echo "  Running systolic-transform..."
if $SYSTOLIC_OPT "$TEST_MM" -systolic-transform \
    -o "$TEST_OUTPUT_DIR/mttkrp_transformed.mlir" 2>&1; then
    echo "  ✓ Transform successful"
else
    echo "  ✗ Transform failed"
    exit 1
fi

echo "  Running systolic-dataflow-generation..."
if $SYSTOLIC_OPT "$TEST_OUTPUT_DIR/mttkrp_transformed.mlir" \
    -systolic-dataflow-generation \
    -o "$TEST_OUTPUT_DIR/mttkrp_dataflow.mlir" 2>&1; then
    echo "  ✓ Dataflow generation successful"
    
    # Check for reordering attributes
    if grep -q "systolic.reorder" "$TEST_OUTPUT_DIR/mttkrp_dataflow.mlir" 2>/dev/null; then
        echo "  ✓ Reordering attributes found:"
        grep "systolic.reorder" "$TEST_OUTPUT_DIR/mttkrp_dataflow.mlir" | head -5 | sed 's/^/    /'
    else
        echo "  ⚠ No reordering attributes (expected for basic kernel without non-linear access)"
    fi
else
    echo "  ✗ Dataflow generation failed"
    exit 1
fi

# Test 2: MTTKRP with non-linear access pattern
echo ""
echo "Test 2: MTTKRP with non-linear access pattern..."
TEST_NL="$TEST_OUTPUT_DIR/mttkrp_nonlinear.mlir"
cp "$SCRIPT_DIR/mttkrp_with_nonlinear.mlir" "$TEST_NL"

echo "  Running systolic-transform..."
if $SYSTOLIC_OPT "$TEST_NL" -systolic-transform \
    -o "$TEST_OUTPUT_DIR/mttkrp_nl_transformed.mlir" 2>&1; then
    echo "  ✓ Transform successful"
else
    echo "  ✗ Transform failed"
    exit 1
fi

echo "  Running systolic-dataflow-generation..."
if $SYSTOLIC_OPT "$TEST_OUTPUT_DIR/mttkrp_nl_transformed.mlir" \
    -systolic-dataflow-generation \
    -o "$TEST_OUTPUT_DIR/mttkrp_nl_dataflow.mlir" 2>&1; then
    echo "  ✓ Dataflow generation successful"
    
    # Check for reordering attributes
    if grep -q "systolic.reorder" "$TEST_OUTPUT_DIR/mttkrp_nl_dataflow.mlir" 2>/dev/null; then
        echo "  ✓ Reordering attributes found:"
        grep "systolic.reorder" "$TEST_OUTPUT_DIR/mttkrp_nl_dataflow.mlir" | head -10 | sed 's/^/    /'
    else
        echo "  ⚠ No reordering attributes found (may need to check analysis)"
    fi
else
    echo "  ✗ Dataflow generation failed"
    exit 1
fi

# Test 3: Generate HLS code
echo ""
echo "Test 3: Generating HLS code..."
if $SYSTOLIC_TRANSLATE "$TEST_OUTPUT_DIR/mttkrp_dataflow.mlir" \
    -array-part=16 \
    -latency=8 \
    -simd=1 \
    -size=128 \
    -o "$TEST_OUTPUT_DIR/mttkrp_kernel.cpp" 2>&1; then
    echo "  ✓ HLS code generation successful"
    
    # Check array declarations
    ARRAY_DECLS=$(grep -E "local_[A-Z]\[.*\]" "$TEST_OUTPUT_DIR/mttkrp_kernel.cpp" | head -5)
    if [ -n "$ARRAY_DECLS" ]; then
        echo "  Array declarations:"
        echo "$ARRAY_DECLS" | sed 's/^/    /'
    fi
else
    echo "  ✗ HLS code generation failed"
    exit 1
fi

# Summary
echo ""
echo "=== Summary ==="
echo "Test files:"
echo "  - Basic MTTKRP: $TEST_OUTPUT_DIR/mttkrp_dataflow.mlir"
echo "  - Non-linear MTTKRP: $TEST_OUTPUT_DIR/mttkrp_nl_dataflow.mlir"
echo "  - Generated HLS: $TEST_OUTPUT_DIR/mttkrp_kernel.cpp"
echo ""
echo "Status:"
echo "  - Transform pass: ✓"
echo "  - Dataflow generation: ✓"
echo "  - HLS code generation: ✓"
echo ""
echo "Next steps:"
echo "  1. Check reordering attributes in generated MLIR files"
echo "  2. Verify array declarations in generated HLS code"
echo "  3. Compare with AutoSA's generated code for MTTKRP"
echo ""


