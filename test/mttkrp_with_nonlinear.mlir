// MTTKRP kernel with non-linear access pattern (simulating AutoSA's generated code)
// This test case simulates the non-linear access pattern found in AutoSA's generated code:
//   local_A[c8][c5][8 * c6 / 16]
//   local_C[c7][8 * c6 / 16]
//
// The non-linear access pattern should trigger write-time reordering analysis

module attributes {systolic.problem_size = [128, 64, 128]} {
  func.func @kernel_mttkrp_nonlinear(
      %A: memref<128x64x128xf32>,  // A[I][K][L] = A[128][64][128]
      %B: memref<64x64xf32>,        // B[K][J] = B[64][64]
      %C: memref<64x128xf32>,       // C[J][L] = C[64][128]
      %D: memref<128x64xf32>        // D[I][J] = D[128][64]
  ) {
    // Simulate the non-linear access pattern from AutoSA's generated code
    // This pattern appears in IO_L2_in_intra_trans functions
    
    // Pattern 1: A array with non-linear access in third dimension
    // Similar to: local_A[c8][c5][8 * c6 / 16]
    // Write: linear access (inter_trans)
    affine.for %i = 0 to 128 {
      affine.for %k = 0 to 64 {
        affine.for %l = 0 to 128 {
          %val_a = arith.constant 1.0 : f32
          affine.store %val_a, %A[%i, %k, %l] : memref<128x64x128xf32>
        }
      }
    }
    
    // Read: non-linear access (intra_trans) - this should trigger reordering
    // Simulating: local_A[c8][c5][8 * c6 / 16]
    // where c8 = i, c5 = k, c6 = l, and the third dim uses 8 * l / 16
    affine.for %i = 0 to 128 {
      affine.for %k = 0 to 64 {
        affine.for %l = 0 to 256 {
          // Non-linear index: (l * 8) floordiv 16
          // This creates a non-linear access pattern in the third dimension
          %idx_l = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%l)
          %a_val = affine.load %A[%i, %k, %idx_l] : memref<128x64x128xf32>
        }
      }
    }
    
    // Pattern 2: C array with non-linear access in second dimension
    // Similar to: local_C[c7][8 * c6 / 16]
    // Write: linear access
    affine.for %j = 0 to 64 {
      affine.for %l = 0 to 128 {
        %val_c = arith.constant 1.0 : f32
        affine.store %val_c, %C[%j, %l] : memref<64x128xf32>
      }
    }
    
    // Read: non-linear access - this should trigger reordering for 2D arrays (future work)
    affine.for %j = 0 to 64 {
      affine.for %l = 0 to 256 {
        %idx_l2 = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%l)
        %c_val = affine.load %C[%j, %idx_l2] : memref<64x128xf32>
      }
    }
    
    // Main computation using the loaded values
    affine.for %i = 0 to 128 {
      affine.for %j = 0 to 64 {
        affine.for %k = 0 to 64 {
          affine.for %l = 0 to 128 {
            %a = affine.load %A[%i, %k, %l] : memref<128x64x128xf32>
            %b = affine.load %B[%k, %j] : memref<64x64xf32>
            %c = affine.load %C[%j, %l] : memref<64x128xf32>
            
            %d_old = affine.load %D[%i, %j] : memref<128x64xf32>
            %prod1 = arith.mulf %a, %b : f32
            %prod2 = arith.mulf %prod1, %c : f32
            %d_new = arith.addf %d_old, %prod2 : f32
            affine.store %d_new, %D[%i, %j] : memref<128x64xf32>
          }
        }
      }
    }
    
    return
  }
}


