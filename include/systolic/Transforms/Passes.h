//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Transform Passes
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_TRANSFORMS_PASSES_H
#define SYSTOLIC_TRANSFORMS_PASSES_H

#include "mlir/Pass/Pass.h"
#include "systolic/Analysis/SystolicConfig.h"

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Pass Declarations
//===----------------------------------------------------------------------===//

/// Create a pass that performs space-time transformation.
/// This includes:
/// - Analyzing dependences (via Polymer)
/// - Selecting space loops
/// - Loop permutation and tiling
std::unique_ptr<Pass> createSpaceTimeTransformPass();
std::unique_ptr<Pass> createSpaceTimeTransformPass(const SystolicConfig &config);

/// Create a pass that generates stream channels and dataflow tasks.
std::unique_ptr<Pass> createStreamGenerationPass();

/// Create a pass that generates I/O modules (L1, L2, L3).
std::unique_ptr<Pass> createIOModuleGenerationPass();

/// Create a pass that applies SIMD vectorization.
std::unique_ptr<Pass> createSIMDVectorizationPass(unsigned simdWidth = 2);

//===----------------------------------------------------------------------===//
// Pass Options
//===----------------------------------------------------------------------===//

struct SpaceTimeTransformOptions {
  /// Space-time mapping mode (0-5, same as AutoSA)
  unsigned spaceTimeMode = 3;  // Default: [i,j] 2D output-stationary
  
  /// Array partitioning factors
  llvm::SmallVector<int64_t, 3> arrayPart = {16, 16, 16};
  
  /// Latency hiding factors
  llvm::SmallVector<int64_t, 2> latency = {8, 8};
  
  /// SIMD width
  unsigned simdWidth = 1;
};

//===----------------------------------------------------------------------===//
// Pass Registration
//===----------------------------------------------------------------------===//

#define GEN_PASS_REGISTRATION
// #include "systolic/Transforms/Passes.h.inc"  // Will be generated

void registerSystolicPasses();

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_TRANSFORMS_PASSES_H
