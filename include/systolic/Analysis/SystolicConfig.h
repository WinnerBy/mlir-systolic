//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Systolic Array Configuration
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_SYSTOLICCONFIG_H
#define SYSTOLIC_ANALYSIS_SYSTOLICCONFIG_H

#include "systolic/Analysis/SpaceTimeAnalysis.h"
#include "llvm/ADT/SmallVector.h"

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Systolic Array Configuration
//===----------------------------------------------------------------------===//

/// Configuration parameters for systolic array generation.
struct SystolicConfig {
  //===--------------------------------------------------------------------===//
  // PE Array Dimensions
  //===--------------------------------------------------------------------===//
  
  unsigned numPE_I;     // Number of PEs in row dimension
  unsigned numPE_J;     // Number of PEs in column dimension
  
  //===--------------------------------------------------------------------===//
  // Tiling Parameters
  //===--------------------------------------------------------------------===//
  
  /// Array partitioning factors (first-level tiling)
  /// Maps loop indices to tile sizes
  llvm::SmallVector<int64_t, 3> arrayPart;
  
  /// Latency hiding factors (second-level tiling)
  llvm::SmallVector<int64_t, 3> latency;
  
  //===--------------------------------------------------------------------===//
  // SIMD Configuration
  //===--------------------------------------------------------------------===//
  
  unsigned simdWidth;   // SIMD vectorization width (1 = no SIMD)
  
  //===--------------------------------------------------------------------===//
  // I/O Configuration
  //===--------------------------------------------------------------------===//
  
  unsigned ddrBitWidth;         // DDR port bit width (32, 64, 128, 256, 512)
  bool enableDoubleBuffer;      // Enable ping-pong buffering
  unsigned numIOLevels;         // Number of I/O hierarchy levels (L1, L2, L3)
  
  //===--------------------------------------------------------------------===//
  // Space-Time Mapping
  //===--------------------------------------------------------------------===//
  
  /// Space loop indices (from SpaceTimeInfo)
  llvm::SmallVector<unsigned, 2> spaceLoops;
  
  /// Time loop indices (from SpaceTimeInfo)
  llvm::SmallVector<unsigned, 3> timeLoops;
  
  /// Data flow directions for each operand
  llvm::DenseMap<Value, SystolicFlowDir> operandFlows;
  
  //===--------------------------------------------------------------------===//
  // Constructors
  //===--------------------------------------------------------------------===//
  
  SystolicConfig()
      : numPE_I(1), numPE_J(1), simdWidth(1), ddrBitWidth(32),
        enableDoubleBuffer(true), numIOLevels(3) {}
  
  /// Create configuration from SpaceTimeInfo and user hints.
  static SystolicConfig fromSpaceTimeInfo(
      const SpaceTimeInfo &info,
      ArrayRef<int64_t> arrayPartHints,
      ArrayRef<int64_t> latencyHints,
      unsigned simd = 1);
  
  //===--------------------------------------------------------------------===//
  // Validation
  //===--------------------------------------------------------------------===//
  
  /// Validate the configuration is consistent.
  bool isValid() const;
  
  /// Check if this is a 2D array configuration.
  bool is2DArray() const { return numPE_J > 1 && numPE_I > 1; }
  
  /// Get total number of PEs.
  unsigned getTotalPEs() const { return numPE_I * numPE_J; }
  
  //===--------------------------------------------------------------------===//
  // Debug
  //===--------------------------------------------------------------------===//
  
  void dump() const;
};

//===----------------------------------------------------------------------===//
// Configuration Presets
//===----------------------------------------------------------------------===//

/// Create a default MatMul configuration.
/// Space loops: [i, j], Time loop: [k]
/// Output-stationary dataflow.
SystolicConfig createMatMulConfig(
    int64_t M, int64_t N, int64_t K,
    ArrayRef<int64_t> arrayPart,    // e.g., {16, 16, 16}
    ArrayRef<int64_t> latency,       // e.g., {8, 8}
    unsigned simd = 1);

/// Create a configuration for 1D systolic array.
SystolicConfig create1DConfig(
    int64_t problemSize,
    int64_t arrayPart,
    int64_t latency,
    unsigned simd = 1);

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_SYSTOLICCONFIG_H
