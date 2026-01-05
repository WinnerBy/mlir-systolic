// MTTKRP (Matricized Tensor Times Khatri-Rao Product) kernel
// Based on: https://github.com/UCLA-VAST/AutoSA/blob/master/autosa_tests/large/mttkrp/kernel.c
// Computation: D(i,j) += A(i,k,l) * B(k,j) * C(l,j)
// Input: A[I][K][L], B[K][J], C[L][J]
// Output: D[I][J]
//
// This test case includes non-linear access patterns that should trigger write-time reordering
// Similar to AutoSA's generated code: local_A[c8][c5][8 * c6 / 16]

module attributes {systolic.problem_size = [128, 64, 128]} {
  func.func @kernel_mttkrp(
      %A: memref<128x64x128xf32>,  // A[I][K][L] = A[128][64][128]
      %B: memref<64x64xf32>,        // B[K][J] = B[64][64]
      %C: memref<64x128xf32>,       // C[J][L] = C[64][128] (transposed from C[L][J])
      %D: memref<128x64xf32>        // D[I][J] = D[128][64]
  ) {
    // Initialize D to zero
    affine.for %i = 0 to 128 {
      affine.for %j = 0 to 64 {
        %zero = arith.constant 0.0 : f32
        affine.store %zero, %D[%i, %j] : memref<128x64xf32>
      }
    }
    
    // Main computation: D(i,j) += A(i,k,l) * B(k,j) * C(j,l)
    // This matches the kernel.c computation pattern
    affine.for %i = 0 to 128 {
      affine.for %j = 0 to 64 {
        affine.for %k = 0 to 64 {
          affine.for %l = 0 to 128 {
            // Load operands
            %a = affine.load %A[%i, %k, %l] : memref<128x64x128xf32>
            %b = affine.load %B[%k, %j] : memref<64x64xf32>
            %c = affine.load %C[%j, %l] : memref<64x128xf32>
            
            // Load current D value
            %d_old = affine.load %D[%i, %j] : memref<128x64xf32>
            
            // Compute: A * B * C
            %prod1 = arith.mulf %a, %b : f32
            %prod2 = arith.mulf %prod1, %c : f32
            
            // Accumulate: D += product
            %d_new = arith.addf %d_old, %prod2 : f32
            
            // Store result
            affine.store %d_new, %D[%i, %j] : memref<128x64xf32>
          }
        }
      }
    }
    
    return
  }
}


