//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Layout Optimizer
//
// This module evaluates different layout permutations and selects the optimal
// one based on access pattern analysis.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_LAYOUTOPTIMIZER_H
#define SYSTOLIC_ANALYSIS_LAYOUTOPTIMIZER_H

#include "systolic/Analysis/PolyhedralAccessAnalyzer.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Layout Evaluation
//===----------------------------------------------------------------------===//

/// Represents a layout permutation and its score
struct LayoutScoreResult {
  // 维度置换：[0, 1, 2] 表示原始顺序
  SmallVector<unsigned, 3> permutation;
  
  // 内存访问成本（越低越好）
  double memoryCost = 0.0;
  
  // 缓存局部性评分（越高越好）
  double cacheLocality = 0.0;
  
  // 综合评分（越低越好）
  double totalScore = 0.0;
  
  // 重排后的维度
  SmallVector<int64_t, 3> reorderedDims;
};

//===----------------------------------------------------------------------===//
// Layout Optimizer
//===----------------------------------------------------------------------===//

/// Evaluates different layout permutations and selects the optimal one
class LayoutOptimizer {
public:
  /// Evaluate all possible 3D layout permutations
  static SmallVector<LayoutScoreResult, 6> evaluateAllLayouts(
      const AccessPattern &writePattern,
      const AccessPattern &readPattern,
      const SmallVector<int64_t, 3> &originalDims);
  
  /// Select the best layout from evaluation results
  static LayoutScoreResult selectBestLayout(
      const SmallVector<LayoutScoreResult, 6> &layouts);
  
  /// Evaluate memory cost for a specific permutation
  static double evaluateMemoryCost(
      const AccessPattern &writePattern,
      const AccessPattern &readPattern,
      const SmallVector<unsigned, 3> &permutation);
  
  /// Evaluate cache locality for a specific permutation
  static double evaluateCacheLocality(
      const AccessPattern &writePattern,
      const AccessPattern &readPattern,
      const SmallVector<unsigned, 3> &permutation);

private:
  /// Generate all 3D permutations (6 total)
  static SmallVector<SmallVector<unsigned, 3>, 6> generateAllPermutations();
  
  /// Apply permutation to dimensions
  static SmallVector<int64_t, 3> applyPermutation(
      const SmallVector<int64_t, 3> &dims,
      const SmallVector<unsigned, 3> &permutation);
};

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_LAYOUTOPTIMIZER_H

