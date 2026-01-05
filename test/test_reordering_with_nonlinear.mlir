// Test case for write-time reordering with non-linear access pattern
// This simulates MTTKRP-like access pattern with non-linear index

module attributes {systolic.problem_size = [16, 2, 16]} {
  func.func @kernel0(%A: memref<16x2x16xf32>) {
    // Write pattern: linear access
    affine.for %i = 0 to 16 {
      affine.for %j = 0 to 2 {
        affine.for %k = 0 to 16 {
          %val = arith.constant 1.0 : f32
          affine.store %val, %A[%i, %j, %k] : memref<16x2x16xf32>
        }
      }
    }
    
    // Read pattern: non-linear access (8 * k / 16)
    affine.for %i = 0 to 16 {
      affine.for %j = 0 to 2 {
        affine.for %k = 0 to 32 {
          // Non-linear index: (k * 8) floordiv 16
          %idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%k)
          %val = affine.load %A[%i, %j, %idx] : memref<16x2x16xf32>
        }
      }
    }
    
    return
  }
}


