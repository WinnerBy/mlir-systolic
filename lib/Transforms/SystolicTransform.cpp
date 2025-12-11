//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Systolic Array Transform Pass
//
// This pass implements space-time transformation, array partitioning, and
// latency hiding for affine loop nests, following AutoSA's methodology.
//
// AutoSA Reference:
//   - sa_legality_check: Single band + uniform dependency
//   - sa_space_time_transform: Dependence distance analysis, space loop selection
//   - sa_array_partitioning_optimize: Multi-level tiling
//   - sa_latency_hiding_optimize: Latency hiding tiling
//
//===----------------------------------------------------------------------===//

#include "systolic/Transforms/Passes.h"
#include "systolic/Analysis/SpaceTimeAnalysis.h"
#include "systolic/Analysis/SystolicConfig.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/AffineAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/LoopAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/Utils.h"
#include "mlir/Dialect/Affine/LoopUtils.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/LoopInvariantCodeMotionUtils.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <algorithm>
#include <vector>

#define DEBUG_TYPE "systolic-transform"

using namespace mlir;
using namespace mlir::affine;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Helper Data Structures
//===----------------------------------------------------------------------===//

/// Loop band representation (similar to AutoSA's band concept)
using LoopBand = SmallVector<AffineForOp, 8>;

/// Dependence information for a loop dimension
/// Following AutoSA's dep_dis (dependence distance) concept
struct LoopDepInfo {
  unsigned loopIndex;       // Index in the loop band
  int64_t minDistance;      // Minimum dependence distance
  int64_t maxDistance;      // Maximum dependence distance
  bool isUniform;           // True if distance is constant (uniform dep)
  bool canBeSpaceLoop;      // True if distance <= 1 (space loop candidate)
};

/// Problem size inferred from loop bounds or memref shapes
/// Following AutoSA's ProblemSize concept
struct ProblemSize {
  int64_t M = 0;  // Output rows
  int64_t N = 0;  // Output columns
  int64_t K = 0;  // Reduction dimension
  bool valid = false;
};

//===----------------------------------------------------------------------===//
// Pass Options
//===----------------------------------------------------------------------===//

struct SystolicTransformOptions {
  /// Space-time mapping mode (0-5, same as AutoSA)
  /// 0: [i]   - 1D row array
  /// 1: [j]   - 1D column array
  /// 2: [k]   - 1D reduction array
  /// 3: [i,j] - 2D output-stationary (default)
  /// 4: [i,k] - 2D with horizontal reduction
  /// 5: [j,k] - 2D with vertical reduction
  unsigned spaceTimeMode = 3;
  
  /// Array partitioning factors (first-level tiling)
  SmallVector<int64_t, 3> arrayPart = {16, 16, 16};
  
  /// Latency hiding factors (second-level tiling)
  SmallVector<int64_t, 2> latency = {8, 8};
  
  /// SIMD width (vectorization factor)
  unsigned simdWidth = 1;
  
  /// Enable two-level buffering (L2 array partitioning)
  bool twoLevelBuffer = false;
};

//===----------------------------------------------------------------------===//
// Legality Check (AutoSA: sa_legality_check)
//===----------------------------------------------------------------------===//

