#!/bin/bash
# Test MTTKRP write-time reordering analysis
# This script tests the analysis functionality independently

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

echo "=== MTTKRP Write-time Reordering Analysis Test ==="
echo ""

SYSTOLIC_OPT="$BUILD_DIR/bin/systolic-opt"

if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo "Error: systolic-opt not found. Please build the project first."
    exit 1
fi

TEST_OUTPUT_DIR="$SCRIPT_DIR/output/mttkrp_test"
mkdir -p "$TEST_OUTPUT_DIR"

# Test 1: Simple test with non-linear access
echo "Test 1: Simple test with non-linear access pattern..."
TEST_FILE="$SCRIPT_DIR/mttkrp_simple_test.mlir"

if [ ! -f "$TEST_FILE" ]; then
    echo "Error: Test file not found: $TEST_FILE"
    exit 1
fi

echo "  Input file: $TEST_FILE"
echo "  Running analysis pass..."

# Run with debug output to see what's happening
if $SYSTOLIC_OPT "$TEST_FILE" \
    -systolic-dataflow-generation \
    -o "$TEST_OUTPUT_DIR/simple_output.mlir" \
    2>&1 | tee "$TEST_OUTPUT_DIR/analysis.log"; then
    echo "  ✓ Pass completed"
    
    # Check output file
    if [ -f "$TEST_OUTPUT_DIR/simple_output.mlir" ]; then
        echo "  ✓ Output file generated"
        
        # Check for reordering attributes
        if grep -q "systolic.reorder" "$TEST_OUTPUT_DIR/simple_output.mlir" 2>/dev/null; then
            echo "  ✓ Reordering attributes found:"
            grep "systolic.reorder" "$TEST_OUTPUT_DIR/simple_output.mlir" | head -5 | sed 's/^/    /'
        else
            echo "  ⚠ No reordering attributes found"
            echo "  Checking function attributes..."
            grep -A 3 "func.func" "$TEST_OUTPUT_DIR/simple_output.mlir" | head -10
        fi
    else
        echo "  ✗ Output file not generated"
    fi
    
    # Check analysis log for debug messages
    if grep -q "Reordering\|non-linear\|Non-linear" "$TEST_OUTPUT_DIR/analysis.log" 2>/dev/null; then
        echo "  Analysis log messages:"
        grep -i "reordering\|non-linear" "$TEST_OUTPUT_DIR/analysis.log" | head -5 | sed 's/^/    /'
    fi
else
    echo "  ✗ Pass failed"
    echo "  Error log:"
    tail -20 "$TEST_OUTPUT_DIR/analysis.log" 2>/dev/null || echo "  (no log file)"
    exit 1
fi

# Test 2: Check if the analysis correctly identifies array names
echo ""
echo "Test 2: Checking array name detection..."
if grep -q "arg0\|arg1" "$TEST_OUTPUT_DIR/simple_output.mlir" 2>/dev/null; then
    echo "  Array references found in output"
    grep "arg0\|arg1" "$TEST_OUTPUT_DIR/simple_output.mlir" | head -3 | sed 's/^/    /'
else
    echo "  ⚠ No array references found (may be transformed)"
fi

# Summary
echo ""
echo "=== Summary ==="
echo "Test files:"
echo "  - Input: $TEST_FILE"
echo "  - Output: $TEST_OUTPUT_DIR/simple_output.mlir"
echo "  - Log: $TEST_OUTPUT_DIR/analysis.log"
echo ""
echo "Next steps:"
echo "  1. Check if reordering attributes are generated"
echo "  2. Verify array name detection works correctly"
echo "  3. Test with full MTTKRP kernel"
echo ""


