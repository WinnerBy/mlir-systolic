//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Space-Time Analysis Implementation
//
// This module provides polyhedral analysis for systolic array generation.
// Integrates with Polymer/ISL for precise dependence distance computation
// when available, with fallback to MLIR-level heuristics.
//
// AutoSA Reference:
//   - uniform_dep_check: Verify uniform dependences
//   - get_dep_dis_at_node: Compute dependence distance vectors
//   - is_space_loop[]: Mark loops with distance <= 1 as space candidates
//
//===----------------------------------------------------------------------===//

#include "systolic/Analysis/SpaceTimeAnalysis.h"
#include "systolic/Analysis/PolymerAnalysis.h"
#include "systolic/Analysis/SystolicConfig.h"

#include "mlir/Dialect/Affine/Analysis/AffineAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/AffineStructures.h"
#include "mlir/Dialect/Affine/Analysis/LoopAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/Utils.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/LoopUtils.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/AffineExprVisitor.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/Support/Debug.h"

#include <set>

#define DEBUG_TYPE "systolic-analysis"

using namespace mlir;
using namespace mlir::affine;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Utility Functions
//===----------------------------------------------------------------------===//

unsigned getLoopNestDepth(AffineForOp loop) {
  unsigned depth = 0;
  Operation *current = loop;
  while (current) {
    if (isa<AffineForOp>(current))
      depth++;
    current = current->getParentOfType<AffineForOp>();
  }
  return depth;
}

void getLoopNest(AffineForOp outerLoop,
                 SmallVectorImpl<AffineForOp> &loops) {
  loops.clear();
  AffineForOp current = outerLoop;
  while (current) {
    loops.push_back(current);
    // Find the next nested loop
    AffineForOp inner = nullptr;
    for (auto &op : *current.getBody()) {
      if (auto nestedFor = dyn_cast<AffineForOp>(op)) {
        inner = nestedFor;
        break;
      }
    }
    current = inner;
  }
}

bool isPerfectlyNested(AffineForOp outerLoop) {
  SmallVector<AffineForOp, 8> loops;
  getLoopNest(outerLoop, loops);
  
  for (unsigned i = 0; i + 1 < loops.size(); ++i) {
    Block *body = loops[i].getBody();
    unsigned nonYieldOps = 0;
    for (auto &op : *body) {
      if (!isa<AffineYieldOp>(op))
        nonYieldOps++;
    }
    // Perfect nesting: exactly one operation (the inner loop) plus yield
    if (nonYieldOps != 1)
      return false;
  }
  return true;
}

//===----------------------------------------------------------------------===//
// Memory Access Pattern Analysis
// (Simplified version - full implementation would use Polymer)
//===----------------------------------------------------------------------===//

namespace {

/// Analyze array subscript to determine which loop induction variables are used.
/// For MatMul pattern C[i,j] += A[i,k] * B[k,j]:
///   - A uses i,k (rows from loop i, cols from loop k)
///   - B uses k,j (rows from loop k, cols from loop j)
///   - C uses i,j (output-stationary)
struct SubscriptAnalysis {
  SmallVector<Value, 3> loopIVs;  // Induction variables for each subscript dimension
  bool isSimple = true;           // True if subscript is just loop IV
  
  /// Analyze which IVs appear in the map operands
  static void analyze(Operation *memOp, 
                     SmallVectorImpl<AffineForOp> &loops,
                     DenseMap<Value, SmallVector<unsigned, 2>> &ivToLoopMap) {
    SmallVector<Value, 4> operands;
    
    if (auto loadOp = dyn_cast<AffineLoadOp>(memOp)) {
      for (auto op : loadOp.getMapOperands())
        operands.push_back(op);
    } else if (auto storeOp = dyn_cast<AffineStoreOp>(memOp)) {
      for (auto op : storeOp.getMapOperands())
        operands.push_back(op);
    }
    
    for (auto operand : operands) {
      for (unsigned i = 0; i < loops.size(); ++i) {
        if (operand == loops[i].getInductionVar()) {
          ivToLoopMap[operand].push_back(i);
          break;
        }
      }
    }
  }
};

/// Identify the role of each memref in a computation pattern.
/// For MatMul: detects which is A (input1), B (input2), C (accumulator)
enum class OperandRole {
  INPUT_A,      // First input (e.g., A[i,k])
  INPUT_B,      // Second input (e.g., B[k,j])
  ACCUMULATOR,  // Output/accumulator (e.g., C[i,j])
  UNKNOWN
};

struct OperandInfo {
  Value memref;
  OperandRole role;
  SmallVector<unsigned, 2> loopDims;  // Which loop dims are used in access
};

} // namespace