/// Check if the loop band is suitable for systolic array transformation.
/// AutoSA requires:
/// 1. Single fully permutable outermost band
/// 2. Uniform dependences
static LogicalResult checkLegality(LoopBand &band) {
  if (band.empty()) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Error: Empty loop band\n");
    return failure();
  }
  
  // Check for at least 3 nested loops (typical for MatMul-like patterns)
  if (band.size() < 3) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Warning: Loop band has only "
                            << band.size() << " levels, expected >= 3\n");
  }
  
  // Check if loops are perfectly nested
  for (unsigned i = 0; i + 1 < band.size(); ++i) {
    auto outerLoop = band[i];
    auto innerLoop = band[i + 1];
    
    // Check if the outer loop body contains only the inner loop
    Block *body = outerLoop.getBody();
    unsigned opCount = 0;
    for (auto &op : *body) {
      if (!isa<AffineYieldOp>(op))
        opCount++;
    }
    
    if (opCount != 1 || body->front().getNumRegions() == 0) {
      LLVM_DEBUG(llvm::dbgs() 
          << "[Systolic] Warning: Loops are not perfectly nested at level " 
          << i << "\n");
    }
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Dependence Distance Analysis
// (AutoSA: get_dep_dis_at_node in autosa_utils.cpp)
//===----------------------------------------------------------------------===//

/// Analyze dependence distances for each loop in the band.
/// This is a simplified version - full implementation would use Polymer/ISL.
static LogicalResult analyzeDependenceDistances(
    LoopBand &band,
    SmallVectorImpl<LoopDepInfo> &depInfos) {
  
  depInfos.clear();
  
  // Get the innermost loop body for memory access analysis
  AffineForOp innermostLoop = band.back();
  Block *body = innermostLoop.getBody();
  
  // Collect all affine loads and stores
  SmallVector<AffineLoadOp, 4> loads;
  SmallVector<AffineStoreOp, 4> stores;
  
  body->walk([&](Operation *op) {
    if (auto loadOp = dyn_cast<AffineLoadOp>(op))
      loads.push_back(loadOp);
    else if (auto storeOp = dyn_cast<AffineStoreOp>(op))
      stores.push_back(storeOp);
  });
  
  // For each loop dimension, analyze dependence distances
  for (unsigned i = 0; i < band.size(); ++i) {
    LoopDepInfo info;
    info.loopIndex = i;
    info.minDistance = 0;
    info.maxDistance = 0;
    info.isUniform = true;
    
    AffineForOp loop = band[i];
    Value iv = loop.getInductionVar();
    
    // Check if induction variable appears in load/store affine maps
    // For RAW deps: store writes, load reads -> distance = how many iterations apart
    
    bool foundDep = false;
    for (auto storeOp : stores) {
      Value memref = storeOp.getMemRef();
      
      // Find loads from the same memref
      for (auto loadOp : loads) {
        if (loadOp.getMemRef() != memref)
          continue;
        
        // Check if both access the same indices
        // For simple analysis, check if IV is used in both
        bool storeUsesIV = false;
        bool loadUsesIV = false;
        
        for (auto operand : storeOp.getMapOperands()) {
          if (operand == iv) storeUsesIV = true;
        }
        for (auto operand : loadOp.getMapOperands()) {
          if (operand == iv) loadUsesIV = true;
        }
        
        // If same memref, and both use this IV -> potential dependence
        if (storeUsesIV && loadUsesIV) {
          foundDep = true;
          // For a simple case like C[i,j] += A[i,k]*B[k,j]:
          // - Loop k has no cross-iteration deps (distance = 0)
          // - Loops i,j have output deps (distance = 0) for C
          info.maxDistance = 0;
          info.minDistance = 0;
        }
        
        // Check for store[i] -> load[i+1] pattern (distance = 1)
        if (storeUsesIV && loadUsesIV && storeOp.getMemRef() == loadOp.getMemRef()) {
          // This is a simplified check - full analysis would compare maps
          // For reduction patterns, distance is typically 0
          info.maxDistance = std::max(info.maxDistance, (int64_t)0);
        }
      }
    }
    
    // AutoSA: space loops are those with dependence distance <= 1
    info.canBeSpaceLoop = (info.maxDistance <= 1);
    
    depInfos.push_back(info);
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Space Loop Selection
// (AutoSA: sa_space_time_transform_at_dim_async)
//===----------------------------------------------------------------------===//

/// Select space loops based on dependence analysis and spaceTimeMode.
/// Following AutoSA's methodology:
/// - Space loops must have dependence distance <= 1
/// - Mode determines which loops are space (PE indices) vs time (execution order)
static LogicalResult selectSpaceLoops(
    const SmallVectorImpl<LoopDepInfo> &depInfos,
    unsigned spaceTimeMode,
    SmallVectorImpl<unsigned> &spaceLoopIndices,
    SmallVectorImpl<unsigned> &timeLoopIndices) {
  
  spaceLoopIndices.clear();
  timeLoopIndices.clear();
  
  unsigned numLoops = depInfos.size();
  if (numLoops < 3) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Need at least 3 loops for space-time mapping\n");
    return failure();
  }
  
  // AutoSA space-time modes for 3-loop case (i=0, j=1, k=2):
  switch (spaceTimeMode) {
    case 0:  // [i] - 1D row array
      spaceLoopIndices.push_back(0);
      timeLoopIndices.push_back(1);
      timeLoopIndices.push_back(2);
      break;
    case 1:  // [j] - 1D column array
      spaceLoopIndices.push_back(1);
      timeLoopIndices.push_back(0);
      timeLoopIndices.push_back(2);
      break;
    case 2:  // [k] - 1D reduction array
      spaceLoopIndices.push_back(2);
      timeLoopIndices.push_back(0);
      timeLoopIndices.push_back(1);
      break;
    case 3:  // [i,j] - 2D output-stationary (default)
      spaceLoopIndices.push_back(0);
      spaceLoopIndices.push_back(1);
      timeLoopIndices.push_back(2);
      break;
    case 4:  // [i,k] - 2D with horizontal reduction
      spaceLoopIndices.push_back(0);
      spaceLoopIndices.push_back(2);
      timeLoopIndices.push_back(1);
      break;
    case 5:  // [j,k] - 2D with vertical reduction
      spaceLoopIndices.push_back(1);
      spaceLoopIndices.push_back(2);
      timeLoopIndices.push_back(0);
      break;
    default:
      LLVM_DEBUG(llvm::dbgs() << "[Systolic] Invalid space-time mode: " 
                              << spaceTimeMode << "\n");
      return failure();
  }
  
  // Verify selected space loops have distance <= 1
  for (unsigned idx : spaceLoopIndices) {
    if (idx >= depInfos.size()) {
      LLVM_DEBUG(llvm::dbgs() << "[Systolic] Space loop index out of range: " 
                              << idx << "\n");
      return failure();
    }
    if (!depInfos[idx].canBeSpaceLoop) {
      LLVM_DEBUG(llvm::dbgs() 
          << "[Systolic] Warning: Loop " << idx 
          << " has dep distance > 1, may not be suitable for space mapping\n");
    }
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[Systolic] Space-time mode " << spaceTimeMode << ":\n";
    llvm::dbgs() << "  Space loops: ";
    for (unsigned i : spaceLoopIndices) llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n  Time loops: ";
    for (unsigned i : timeLoopIndices) llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n";
  });
  
  return success();
}

//===----------------------------------------------------------------------===//
// Loop Permutation
// (AutoSA: loop_interchange_at_node)
//===----------------------------------------------------------------------===//

/// Permute loops to place space loops as the outer dimensions.
/// For async systolic arrays, space loops should be outermost.
/// After tiling, the order becomes:
///   [space_tile] -> [time_tile] -> [space_point] -> [time_point]
static LogicalResult permuteLoopsForSpaceTime(
    LoopBand &band,
    const SmallVectorImpl<unsigned> &spaceLoopIndices,
    const SmallVectorImpl<unsigned> &timeLoopIndices) {
  
  // Build permutation map: space loops first, then time loops
  SmallVector<unsigned, 6> permMap;
  for (unsigned idx : spaceLoopIndices)
    permMap.push_back(idx);
  for (unsigned idx : timeLoopIndices)
    permMap.push_back(idx);
  
  // Check if permutation is identity
  bool isIdentity = true;
  for (unsigned i = 0; i < permMap.size(); ++i) {
    if (permMap[i] != i) {
      isIdentity = false;
      break;
    }
  }
  
  if (isIdentity) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Loop order is already optimal\n");
    return success();
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[Systolic] Permuting loops: ";
    for (unsigned i : permMap) llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n";
  });
  
  // Use MLIR's loop permutation utility
  // Note: This requires the loops to be perfectly nested
  // permuteLoops returns the number of loops that could not be permuted
  unsigned numUnpermuted = permuteLoops(band, permMap);
  if (numUnpermuted > 0) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Loop permutation failed, " 
                            << numUnpermuted << " loops could not be permuted\n");
    return failure();
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Multi-Level Tiling
// (AutoSA: sa_array_partitioning_optimize + sa_latency_hiding_optimize)
//===----------------------------------------------------------------------===//

