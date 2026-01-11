#!/bin/bash
# Verbose test script for spacetime enumeration functionality

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="${BUILD_DIR:-$PROJECT_ROOT/build}"

SYSTOLIC_OPT="$BUILD_DIR/bin/systolic-opt"
TEST_FILE="$SCRIPT_DIR/spacetime-enumeration.mlir"

echo "=========================================="
echo "Testing Spacetime Enumeration (Verbose)"
echo "=========================================="
echo ""

# Check if systolic-opt exists
if [ ! -f "$SYSTOLIC_OPT" ]; then
    echo "ERROR: systolic-opt not found at $SYSTOLIC_OPT"
    exit 1
fi

echo "Running: $SYSTOLIC_OPT $TEST_FILE --systolic-transform"
echo ""

# Run and capture output
OUTPUT=$($SYSTOLIC_OPT "$TEST_FILE" --systolic-transform 2>&1)

echo "$OUTPUT"
echo ""

# Check for enumeration output
if echo "$OUTPUT" | grep -q "Spacetime enumeration: Found"; then
    ENUM_COUNT=$(echo "$OUTPUT" | grep -o "Found [0-9]* configurations" | grep -o "[0-9]*")
    SELECTED=$(echo "$OUTPUT" | grep -o "selected \[[0-9]*\]" | grep -o "[0-9]*")
    ST_TYPE=$(echo "$OUTPUT" | grep -o "selected \[[0-9]*\] ST[0-9]" | grep -o "ST[0-9]")
    
    echo "=========================================="
    echo "Test Results:"
    echo "=========================================="
    echo "✓ Enumeration successful"
    echo "  - Found $ENUM_COUNT configurations"
    echo "  - Selected configuration [$SELECTED]: $ST_TYPE"
    
    if [ "$ENUM_COUNT" = "6" ]; then
        echo "✓ Correct number of configurations for 3-loop kernel (expected 6)"
    else
        echo "⚠ Unexpected number of configurations: $ENUM_COUNT (expected 6)"
    fi
    
    if [ "$ST_TYPE" = "ST3" ]; then
        echo "✓ Correct default selection: ST3 (expected for 3-loop MM)"
    else
        echo "⚠ Unexpected selection: $ST_TYPE (expected ST3)"
    fi
else
    echo "=========================================="
    echo "Test Results:"
    echo "=========================================="
    echo "✗ Enumeration output not found"
    echo ""
    echo "Full output saved for inspection"
fi

echo ""
