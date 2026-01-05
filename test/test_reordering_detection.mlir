// Test case specifically for write-time reordering detection
// This test has a clear non-linear access pattern that should trigger reordering

module attributes {systolic.problem_size = [16, 2, 16]} {
  func.func @kernel0(%A: memref<16x2x16xf32>) {
    // Read: non-linear access pattern (8 * k / 16)
    // This should trigger reordering: [16][2][16] -> [2][16][16]
    // The non-linear index is in the third dimension (dim2)
    affine.for %i = 0 to 16 {
      affine.for %j = 0 to 2 {
        affine.for %k = 0 to 32 {
          // Non-linear index expression: (k * 8) floordiv 16
          // This accesses A[%i, %j, (k * 8) / 16]
          // The third dimension has non-linear access
          %idx = affine.apply affine_map<(d0) -> (d0 * 8 floordiv 16)>(%k)
          %val = affine.load %A[%i, %j, %idx] : memref<16x2x16xf32>
        }
      }
    }
    
    return
  }
}

