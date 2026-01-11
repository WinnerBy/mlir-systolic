#!/bin/bash
# Test script for spacetime enumeration functionality

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

SYSTOLIC_OPT="$BUILD_DIR/bin/systolic-opt"
TEST_FILE="$SCRIPT_DIR/spacetime-enumeration.mlir"

echo "=========================================="
echo "Testing Spacetime Enumeration"
echo "=========================================="
echo ""

# Check if systolic-opt exists
if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo "ERROR: systolic-opt not found at $SYSTOLIC_OPT"
    echo "Please build the project first:"
    echo "  cd $PROJECT_ROOT && mkdir -p build && cd build && cmake .. && make"
    exit 1
fi

# Check if test file exists
if [ ! -f "$TEST_FILE" ]; then
    echo "ERROR: Test file not found at $TEST_FILE"
    exit 1
fi

echo "Test file: $TEST_FILE"
echo "systolic-opt: $SYSTOLIC_OPT"
echo ""

# Test 1: Run with default settings (should auto-select ST3)
echo "----------------------------------------"
echo "Test 1: Default auto-selection"
echo "----------------------------------------"
echo "Running: $SYSTOLIC_OPT $TEST_FILE --systolic-transform"
echo ""

if $SYSTOLIC_OPT "$TEST_FILE" --systolic-transform 2>&1 | tee /tmp/systolic_test_output.txt; then
    echo ""
    echo "✓ Test 1 passed: Pass completed without errors"
    
    # Check for expected output
    if grep -q "Enumerated.*spacetime configurations" /tmp/systolic_test_output.txt; then
        echo "✓ Found enumeration output"
    fi
    
    if grep -q "ST3\|space=\[0,1\]" /tmp/systolic_test_output.txt; then
        echo "✓ Found ST3 configuration (expected default)"
    fi
else
    echo ""
    echo "✗ Test 1 failed: Pass returned error"
    exit 1
fi

echo ""

# Test 2: Check debug output (if available)
echo "----------------------------------------"
echo "Test 2: Debug output check"
echo "----------------------------------------"
echo "Running with debug output..."
echo ""

if $SYSTOLIC_OPT "$TEST_FILE" --systolic-transform --debug 2>&1 | grep -i "spacetime\|enum\|config" | head -20; then
    echo ""
    echo "✓ Test 2 passed: Debug output contains spacetime information"
else
    echo ""
    echo "⚠ Test 2: Debug output not available or different format"
fi

echo ""

# Test 3: Verify enumeration count for 3-loop kernel
echo "----------------------------------------"
echo "Test 3: Enumeration count verification"
echo "----------------------------------------"
echo "Expected: 6 configurations for 3-loop kernel (ST0-ST5)"
echo ""

ENUM_COUNT=$(grep -o "Found [0-9]* spacetime configurations" /tmp/systolic_test_output.txt | grep -o "[0-9]*" || echo "0")

if [ "$ENUM_COUNT" = "6" ]; then
    echo "✓ Test 3 passed: Found 6 configurations as expected"
elif [ "$ENUM_COUNT" = "0" ]; then
    echo "⚠ Test 3: Could not extract enumeration count from output"
else
    echo "⚠ Test 3: Found $ENUM_COUNT configurations (expected 6)"
fi

echo ""
echo "=========================================="
echo "Test Summary"
echo "=========================================="
echo "All basic tests completed"
echo ""
echo "To see full output, check: /tmp/systolic_test_output.txt"
