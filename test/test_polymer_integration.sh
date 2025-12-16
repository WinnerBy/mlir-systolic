#!/bin/bash
# Test script for Polymer integration

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=== Testing Polymer Integration ==="
echo ""

# Check if systolic-opt exists
if [ ! -f "$BUILD_DIR/bin/systolic-opt" ]; then
    echo "ERROR: systolic-opt not found. Please build the project first."
    exit 1
fi

echo "1. Testing basic transformation..."
echo "-----------------------------------"
"$BUILD_DIR/bin/systolic-opt" "$PROJECT_ROOT/test/matmul/matmul.mlir" \
    --systolic-transform 2>&1 | head -20

echo ""
echo "2. Checking Polymer availability..."
echo "-----------------------------------"
if "$BUILD_DIR/bin/systolic-opt" --help 2>&1 | grep -q "polymer"; then
    echo "✓ Polymer option found"
else
    echo "⚠ Polymer option not found in help"
fi

echo ""
echo "3. Testing with debug output..."
echo "-----------------------------------"
"$BUILD_DIR/bin/systolic-opt" "$PROJECT_ROOT/test/matmul/matmul.mlir" \
    --systolic-transform \
    -debug-only=systolic-transform 2>&1 | grep -i "polymer\|dependence\|space" | head -10 || echo "No debug output found"

echo ""
echo "4. Checking if transformation succeeded..."
echo "-----------------------------------"
OUTPUT=$("$BUILD_DIR/bin/systolic-opt" "$PROJECT_ROOT/test/matmul/matmul.mlir" \
    --systolic-transform 2>&1)

if echo "$OUTPUT" | grep -q "PE array size"; then
    echo "✓ Transformation succeeded"
    echo "$OUTPUT" | grep "PE array size"
else
    echo "✗ Transformation may have failed"
fi

echo ""
echo "=== Test Complete ==="