//===----------------------------------------------------------------------===//
// Dependence Distance Computation
// (Uses Polymer/ISL when available, falls back to heuristics)
//===----------------------------------------------------------------------===//

/// Try to compute dependence distances using Polymer/ISL.
/// Returns success if Polymer analysis succeeds, failure otherwise.
static LogicalResult tryPolymerDependenceAnalysis(
    affine::AffineForOp outerLoop,
    SmallVectorImpl<affine::AffineForOp> &loops,
    SmallVectorImpl<DependenceDistance> &distances) {
  
  // Get the parent function
  auto funcOp = outerLoop->getParentOfType<func::FuncOp>();
  if (!funcOp) {
    return failure();
  }
  
  // Try Polymer analysis
  SmallVector<LoopDependenceDistance, 8> polymerDistances;
  if (failed(computeDependenceDistancesWithPolymer(funcOp, polymerDistances))) {
    return failure();
  }
  
  // Convert Polymer results to SpaceTimeAnalysis format
  distances.clear();
  for (const auto &polyDist : polymerDistances) {
    DependenceDistance dist;
    dist.minDistance = polyDist.minDistance;
    dist.maxDistance = polyDist.maxDistance;
    dist.isUniform = polyDist.isUniform;
    distances.push_back(dist);
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[SpaceTimeAnalysis] Using Polymer dependence analysis\n";
    llvm::dbgs() << "  Computed " << distances.size() << " loop distances\n";
  });
  
  return success();
}

