//===----------------------------------------------------------------------===//
// Matrix Multiplication Test Case
//===----------------------------------------------------------------------===//
//
// This test case represents a simple matrix multiplication:
//   C[i][j] += A[i][k] * B[k][j]
//
// This corresponds to the SCoP region in AutoSA's kernel.c:
//   #pragma scop
//   for (int i = 0; i < I; i++)
//     for (int j = 0; j < J; j++) {
//       for (int k = 0; k < K; k++) {
//         C[i][j] = C[i][j] + A[i][k] * B[k][j];
//       }
//     }
//   #pragma endscop
//
// For testing, we use small dimensions: I=32, J=32, K=32
//===----------------------------------------------------------------------===//

module {
  func.func @matmul(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
    // Matrix multiplication: C[i][j] += A[i][k] * B[k][j]
    affine.for %i = 0 to 32 {
      affine.for %j = 0 to 32 {
        affine.for %k = 0 to 32 {
          %a = affine.load %A[%i, %k] : memref<32x32xf32>
          %b = affine.load %B[%k, %j] : memref<32x32xf32>
          %c = affine.load %C[%i, %j] : memref<32x32xf32>
          %mul = arith.mulf %a, %b : f32
          %add = arith.addf %c, %mul : f32
          affine.store %add, %C[%i, %j] : memref<32x32xf32>
        }
      }
    }
    return
  }
}

