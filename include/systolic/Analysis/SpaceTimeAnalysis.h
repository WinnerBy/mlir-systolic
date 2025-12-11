//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Space-Time Analysis
// 
// This module provides polyhedral analysis for systolic array generation,
// using Polymer/ISL for dependence analysis and space loop selection.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_SPACETIMEANALYSIS_H
#define SYSTOLIC_ANALYSIS_SPACETIMEANALYSIS_H

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/IR/Value.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Data Flow Direction
//===----------------------------------------------------------------------===//

/// Describes the systolic flow direction for a data operand.
enum class SystolicFlowDir {
  NONE,               // No systolic flow, local access (e.g., C in output-stationary)
  HORIZONTAL,         // Data flows along j-axis (PE[i,j] -> PE[i,j+1])
  VERTICAL,           // Data flows along i-axis (PE[i,j] -> PE[i+1,j])
  REDUCE_HORIZONTAL,  // Reduction flows horizontally
  REDUCE_VERTICAL,    // Reduction flows vertically
};

//===----------------------------------------------------------------------===//
// Dependence Distance Info
//===----------------------------------------------------------------------===//

/// Represents the dependence distance for a loop dimension.
struct DependenceDistance {
  int64_t minDistance;  // Minimum dependence distance
  int64_t maxDistance;  // Maximum dependence distance
  bool isUniform;       // True if distance is constant
  
  /// Check if this loop can be a space loop (distance <= 1)
  bool canBeSpaceLoop() const {
    return maxDistance <= 1 && minDistance >= -1;
  }
};

//===----------------------------------------------------------------------===//
// Space-Time Analysis Result
//===----------------------------------------------------------------------===//

/// Result of space-time analysis for an affine loop nest.
struct SpaceTimeInfo {
  /// The analyzed loop nest (outermost loop)
  affine::AffineForOp outerLoop;
  
  /// Total number of loops in the nest
  unsigned numLoops;
  
  /// Dependence distance for each loop (indexed by loop depth)
  llvm::SmallVector<DependenceDistance> dependenceDistances;
  
  /// Indices of loops that can be space loops (dependence distance <= 1)
  llvm::SmallVector<unsigned> spaceLoopCandidates;
  
  /// Selected space loop indices
  llvm::SmallVector<unsigned> selectedSpaceLoops;
  
  /// Time loop indices (remaining loops)
  llvm::SmallVector<unsigned> timeLoops;
  
  /// Data flow direction for each memory operand
  llvm::DenseMap<Value, SystolicFlowDir> operandFlows;
  
  /// Check if analysis is valid
  bool isValid() const { return outerLoop && numLoops > 0; }
  
  /// Get the number of space dimensions (1D or 2D array)
  unsigned getNumSpaceDims() const { return selectedSpaceLoops.size(); }
  
  /// Check if this is a 2D systolic array
  bool is2DArray() const { return getNumSpaceDims() == 2; }
  
  /// Print for debugging
  void dump() const;
};

//===----------------------------------------------------------------------===//
// Analysis Interface
//===----------------------------------------------------------------------===//

/// Analyze an affine loop nest for space-time properties.
/// This is the main entry point for polyhedral analysis.
///
/// The analysis:
/// 1. Extracts the loop nest structure
/// 2. Computes dependence distances using Polymer/ISL
/// 3. Identifies loops that can be space loops
/// 4. Selects the best space-time mapping
///
/// Returns failure if the loop nest cannot be analyzed or is not suitable
/// for systolic array mapping.
LogicalResult analyzeSpaceTime(affine::AffineForOp outerLoop,
                                SpaceTimeInfo &result);

/// Verify that a given space loop selection is legal.
/// A selection is legal if all selected loops have dependence distance <= 1.
bool verifySpaceLoopSelection(const SpaceTimeInfo &info,
                               ArrayRef<unsigned> spaceLoops);

/// Select space loops automatically based on heuristics.
/// For MatMul-like patterns, prefers [i,j] (output-stationary).
LogicalResult selectSpaceLoops(SpaceTimeInfo &info,
                                unsigned numSpaceDims);

/// Analyze data flow directions for operands based on space loop selection.
LogicalResult analyzeDataFlow(SpaceTimeInfo &info);

//===----------------------------------------------------------------------===//
// Utility Functions
//===----------------------------------------------------------------------===//

/// Get the loop nest depth of an affine loop.
unsigned getLoopNestDepth(affine::AffineForOp loop);

/// Get all loops in a perfect loop nest.
void getLoopNest(affine::AffineForOp outerLoop,
                  llvm::SmallVectorImpl<affine::AffineForOp> &loops);

/// Check if a loop nest is perfectly nested.
bool isPerfectlyNested(affine::AffineForOp outerLoop);

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_SPACETIMEANALYSIS_H