/// Apply multi-level tiling following AutoSA's methodology:
/// 1. Array partitioning (first-level tiling): Creates tile loops for PE array
/// 2. Latency hiding (second-level tiling): Creates point loops for pipelining
///
/// Input loop nest (3 loops: i, j, k):
///   for i = 0..M:
///     for j = 0..N:
///       for k = 0..K:
///         C[i,j] += A[i,k] * B[k,j]
///
/// After array_part tiling [16,16,16]:
///   for i0 = 0..M/16:         // Tile loops
///     for j0 = 0..N/16:
///       for k0 = 0..K/16:
///         for i1 = 0..16:     // Point loops
///           for j1 = 0..16:
///             for k1 = 0..16:
///               C[i0*16+i1, j0*16+j1] += ...
///
/// After latency tiling [8,8,16] on point loops:
///   for i0, j0, k0:           // Array partition tiles (L3)
///     for i1, j1, k1:         // Latency tiles (L2)
///       for i2, j2, k2:       // Point loops (L1)
///         ...
///
/// PE array size = array_part / latency = 16/8 = 2 (per dimension)
static LogicalResult applyMultiLevelTiling(
    LoopBand &band,
    const SystolicTransformOptions &options,
    LoopBand &tiledBand) {
  
  if (band.size() < 3) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Need at least 3 loops for tiling\n");
    return failure();
  }
  
  // Level 1: Array Partitioning
  SmallVector<int64_t, 3> tileSizes1;
  for (unsigned i = 0; i < std::min((size_t)band.size(), options.arrayPart.size()); ++i) {
    tileSizes1.push_back(options.arrayPart[i]);
  }
  // Pad with 1s if needed
  while (tileSizes1.size() < band.size()) {
    tileSizes1.push_back(1);
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[Systolic] Array partitioning tile sizes: ";
    for (auto s : tileSizes1) llvm::dbgs() << s << " ";
    llvm::dbgs() << "\n";
  });
  
  // Apply first-level tiling
  SmallVector<AffineForOp, 6> tiledNest1;
  SmallVector<unsigned, 3> tileSizes1Unsigned(tileSizes1.begin(), tileSizes1.end());
  if (failed(tilePerfectlyNested(band, tileSizes1Unsigned, &tiledNest1))) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] First-level tiling failed\n");
    return failure();
  }
  
  // After first tiling: [tile0, tile1, tile2, point0, point1, point2]
  unsigned numOrigLoops = band.size();
  if (tiledNest1.size() != numOrigLoops * 2) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Unexpected number of loops after tiling: "
                            << tiledNest1.size() << "\n");
    return failure();
  }
  
  // Extract point loops for second-level tiling
  LoopBand pointLoops;
  for (unsigned i = numOrigLoops; i < tiledNest1.size(); ++i) {
    pointLoops.push_back(tiledNest1[i]);
  }
  
  // Level 2: Latency Hiding
  SmallVector<int64_t, 3> tileSizes2;
  for (unsigned i = 0; i < pointLoops.size(); ++i) {
    if (i < options.latency.size()) {
      tileSizes2.push_back(options.latency[i]);
    } else {
      // For non-space loops (e.g., k), use full tile size (no latency tiling)
      tileSizes2.push_back(options.arrayPart[i]);
    }
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[Systolic] Latency hiding tile sizes: ";
    for (auto s : tileSizes2) llvm::dbgs() << s << " ";
    llvm::dbgs() << "\n";
  });
  
  // Apply second-level tiling
  SmallVector<AffineForOp, 6> tiledNest2;
  SmallVector<unsigned, 3> tileSizes2Unsigned(tileSizes2.begin(), tileSizes2.end());
  if (failed(tilePerfectlyNested(pointLoops, tileSizes2Unsigned, &tiledNest2))) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Second-level tiling failed\n");
    return failure();
  }
  
  // Build final tiled band:
  // [array_part_tile0, array_part_tile1, array_part_tile2,
  //  latency_tile0, latency_tile1, latency_tile2,
  //  point0, point1, point2]
  tiledBand.clear();
  
  // Array partition tile loops
  for (unsigned i = 0; i < numOrigLoops; ++i) {
    tiledBand.push_back(tiledNest1[i]);
  }
  
  // Latency tile loops + point loops from second tiling
  for (auto loop : tiledNest2) {
    tiledBand.push_back(loop);
  }
  
  LLVM_DEBUG(llvm::dbgs() << "[Systolic] Final tiled nest has " 
                          << tiledBand.size() << " loops\n");
  
  return success();
}