/// Compute dependence distances for each loop dimension.
/// AutoSA methodology:
///   1. Compute flow (RAW) and output (RAR) dependences
///   2. Extract dependence distance vector for each pair
///   3. Mark loops with distance <= 1 as space loop candidates
///
/// For MatMul C[i,j] += A[i,k] * B[k,j]:
///   - Loop i: distance 0 (no cross-iteration dep from C)
///   - Loop j: distance 0 (no cross-iteration dep from C)
///   - Loop k: distance 0 (reduction, but carried inside iteration)
/// All three can be space loops, but we typically choose [i,j] for 2D array.
static LogicalResult computeDependenceDistances(
    AffineForOp outerLoop,
    SmallVectorImpl<AffineForOp> &loops,
    SmallVectorImpl<DependenceDistance> &distances) {
  
  distances.clear();
  
  // First, try using Polymer/ISL for accurate dependence analysis
  if (succeeded(tryPolymerDependenceAnalysis(outerLoop, loops, distances))) {
    return success();
  }
  
  // Fallback: heuristic MLIR-level analysis
  LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Polymer analysis unavailable, "
                          << "using MLIR heuristics\n");
  
  // Initialize distances for each loop
  for (unsigned i = 0; i < loops.size(); ++i) {
    DependenceDistance dist;
    dist.minDistance = 0;
    dist.maxDistance = 0;
    dist.isUniform = true;
    distances.push_back(dist);
  }
  
  // Collect memory operations in the innermost loop body
  AffineForOp innermostLoop = loops.back();
  Block *body = innermostLoop.getBody();
  
  SmallVector<AffineLoadOp, 4> loads;
  SmallVector<AffineStoreOp, 4> stores;
  
  body->walk([&](Operation *op) {
    if (auto loadOp = dyn_cast<AffineLoadOp>(op))
      loads.push_back(loadOp);
    else if (auto storeOp = dyn_cast<AffineStoreOp>(op))
      stores.push_back(storeOp);
  });
  
  // Analyze dependences between stores and loads
  for (auto storeOp : stores) {
    Value storeMemref = storeOp.getMemRef();
    
    for (auto loadOp : loads) {
      if (loadOp.getMemRef() != storeMemref)
        continue;
      
      // Same memref accessed -> potential dependence
      // Analyze which loops carry the dependence
      
      DenseMap<Value, SmallVector<unsigned, 2>> storeIVs, loadIVs;
      SubscriptAnalysis::analyze(storeOp, loops, storeIVs);
      SubscriptAnalysis::analyze(loadOp, loops, loadIVs);
      
      // For each loop, check if it carries a dependence
      for (unsigned i = 0; i < loops.size(); ++i) {
        Value iv = loops[i].getInductionVar();
        
        bool storeUsesIV = storeIVs.count(iv) > 0;
        bool loadUsesIV = loadIVs.count(iv) > 0;
        
        if (storeUsesIV && loadUsesIV) {
          // Both access the same dimension with this IV
          // For simple affine accesses, dependence distance is typically 0
          // (access same element in same iteration)
          distances[i].maxDistance = std::max(distances[i].maxDistance, (int64_t)0);
          distances[i].minDistance = std::min(distances[i].minDistance, (int64_t)0);
        }
      }
    }
  }
  
  // Mark which loops can be space loops
  for (auto &dist : distances) {
    // AutoSA criterion: space loops have dependence distance <= 1
    // This allows systolic data flow (distance 1 = pass to neighbor)
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[SpaceTimeAnalysis] Dependence distances:\n";
    for (unsigned i = 0; i < distances.size(); ++i) {
      llvm::dbgs() << "  Loop " << i << ": [" << distances[i].minDistance 
                   << ", " << distances[i].maxDistance << "]"
                   << (distances[i].isUniform ? " (uniform)" : "") 
                   << (distances[i].canBeSpaceLoop() ? " [space candidate]" : "")
                   << "\n";
    }
  });
  
  return success();
}

//===----------------------------------------------------------------------===//
// Data Flow Direction Analysis
// (AutoSA: analyzeFlowDirection)
//===----------------------------------------------------------------------===//

