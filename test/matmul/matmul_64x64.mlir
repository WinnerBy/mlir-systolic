// RUN: systolic-opt %s -systolic-space-time-transform | FileCheck %s

// Test: 64x64x64 Matrix Multiplication
// Expected: 2D output-stationary systolic array with [i,j] as space loops

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

// CHECK: hls.dataflow.dispatch
// CHECK:   hls.dataflow.stream
// CHECK:   hls.dataflow.task