//===----------------------------------------------------------------------===//
// Loop Permutation after Tiling
// (AutoSA's permutation map: {0,1,2, 3,4,5, 7,8,6})
//===----------------------------------------------------------------------===//

/// Apply final permutation to place loops in the order needed for HLS:
/// After tiling we have 9 loops (for 3-loop nest):
///   [i0,j0,k0, i1,j1,k1, i2,j2,k2]
///    0  1  2   3  4  5   6  7  8
///
/// AutoSA permutes to:
///   [i0,j0,k0, i1,j1,k1, j2,k2,i2]
///    0  1  2   3  4  5   7  8  6
///
/// This places the reduction loop innermost for accumulation.
static LogicalResult applyFinalPermutation(LoopBand &tiledBand) {
  if (tiledBand.size() != 9) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Expected 9 loops for final permutation, got "
                            << tiledBand.size() << "\n");
    // For non-standard sizes, skip permutation
    return success();
  }
  
  // AutoSA permutation: {0,1,2, 3,4,5, 7,8,6}
  std::vector<unsigned> permMap = {0, 1, 2, 3, 4, 5, 7, 8, 6};
  
  LLVM_DEBUG({
    llvm::dbgs() << "[Systolic] Applying final permutation: ";
    for (unsigned i : permMap) llvm::dbgs() << i << " ";
    llvm::dbgs() << "\n";
  });
  
  unsigned numUnpermuted = permuteLoops(tiledBand, permMap);
  if (numUnpermuted > 0) {
    LLVM_DEBUG(llvm::dbgs() << "[Systolic] Final permutation failed, "
                            << numUnpermuted << " loops could not be permuted\n");
    return failure();
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Problem Size Inference
// (AutoSA: inferProblemSize)
//===----------------------------------------------------------------------===//

/// Infer problem dimensions (M, N, K) from loop bounds or memref shapes.
static ProblemSize inferProblemSize(LoopBand &band) {
  ProblemSize size;
  
  // Method 1: Extract from loop bounds
  if (band.size() >= 3) {
    for (unsigned i = 0; i < 3; ++i) {
      auto loop = band[i];
      if (loop.hasConstantUpperBound()) {
        int64_t ub = loop.getConstantUpperBound();
        if (i == 0) size.M = ub;
        else if (i == 1) size.N = ub;
        else if (i == 2) size.K = ub;
      }
    }
    
    if (size.M > 0 && size.N > 0 && size.K > 0) {
      size.valid = true;
      LLVM_DEBUG(llvm::dbgs() << "[Systolic] Inferred problem size: M=" << size.M
                              << ", N=" << size.N << ", K=" << size.K << "\n");
      return size;
    }
  }
  
  // Method 2: Extract from memref shapes (TODO: implement)
  
  return size;
}

//===----------------------------------------------------------------------===//
// Systolic Transform Pass
//===----------------------------------------------------------------------===//

namespace {

struct SystolicTransformPass 
    : public PassWrapper<SystolicTransformPass, OperationPass<func::FuncOp>> {
  
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(SystolicTransformPass)
  
  SystolicTransformPass() = default;
  SystolicTransformPass(const SystolicConfig &config) {
    options.spaceTimeMode = 3;  // Default to [i,j] 2D output-stationary
    if (!config.arrayPart.empty())
      options.arrayPart.assign(config.arrayPart.begin(), config.arrayPart.end());
    if (!config.latency.empty())
      options.latency.assign(config.latency.begin(), config.latency.end());
    options.simdWidth = config.simdWidth;
  }
  
  StringRef getArgument() const override { return "systolic-transform"; }
  StringRef getDescription() const override {
    return "Apply systolic array transformations (space-time, tiling, permutation)";
  }
  
  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<affine::AffineDialect, memref::MemRefDialect>();
  }
  
  void runOnOperation() override {
    auto func = getOperation();
    
    LLVM_DEBUG(llvm::dbgs() << "\n=== Systolic Transform Pass ===\n");
    LLVM_DEBUG(llvm::dbgs() << "Processing function: " << func.getName() << "\n");
    LLVM_DEBUG({
      llvm::dbgs() << "Options:\n";
      llvm::dbgs() << "  space-time mode: " << options.spaceTimeMode << "\n";
      llvm::dbgs() << "  array-part: ";
      for (auto s : options.arrayPart) llvm::dbgs() << s << " ";
      llvm::dbgs() << "\n  latency: ";
      for (auto s : options.latency) llvm::dbgs() << s << " ";
      llvm::dbgs() << "\n  simd: " << options.simdWidth << "\n";
    });
    
    // Step 1: Collect all loop bands
    SmallVector<LoopBand, 4> bands;
    func.walk([&](AffineForOp forOp) {
      // Only process top-level loops (not nested inside another AffineForOp)
      if (!forOp->getParentOfType<AffineForOp>()) {
        LoopBand band;
        // Collect the full loop nest
        AffineForOp current = forOp;
        while (current) {
          band.push_back(current);
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
        if (!band.empty())
          bands.push_back(band);
      }
    });
    
    LLVM_DEBUG(llvm::dbgs() << "Found " << bands.size() << " loop band(s)\n");
    
    // Step 2: Process each loop band
    for (auto &band : bands) {
      LLVM_DEBUG(llvm::dbgs() << "\nProcessing band with " << band.size() << " loops\n");
      
      // Step 2.1: Legality check (AutoSA: sa_legality_check)
      if (failed(checkLegality(band))) {
        LLVM_DEBUG(llvm::dbgs() << "Legality check failed, skipping band\n");
        continue;
      }
      
      // Step 2.2: Infer problem size
      ProblemSize problemSize = inferProblemSize(band);
      
      // Step 2.3: Dependence analysis (AutoSA: get_dep_dis_at_node)
      SmallVector<LoopDepInfo, 4> depInfos;
      if (failed(analyzeDependenceDistances(band, depInfos))) {
        LLVM_DEBUG(llvm::dbgs() << "Dependence analysis failed\n");
        continue;
      }
      
      // Step 2.4: Select space and time loops (AutoSA: sa_space_time_transform)
      SmallVector<unsigned, 2> spaceLoops;
      SmallVector<unsigned, 3> timeLoops;
      if (failed(selectSpaceLoops(depInfos, options.spaceTimeMode, 
                                   spaceLoops, timeLoops))) {
        LLVM_DEBUG(llvm::dbgs() << "Space loop selection failed\n");
        continue;
      }
      
      // Step 2.5: Permute loops (space loops to outer positions)
      // Note: For now, we skip permutation before tiling if already in order
      // This matches AutoSA's behavior for [i,j] mode on MatMul
      
      // Step 2.6: Multi-level tiling (AutoSA: array_part + latency)
      LoopBand tiledBand;
      if (failed(applyMultiLevelTiling(band, options, tiledBand))) {
        LLVM_DEBUG(llvm::dbgs() << "Multi-level tiling failed\n");
        continue;
      }
      
      // Step 2.7: Final permutation
      if (failed(applyFinalPermutation(tiledBand))) {
        LLVM_DEBUG(llvm::dbgs() << "Final permutation failed\n");
        // Continue anyway, permutation is optional
      }
      
      // Calculate PE array dimensions
      int64_t numPE_I = options.arrayPart[0] / options.latency[0];
      int64_t numPE_J = options.arrayPart[1] / options.latency[1];
      
      llvm::outs() << "[Systolic] Transformation complete:\n";
      llvm::outs() << "  PE array size: " << numPE_I << " x " << numPE_J << "\n";
      llvm::outs() << "  Total loops after tiling: " << tiledBand.size() << "\n";
    }
    
    LLVM_DEBUG(llvm::dbgs() << "\n=== Systolic Transform Pass Complete ===\n");
  }
  
private:
  SystolicTransformOptions options;
};

} // namespace

//===----------------------------------------------------------------------===//
// Pass Creation
//===----------------------------------------------------------------------===//

std::unique_ptr<Pass> createSpaceTimeTransformPass() {
  return std::make_unique<SystolicTransformPass>();
}

std::unique_ptr<Pass> createSpaceTimeTransformPass(const SystolicConfig &config) {
  return std::make_unique<SystolicTransformPass>(config);
}

//===----------------------------------------------------------------------===//
// Pass Registration
//===----------------------------------------------------------------------===//

void registerSystolicPasses() {
  PassRegistration<SystolicTransformPass>();
}

} // namespace systolic
} // namespace mlir