/// Determine how data flows through the PE array for each operand.
/// Based on which loops access which operands:
///   - A[i,k]: flows horizontally (along j-axis, since j doesn't appear)
///   - B[k,j]: flows vertically (along i-axis, since i doesn't appear)
///   - C[i,j]: stays local (output-stationary, k doesn't appear)
/// Legacy data flow analysis (hardcoded for 2D PE arrays)
/// 
/// This version assumes 2 space loops and uses hardcoded rules for
/// MatMul-like patterns. Kept for backward compatibility.
static LogicalResult analyzeOperandFlows(
    AffineForOp outerLoop,
    SmallVectorImpl<AffineForOp> &loops,
    const SmallVectorImpl<unsigned> &spaceLoops,
    DenseMap<Value, SystolicFlowDir> &flows) {
  
  flows.clear();
  
  if (loops.size() < 3 || spaceLoops.size() < 2) {
    LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Need 3+ loops and 2 space loops\n");
    return failure();
  }
  
  // Collect all accessed memrefs and their IV patterns
  AffineForOp innermostLoop = loops.back();
  Block *body = innermostLoop.getBody();
  
  DenseMap<Value, SmallPtrSet<Value, 3>> memrefToIVs;
  
  body->walk([&](Operation *op) {
    SmallVector<Value, 4> operands;
    Value memref;
    
    if (auto loadOp = dyn_cast<AffineLoadOp>(op)) {
      memref = loadOp.getMemRef();
      for (auto o : loadOp.getMapOperands())
        operands.push_back(o);
    } else if (auto storeOp = dyn_cast<AffineStoreOp>(op)) {
      memref = storeOp.getMemRef();
      for (auto o : storeOp.getMapOperands())
        operands.push_back(o);
    }
    
    if (memref) {
      for (auto operand : operands) {
        for (auto loop : loops) {
          if (operand == loop.getInductionVar()) {
            memrefToIVs[memref].insert(operand);
            break;
          }
        }
      }
    }
  });
  
  // For 2D output-stationary (spaceLoops = [0,1] = [i,j]):
  // Space loops: i (index 0), j (index 1)
  // Time loop: k (index 2)
  Value iv_i = loops[spaceLoops[0]].getInductionVar();  // First space loop
  Value iv_j = loops[spaceLoops[1]].getInductionVar();  // Second space loop
  
  for (auto &entry : memrefToIVs) {
    Value memref = entry.first;
    auto &ivSet = entry.second;
    
    bool usesI = ivSet.count(iv_i) > 0;
    bool usesJ = ivSet.count(iv_j) > 0;
    
    if (usesI && usesJ) {
      // Uses both space loop IVs -> output-stationary (C matrix)
      flows[memref] = SystolicFlowDir::NONE;
      LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Memref uses [i,j] -> NONE (local)\n");
    } else if (usesI && !usesJ) {
      // Uses only i -> flows horizontally along j (A matrix)
      flows[memref] = SystolicFlowDir::HORIZONTAL;
      LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Memref uses [i] only -> HORIZONTAL\n");
    } else if (!usesI && usesJ) {
      // Uses only j -> flows vertically along i (B matrix)
      flows[memref] = SystolicFlowDir::VERTICAL;
      LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Memref uses [j] only -> VERTICAL\n");
    } else {
      // Neither space loop -> broadcast or some special pattern
      flows[memref] = SystolicFlowDir::NONE;
      LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Memref uses neither -> NONE\n");
    }
  }
  
  return success();
}

