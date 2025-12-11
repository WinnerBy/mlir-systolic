// RUN: systolic-opt %s --systolic-transform | FileCheck %s

// Matrix multiplication test case
// C[I,J] = A[I,K] * B[K,J]
// This matches AutoSA's mm_st3_I32_J32_K32 configuration

module {
  func.func @matmul(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
    // Perfect loop nest for matrix multiplication
    // i: 0->32, j: 0->32, k: 0->32
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

// CHECK: func.func @matmul
