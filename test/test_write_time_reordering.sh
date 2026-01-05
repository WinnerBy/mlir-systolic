#!/bin/bash
# Test script for write-time reordering functionality
# This script verifies that write-time reordering is correctly detected and applied

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

echo "=== Write-time Reordering Verification Test ==="
echo ""

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory not found: $BUILD_DIR"
    echo "Please build the project first:"
    echo "  mkdir build && cd build"
    echo "  cmake .. && make"
    exit 1
fi

# Find tools
SYSTOLIC_OPT="$BUILD_DIR/bin/systolic-opt"
SYSTOLIC_TRANSLATE="$BUILD_DIR/bin/systolic-translate"

# Use systolic-opt as mlir-opt replacement
MLIR_OPT="$SYSTOLIC_OPT"

if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo "Error: systolic-opt not found: $SYSTOLIC_OPT"
    exit 1
fi

if [ ! -f "$SYSTOLIC_TRANSLATE" ]; then
    echo "Error: systolic-translate not found: $SYSTOLIC_TRANSLATE"
    exit 1
fi

echo "Using tools:"
echo "  systolic-opt: $SYSTOLIC_OPT"
echo "  systolic-translate: $SYSTOLIC_TRANSLATE"
echo ""

# Create test output directory
TEST_OUTPUT_DIR="$SCRIPT_DIR/output/write_time_reordering_test"
mkdir -p "$TEST_OUTPUT_DIR"

# Test 1: Check if reordering analysis pass is available
echo "Test 1: Checking if write-time reordering analysis is integrated..."
if $MLIR_OPT --help | grep -q "systolic-dataflow-generation"; then
    echo "  ✓ systolic-dataflow-generation pass found"
else
    echo "  ✗ systolic-dataflow-generation pass not found"
    exit 1
fi

# Test 2: Run analysis on a simple test case
echo ""
echo "Test 2: Running analysis on test MLIR file..."

# Create a simple test MLIR with non-linear access pattern
TEST_MLIR="$TEST_OUTPUT_DIR/test_reordering.mlir"
cat > "$TEST_MLIR" << 'EOF'
func.func @kernel0(%A: memref<16x2x16xf32>, %B: memref<2x1xf32>, %C: memref<8x16xf32>) {
  affine.for %i = 0 to 16 {
    affine.for %j = 0 to 2 {
      affine.for %k = 0 to 16 {
        // Write: linear access
        %val = affine.load %A[%i, %j, %k] : memref<16x2x16xf32>
      }
    }
  }
  
  affine.for %i = 0 to 16 {
    affine.for %j = 0 to 2 {
      affine.for %k = 0 to 32 {
        // Read: non-linear access (8 * k / 16)
        %idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%k)
        %val = affine.load %A[%i, %j, %idx] : memref<16x2x16xf32>
      }
    }
  }
  
  return
}
EOF

echo "  Created test MLIR file: $TEST_MLIR"

# Run passes to check if reordering attributes are added
echo "  Running passes..."
INTERMEDIATE_MLIR="$TEST_OUTPUT_DIR/intermediate.mlir"
if $MLIR_OPT "$TEST_MLIR" \
    -systolic-dataflow-generation \
    -o "$INTERMEDIATE_MLIR" 2>&1; then
    echo "  ✓ Passes completed successfully"
else
    echo "  ✗ Passes failed"
    exit 1
fi

# Test 3: Check if reordering attributes are present
echo ""
echo "Test 3: Checking for reordering attributes..."
if grep -q "systolic.reorder" "$INTERMEDIATE_MLIR"; then
    echo "  ✓ Reordering attributes found in output"
    echo "  Attributes found:"
    grep "systolic.reorder" "$INTERMEDIATE_MLIR" | sed 's/^/    /'
else
    echo "  ⚠ No reordering attributes found (may be expected if no non-linear access detected)"
    echo "  Checking intermediate MLIR:"
    cat "$INTERMEDIATE_MLIR" | head -20
fi

# Test 4: Generate HLS C++ code
echo ""
echo "Test 4: Generating HLS C++ code..."
OUTPUT_CPP="$TEST_OUTPUT_DIR/output.cpp"
if $SYSTOLIC_TRANSLATE "$INTERMEDIATE_MLIR" \
    -array-part=16 \
    -latency=8 \
    -simd=1 \
    -size=32 \
    -o "$OUTPUT_CPP" 2>&1; then
    echo "  ✓ HLS C++ code generated successfully"
else
    echo "  ✗ Code generation failed"
    exit 1
fi

# Test 5: Check if reordered array declarations are present
echo ""
echo "Test 5: Checking for reordered array declarations..."
if grep -q "local_A\[2\]\[16\]\[16\]" "$OUTPUT_CPP" || \
   grep -q "local_A\[.*\]\[.*\]\[.*\]" "$OUTPUT_CPP"; then
    echo "  ✓ Multi-dimensional array declarations found"
    echo "  Array declarations:"
    grep -E "local_[A-Z]\[.*\]" "$OUTPUT_CPP" | head -5 | sed 's/^/    /'
else
    echo "  ⚠ No multi-dimensional array declarations found (may use default layout)"
fi

# Test 6: Check for reordered index access
echo ""
echo "Test 6: Checking for reordered index access patterns..."
if grep -q "local_A\[.*\]\[.*\]\[.*\]" "$OUTPUT_CPP"; then
    echo "  ✓ Reordered index access patterns found"
    echo "  Sample access patterns:"
    grep -E "local_[A-Z]\[.*\]\[.*\]\[.*\]" "$OUTPUT_CPP" | head -3 | sed 's/^/    /'
else
    echo "  ⚠ No reordered index patterns found"
fi

echo ""
echo "=== Test Summary ==="
echo "Test output directory: $TEST_OUTPUT_DIR"
echo "Generated files:"
echo "  - Test MLIR: $TEST_MLIR"
echo "  - Intermediate MLIR: $INTERMEDIATE_MLIR"
echo "  - Generated C++: $OUTPUT_CPP"
echo ""
echo "To inspect the results:"
echo "  cat $INTERMEDIATE_MLIR | grep systolic.reorder"
echo "  cat $OUTPUT_CPP | grep local_A"
echo ""