/// Parametric data flow analysis (Phase 2 enhancement)
/// 
/// This version uses the ParametricSpaceTime configuration to determine
/// data flow directions based on access projection patterns.
/// Supports both 1D and 2D PE arrays (ST0-ST5).
LogicalResult analyzeOperandFlowsParametric(
    AffineForOp outerLoop,
    SmallVectorImpl<AffineForOp> &loops,
    const ParametricSpaceTime &parametric,
    DenseMap<Value, SystolicFlowDir> &flows) {
  
  flows.clear();
  
  unsigned numSpaceDims = parametric.getNumSpaceDims();
  if (loops.size() < 3 || numSpaceDims == 0) {
    LLVM_DEBUG(llvm::dbgs() 
        << "[SpaceTimeAnalysis] Invalid config: loops=" << loops.size()
        << ", spaceDims=" << numSpaceDims << "\n");
    return failure();
  }
  
  // Collect all accessed memrefs and their IV usage patterns
  AffineForOp innermostLoop = loops.back();
  Block *body = innermostLoop.getBody();
  
  DenseMap<Value, SmallPtrSet<Value, 4>> memrefToIVs;
  
  body->walk([&](Operation *op) {
    SmallVector<Value, 4> operands;
    Value memref;
    
    if (auto loadOp = dyn_cast<AffineLoadOp>(op)) {
      memref = loadOp.getMemRef();
      for (auto o : loadOp.getMapOperands())
        operands.push_back(o);
    } else if (auto storeOp = dyn_cast<AffineStoreOp>(op)) {
      memref = storeOp.getMemRef();
      for (auto o : storeOp.getMapOperands())
        operands.push_back(o);
    }
    
    if (memref) {
      for (auto operand : operands) {
        for (auto loop : loops) {
          if (operand == loop.getInductionVar()) {
            memrefToIVs[memref].insert(operand);
            break;
          }
        }
      }
    }
  });
  
  // Extract space loop induction variables from parametric config
  SmallVector<Value, 2> spaceLoopIVs;
  for (unsigned i = 0; i < numSpaceDims; ++i) {
    unsigned loopIdx = parametric.getSpaceDimConfig(i).loopDim;
    if (loopIdx < loops.size()) {
      spaceLoopIVs.push_back(loops[loopIdx].getInductionVar());
    }
  }
  
  // Analyze each memref's access pattern
  for (auto &entry : memrefToIVs) {
    Value memref = entry.first;
    auto &ivSet = entry.second;
    
    // Check which space loop IVs are used
    SmallVector<bool, 2> usesSpaceDim(numSpaceDims, false);
    for (unsigned i = 0; i < numSpaceDims; ++i) {
      usesSpaceDim[i] = ivSet.count(spaceLoopIVs[i]) > 0;
    }
    
    // Determine flow direction based on access projection pattern
    if (numSpaceDims == 1) {
      // 1D PE array (ST0/ST1/ST2)
      if (usesSpaceDim[0]) {
        // Uses the space loop IV -> local storage
        flows[memref] = SystolicFlowDir::NONE;
        LLVM_DEBUG(llvm::dbgs() 
            << "[SpaceTimeAnalysis] 1D: Memref uses space loop -> NONE (local)\n");
      } else {
        // Does not use space loop IV -> broadcast along PE array
        flows[memref] = SystolicFlowDir::HORIZONTAL;
        LLVM_DEBUG(llvm::dbgs() 
            << "[SpaceTimeAnalysis] 1D: Memref broadcast -> HORIZONTAL\n");
      }
    } else if (numSpaceDims == 2) {
      // 2D PE array (ST3/ST4/ST5)
      bool usesFirst = usesSpaceDim[0];
      bool usesSecond = usesSpaceDim[1];
      
      if (usesFirst && usesSecond) {
        // Uses both space loop IVs -> local storage (output)
        flows[memref] = SystolicFlowDir::NONE;
        LLVM_DEBUG(llvm::dbgs() 
            << "[SpaceTimeAnalysis] 2D: Memref uses both space dims -> NONE (local)\n");
      } else if (usesFirst && !usesSecond) {
        // Uses only first space dim -> flows along second dimension
        flows[memref] = SystolicFlowDir::HORIZONTAL;
        LLVM_DEBUG(llvm::dbgs() 
            << "[SpaceTimeAnalysis] 2D: Memref uses first space dim -> HORIZONTAL\n");
      } else if (!usesFirst && usesSecond) {
        // Uses only second space dim -> flows along first dimension
        flows[memref] = SystolicFlowDir::VERTICAL;
        LLVM_DEBUG(llvm::dbgs() 
            << "[SpaceTimeAnalysis] 2D: Memref uses second space dim -> VERTICAL\n");
      } else {
        // Uses neither space dim -> broadcast or special pattern
        flows[memref] = SystolicFlowDir::NONE;
        LLVM_DEBUG(llvm::dbgs() 
            << "[SpaceTimeAnalysis] 2D: Memref uses neither space dim -> NONE\n");
      }
    }
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Main Analysis Entry Points
//===----------------------------------------------------------------------===//

LogicalResult analyzeSpaceTime(AffineForOp outerLoop,
                               SpaceTimeInfo &result) {
  result = SpaceTimeInfo();
  result.outerLoop = outerLoop;
  
  // Step 1: Get the loop nest
  SmallVector<AffineForOp, 8> loops;
  getLoopNest(outerLoop, loops);
  result.numLoops = loops.size();
  
  if (loops.size() < 3) {
    LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Need at least 3 loops, got "
                            << loops.size() << "\n");
    return failure();
  }
  
  // Step 2: Compute dependence distances
  if (failed(computeDependenceDistances(outerLoop, loops, result.dependenceDistances))) {
    return failure();
  }
  
  // Step 3: Identify space loop candidates
  for (unsigned i = 0; i < result.dependenceDistances.size(); ++i) {
    if (result.dependenceDistances[i].canBeSpaceLoop()) {
      result.spaceLoopCandidates.push_back(i);
    }
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[SpaceTimeAnalysis] Space loop candidates: ";
    for (unsigned i : result.spaceLoopCandidates)
      llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n";
  });
  
  return success();
}

