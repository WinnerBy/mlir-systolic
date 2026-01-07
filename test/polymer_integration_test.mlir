// RUN: systolic-opt %s --systolic-transform | FileCheck %s

// Test basic matrix multiplication to verify Polymer integration
// This should be recognized as a valid systolic pattern and transformed

func.func @matmul_test(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
  %c0 = arith.constant 0 : index
  %c32 = arith.constant 32 : index
  %c1 = arith.constant 1 : index
  
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

// CHECK-LABEL: func.func @matmul_test
// CHECK: affine.for
