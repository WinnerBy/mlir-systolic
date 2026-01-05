// MTTKRP test with non-linear access pattern - Pure Affine IR
// This file should be used directly with systolic-dataflow-generation pass
// Based on AutoSA's pattern: local_A[c8][c5][8 * c6 / 16]

module attributes {systolic.problem_size = [16, 2, 16]} {
  func.func @kernel_mttkrp_affine(
      %A: memref<16x2x16xf32>  // A[I][K][L] = A[16][2][16]
  ) {
    // Read pattern: non-linear access (intra_trans phase)
    // This simulates reading from local_A in intra_trans with non-linear index
    // Pattern: local_A[c8][c5][8 * c6 / 16]
    // where c8 = i, c5 = k, c6 = l
    // This is the pattern that should trigger write-time reordering
    affine.for %i = 0 to 16 {
      affine.for %k = 0 to 2 {
        affine.for %l = 0 to 32 {
          // Non-linear index: (l * 8) floordiv 16
          // This creates the pattern: 8 * c6 / 16
          // When l goes from 0 to 31, idx goes: 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3
          %idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%l)
          %val = affine.load %A[%i, %k, %idx] : memref<16x2x16xf32>
        }
      }
    }
    
    return
  }
}