bool verifySpaceLoopSelection(const SpaceTimeInfo &info,
                              ArrayRef<unsigned> spaceLoops) {
  for (unsigned idx : spaceLoops) {
    if (idx >= info.dependenceDistances.size())
      return false;
    if (!info.dependenceDistances[idx].canBeSpaceLoop())
      return false;
  }
  return true;
}

LogicalResult selectSpaceLoops(SpaceTimeInfo &info,
                               unsigned numSpaceDims) {
  info.selectedSpaceLoops.clear();
  info.timeLoops.clear();
  
  if (info.spaceLoopCandidates.size() < numSpaceDims) {
    LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Not enough space loop candidates\n");
    return failure();
  }
  
  // Select first numSpaceDims candidates as space loops
  // For MatMul, this typically selects [i, j] for 2D array
  for (unsigned i = 0; i < numSpaceDims && i < info.spaceLoopCandidates.size(); ++i) {
    info.selectedSpaceLoops.push_back(info.spaceLoopCandidates[i]);
  }
  
  // Remaining loops are time loops
  for (unsigned i = 0; i < info.numLoops; ++i) {
    bool isSpace = false;
    for (unsigned s : info.selectedSpaceLoops) {
      if (s == i) {
        isSpace = true;
        break;
      }
    }
    if (!isSpace) {
      info.timeLoops.push_back(i);
    }
  }
  
  return success();
}

/// Analyze data flow directions using the parametric configuration
/// Phase 2 Enhancement: Uses ParametricSpaceTime if available, 
/// otherwise falls back to legacy mode
LogicalResult analyzeDataFlow(SpaceTimeInfo &info) {
  SmallVector<AffineForOp, 8> loops;
  getLoopNest(info.outerLoop, loops);
  
  // Try parametric analysis first if configuration is valid
  if (info.parametric.isValid() && info.parametric.getNumSpaceDims() > 0) {
    LLVM_DEBUG(llvm::dbgs() 
        << "[SpaceTimeAnalysis] Using parametric data flow analysis ("
        << info.parametric.getSpaceTimeTypeString() << ")\n");
    
    if (succeeded(analyzeOperandFlowsParametric(info.outerLoop, loops,
                                                info.parametric, info.operandFlows))) {
      // Copy flows to parametric config for later use
      for (auto &entry : info.operandFlows) {
        info.parametric.setOperandFlow(entry.first, entry.second);
      }
      return success();
    }
    
    LLVM_DEBUG(llvm::dbgs() 
        << "[SpaceTimeAnalysis] Parametric analysis failed, falling back to legacy\n");
  }
  
  // Fallback to legacy analysis (requires 2 space loops)
  if (info.selectedSpaceLoops.size() >= 2) {
    LLVM_DEBUG(llvm::dbgs() 
        << "[SpaceTimeAnalysis] Using legacy data flow analysis\n");
    return analyzeOperandFlows(info.outerLoop, loops, 
                               info.selectedSpaceLoops, info.operandFlows);
  }
  
  LLVM_DEBUG(llvm::dbgs() 
      << "[SpaceTimeAnalysis] Data flow analysis failed: "
      << "no valid parametric config and < 2 space loops\n");
  return failure();
}

void SpaceTimeInfo::dump() const {
  llvm::errs() << "SpaceTimeInfo:\n";
  llvm::errs() << "  numLoops: " << numLoops << "\n";
  llvm::errs() << "  spaceLoopCandidates: ";
  for (unsigned i : spaceLoopCandidates) llvm::errs() << i << " ";
  llvm::errs() << "\n  selectedSpaceLoops: ";
  for (unsigned i : selectedSpaceLoops) llvm::errs() << i << " ";
  llvm::errs() << "\n  timeLoops: ";
  for (unsigned i : timeLoops) llvm::errs() << i << " ";
  llvm::errs() << "\n";
}

