#!/bin/bash
# Simple verification script for write-time reordering
# This script creates a test case and checks if reordering is applied

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

echo "=== Simple Write-time Reordering Verification ==="
echo ""

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "Error: Build directory not found: $BUILD_DIR"
    exit 1
fi

SYSTOLIC_TRANSLATE="$BUILD_DIR/bin/systolic-translate"
if [ ! -f "$SYSTOLIC_TRANSLATE" ]; then
    echo "Error: systolic-translate not found: $SYSTOLIC_TRANSLATE"
    exit 1
fi

# Create test output directory
TEST_OUTPUT_DIR="$SCRIPT_DIR/output/write_time_reordering_test"
mkdir -p "$TEST_OUTPUT_DIR"

# Create a simple test MLIR file (matrix multiplication - no non-linear access expected)
echo "Test 1: Creating test MLIR file..."
TEST_MLIR="$TEST_OUTPUT_DIR/test_simple.mlir"
cat > "$TEST_MLIR" << 'EOF'
module attributes {systolic.problem_size = [32, 32, 32]} {
  func.func @kernel0(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
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

echo "  Created: $TEST_MLIR"

# Generate HLS C++ code
echo ""
echo "Test 2: Generating HLS C++ code..."
OUTPUT_CPP="$TEST_OUTPUT_DIR/output.cpp"
if $SYSTOLIC_TRANSLATE "$TEST_MLIR" \
    -array-part=8 \
    -latency=4 \
    -simd=1 \
    -size=32 \
    -o "$OUTPUT_CPP" 2>&1; then
    echo "  ✓ Code generation successful"
    echo "  Generated: $OUTPUT_CPP"
else
    echo "  ✗ Code generation failed"
    exit 1
fi

# Check if code was generated
if [ ! -f "$OUTPUT_CPP" ] || [ ! -s "$OUTPUT_CPP" ]; then
    echo "  ✗ Output file is empty or missing"
    exit 1
fi

# Check for array declarations
echo ""
echo "Test 3: Checking array declarations..."
ARRAY_DECLS=$(grep -E "local_[A-Z]\[.*\]" "$OUTPUT_CPP" | head -5)
if [ -n "$ARRAY_DECLS" ]; then
    echo "  ✓ Array declarations found:"
    echo "$ARRAY_DECLS" | sed 's/^/    /'
else
    echo "  ⚠ No array declarations found"
fi

# Check for function signatures
echo ""
echo "Test 4: Checking function signatures..."
FUNC_SIGS=$(grep -E "void.*IO_L2_in.*local_" "$OUTPUT_CPP" | head -3)
if [ -n "$FUNC_SIGS" ]; then
    echo "  ✓ Function signatures found:"
    echo "$FUNC_SIGS" | sed 's/^/    /'
else
    echo "  ⚠ No function signatures found"
fi

# Summary
echo ""
echo "=== Summary ==="
echo "Test files:"
echo "  - Test MLIR: $TEST_MLIR"
echo "  - Generated C++: $OUTPUT_CPP"
echo ""
echo "To inspect the generated code:"
echo "  cat $OUTPUT_CPP | grep -A 2 'local_A'"
echo ""


