// MatMul 32x32 for systolic array translation
// Configuration: I=32, J=32, K=32
// PE array size: 2x2 (with array_part=8, latency=4)
// This matches AutoSA's mm_st3_I32_J32_K32_ap8_lat4_simd1

module attributes {systolic.problem_size = [32, 32, 32]} {
  func.func @kernel_matmul(%A: memref<32x32xf32>, %B: memref<32x32xf32>, %C: memref<32x32xf32>) {
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