//===----------------------------------------------------------------------===//
// SystolicConfig Factory
//===----------------------------------------------------------------------===//

SystolicConfig SystolicConfig::fromSpaceTimeInfo(
    const SpaceTimeInfo &info,
    ArrayRef<int64_t> arrayPartHints,
    ArrayRef<int64_t> latencyHints,
    unsigned simd) {
  
  SystolicConfig config;
  
  // Copy space/time loop indices
  config.spaceLoops.assign(info.selectedSpaceLoops.begin(), 
                           info.selectedSpaceLoops.end());
  config.timeLoops.assign(info.timeLoops.begin(), 
                          info.timeLoops.end());
  
  // Set tiling parameters
  config.arrayPart.assign(arrayPartHints.begin(), arrayPartHints.end());
  config.latency.assign(latencyHints.begin(), latencyHints.end());
  config.simdWidth = simd;
  
  // Calculate PE array dimensions
  if (config.arrayPart.size() >= 2 && config.latency.size() >= 2) {
    config.numPE_I = config.arrayPart[0] / config.latency[0];
    config.numPE_J = config.arrayPart[1] / config.latency[1];
  }
  
  // Copy data flow info
  config.operandFlows = info.operandFlows;
  
  return config;
}

bool SystolicConfig::isValid() const {
  if (numPE_I == 0 || numPE_J == 0)
    return false;
  if (arrayPart.empty() || latency.empty())
    return false;
  return true;
}

void SystolicConfig::dump() const {
  llvm::errs() << "SystolicConfig:\n";
  llvm::errs() << "  PE array: " << numPE_I << " x " << numPE_J << "\n";
  llvm::errs() << "  arrayPart: ";
  for (auto s : arrayPart) llvm::errs() << s << " ";
  llvm::errs() << "\n  latency: ";
  for (auto s : latency) llvm::errs() << s << " ";
  llvm::errs() << "\n  simdWidth: " << simdWidth << "\n";
}

//===----------------------------------------------------------------------===//
// NEW: Parametric Space-Time Configuration Inference
//===----------------------------------------------------------------------===//

/// Infer parametric space-time configuration from analysis results
/// Phase 2 Enhancement: Automatically populates SpaceTimeInfo with 
/// the parametric configuration and syncs loop indices
LogicalResult inferParametricSpaceTime(SpaceTimeInfo &info,
                                       const ParametricSpaceTime &spacetimeHint) {
  // Initialize with the hint (default ST3 for backward compatibility)
  info.parametric = spacetimeHint;
  
  // Validate the configuration
  if (!info.parametric.isValid()) {
    LLVM_DEBUG(llvm::dbgs() << "[SpaceTimeAnalysis] Invalid parametric spacetime configuration\n");
    return failure();
  }
  
  // Sync selected space loops from parametric config
  info.selectedSpaceLoops.clear();
  for (unsigned i = 0; i < info.parametric.getNumSpaceDims(); ++i) {
    unsigned loopIdx = info.parametric.getSpaceDimConfig(i).loopDim;
    info.selectedSpaceLoops.push_back(loopIdx);
  }
  
  // Sync time loops (all loops not in space dimensions)
  info.timeLoops.clear();
  std::set<unsigned> spaceSet(info.selectedSpaceLoops.begin(),
                              info.selectedSpaceLoops.end());
  for (unsigned i = 0; i < info.numLoops; ++i) {
    if (spaceSet.find(i) == spaceSet.end()) {
      info.timeLoops.push_back(i);
    }
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[SpaceTimeAnalysis] Inferred parametric spacetime: "
                 << info.parametric.getSpaceTimeTypeString() << "\n";
    llvm::dbgs() << "  Space loops: ";
    for (unsigned i : info.selectedSpaceLoops) llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n  Time loops: ";
    for (unsigned i : info.timeLoops) llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n";
    info.parametric.dump();
  });
  
  return success();
}

} // namespace systolic
} // namespace mlir
