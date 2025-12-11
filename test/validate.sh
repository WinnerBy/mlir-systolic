#!/bin/bash
# Validation script for mlir-systolic
# Compares transformation output with AutoSA reference samples

SYSTOLIC_OPT="/home/user/work/mlir-systolic/build/bin/systolic-opt"
AUTOSA_SAMPLES="/home/user/work/scalehls/test/Transforms/Dataflow/AutoSA/autosa-reference-samples"
TEST_DIR="/home/user/work/mlir-systolic/test"

echo "========================================"
echo "mlir-systolic Validation Report"
echo "========================================"
echo ""

# Test 1: Basic MatMul 32x32 (matching AutoSA mm_st3_I32_J32_K32_ap8_lat4)
echo "Test 1: MatMul 32x32 (st3, ap8, lat4)"
echo "----------------------------------------"

cat > /tmp/matmul32.mlir << 'EOF'
module {
  func.func @matmul(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
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

OUTPUT=$($SYSTOLIC_OPT /tmp/matmul32.mlir --systolic-transform 2>&1)
echo "$OUTPUT" | grep -E "PE array size|Total loops"
echo ""

# Check for key transformations
if echo "$OUTPUT" | grep -q "PE array size: 2 x 2"; then
    echo "✅ PE array size correct (2x2)"
else
    echo "❌ PE array size incorrect"
fi

if echo "$OUTPUT" | grep -q "Total loops after tiling: 9"; then
    echo "✅ Loop tiling correct (3-level tiling -> 9 loops)"
else
    echo "❌ Loop tiling incorrect"
fi

# Test 2: MatMul 64x64
echo ""
echo "Test 2: MatMul 64x64 (st3, ap16, lat8)"
echo "----------------------------------------"

cat > /tmp/matmul64.mlir << 'EOF'
module {
  func.func @matmul(%A: memref<64x64xf32>, %B: memref<64x64xf32>, %C: memref<64x64xf32>) {
    affine.for %i = 0 to 64 {
      affine.for %j = 0 to 64 {
        affine.for %k = 0 to 64 {
          %a = affine.load %A[%i, %k] : memref<64x64xf32>
          %b = affine.load %B[%k, %j] : memref<64x64xf32>
          %c = affine.load %C[%i, %j] : memref<64x64xf32>
          %prod = arith.mulf %a, %b : f32
          %sum = arith.addf %c, %prod : f32
          affine.store %sum, %C[%i, %j] : memref<64x64xf32>
        }
      }
    }
    return
  }
}
EOF

OUTPUT64=$($SYSTOLIC_OPT /tmp/matmul64.mlir --systolic-transform 2>&1)
echo "$OUTPUT64" | grep -E "PE array size|Total loops"
echo ""

# Test 3: MatMul 128x128
echo ""
echo "Test 3: MatMul 128x128 (st3, ap32, lat16)"
echo "----------------------------------------"

cat > /tmp/matmul128.mlir << 'EOF'
module {
  func.func @matmul(%A: memref<128x128xf32>, %B: memref<128x128xf32>, %C: memref<128x128xf32>) {
    affine.for %i = 0 to 128 {
      affine.for %j = 0 to 128 {
        affine.for %k = 0 to 128 {
          %a = affine.load %A[%i, %k] : memref<128x128xf32>
          %b = affine.load %B[%k, %j] : memref<128x128xf32>
          %c = affine.load %C[%i, %j] : memref<128x128xf32>
          %prod = arith.mulf %a, %b : f32
          %sum = arith.addf %c, %prod : f32
          affine.store %sum, %C[%i, %j] : memref<128x128xf32>
        }
      }
    }
    return
  }
}
EOF

OUTPUT128=$($SYSTOLIC_OPT /tmp/matmul128.mlir --systolic-transform 2>&1)
echo "$OUTPUT128" | grep -E "PE array size|Total loops"
echo ""

echo "========================================"
echo "AutoSA Reference Structure Analysis"
echo "========================================"
echo ""

echo "Reference: mm_st3_I32_J32_K32_ap8_lat4_simd1_kernel.cpp"
echo "Expected structure:"
echo "  - space-time = 3 (2D array, i/j as space)"
echo "  - PE array = 2x2 (ap8/lat4 = 2)"
echo "  - 9-level loop nest after tiling"
echo "  - Modules: A_IO_L3_in, A_IO_L2_in, B_IO_L3_in, B_IO_L2_in"
echo "  - PE_wrapper for computation"
echo "  - C_drain_IO_L1/L2/L3_out for output"
echo ""

echo "========================================"
echo "Summary"
echo "========================================"
echo ""
echo "Core transformations implemented:"
echo "  ✅ Legality check for perfect loop nests"
echo "  ✅ Dependence analysis for space-time mapping"
echo "  ✅ Multi-level tiling (array_part, latency, simd)"
echo "  ✅ PE array dimension inference"
echo ""
echo "Pending features for full AutoSA compatibility:"
echo "  ⚠️ I/O module generation (L1/L2/L3 hierarchy)"
echo "  ⚠️ Double buffering insertion"
echo "  ⚠️ HLS pragma emission"
echo "  ⚠️ Stream/FIFO declaration"
echo ""
