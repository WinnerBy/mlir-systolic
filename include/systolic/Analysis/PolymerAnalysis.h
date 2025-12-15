//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Polymer Polyhedral Analysis Interface
//
// This module provides an interface to Polymer for polyhedral analysis,
// including SCoP extraction, schedule tree computation, and dependence
// analysis using ISL.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_POLYMERANALYSIS_H
#define SYSTOLIC_ANALYSIS_POLYMERANALYSIS_H

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "llvm/ADT/SmallVector.h"

// Forward declarations for ISL types
struct isl_schedule;
struct isl_union_map;
struct isl_union_set;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Polymer SCoP Wrapper
//===----------------------------------------------------------------------===//

/// Wrapper for Polymer's IslScop (Static Control Part)
/// This encapsulates the polyhedral representation of a loop nest.
class PolymerScop {
public:
  /// Construct from a function
  static std::unique_ptr<PolymerScop> extract(func::FuncOp func);
  
  /// Check if SCoP extraction was successful
  bool isValid() const { return scop != nullptr; }
  
  /// Get the underlying ISL SCoP (opaque pointer)
  void *getScop() const { return scop; }
  
  /// Compute the schedule tree for this SCoP
  isl_schedule *computeSchedule();
  
  /// Compute dependences
  isl_union_map *computeDependences();
  
  /// Compute dependence distances
  isl_union_map *computeDependenceDistances(isl_union_map *deps);
  
  ~PolymerScop();

private:
  PolymerScop(void *scop) : scop(scop) {}
  void *scop;  // Opaque pointer to polymer::IslScop or similar
};

//===----------------------------------------------------------------------===//
// Schedule Tree Analysis
//===----------------------------------------------------------------------===//

/// Information extracted from ISL schedule tree
struct ScheduleTreeInfo {
  /// The schedule tree (ISL object)
  isl_schedule *schedule;
  
  /// Dependence distances for each loop dimension
  SmallVector<int64_t, 8> dependenceDistances;
  
  /// Space loop candidates (loops with distance <= 1)
  SmallVector<unsigned, 4> spaceLoopCandidates;
  
  /// Check if analysis is valid
  bool isValid() const { return schedule != nullptr; }
  
  /// Cleanup ISL objects
  void release();
};

/// Analyze schedule tree to extract dependence information
LogicalResult analyzeScheduleTree(isl_schedule *schedule,
                                   ScheduleTreeInfo &info);

//===----------------------------------------------------------------------===//
// Dependence Distance Analysis
//===----------------------------------------------------------------------===//

/// Dependence distance for a specific loop dimension
struct LoopDependenceDistance {
  unsigned loopIndex;
  int64_t minDistance;
  int64_t maxDistance;
  bool isUniform;  // True if distance is constant
  bool canBeSpaceLoop;  // True if distance <= 1
};

/// Compute dependence distances for all loops in a function
/// This uses Polymer/ISL to perform accurate polyhedral analysis
LogicalResult computeDependenceDistancesWithPolymer(
    func::FuncOp func,
    SmallVectorImpl<LoopDependenceDistance> &distances);

//===----------------------------------------------------------------------===//
// Utility Functions
//===----------------------------------------------------------------------===//

/// Check if Polymer is available (compiled with Polymer support)
bool isPolymerAvailable();

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_POLYMERANALYSIS_H

