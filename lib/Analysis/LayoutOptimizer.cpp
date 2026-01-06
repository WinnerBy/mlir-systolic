//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Layout Optimizer Implementation
//
//===----------------------------------------------------------------------===//

#include "systolic/Analysis/LayoutOptimizer.h"
#include "llvm/Support/Debug.h"
#include <algorithm>
#include <cmath>

#define DEBUG_TYPE "write-time-reordering-analysis"

using namespace mlir;
using namespace mlir::systolic;

//===----------------------------------------------------------------------===//
// Permutation Generation
//===----------------------------------------------------------------------===//

SmallVector<SmallVector<unsigned, 3>, 6> LayoutOptimizer::generateAllPermutations() {
  SmallVector<SmallVector<unsigned, 3>, 6> perms;
  
  // 所有 3D 排列（6 种）
  perms.push_back({0, 1, 2});  // 原始顺序
  perms.push_back({0, 2, 1});
  perms.push_back({1, 0, 2});
  perms.push_back({1, 2, 0});
  perms.push_back({2, 0, 1});
  perms.push_back({2, 1, 0});
  
  return perms;
}

SmallVector<int64_t, 3> LayoutOptimizer::applyPermutation(
    const SmallVector<int64_t, 3> &dims,
    const SmallVector<unsigned, 3> &permutation) {
  SmallVector<int64_t, 3> result;
  for (unsigned i = 0; i < 3; i++) {
    result.push_back(dims[permutation[i]]);
  }
  return result;
}

//===----------------------------------------------------------------------===//
// Cost Evaluation
//===----------------------------------------------------------------------===//

double LayoutOptimizer::evaluateMemoryCost(
    const AccessPattern &writePattern,
    const AccessPattern &readPattern,
    const SmallVector<unsigned, 3> &permutation) {
  double cost = 0.0;
  
  // 对每个维度计算成本
  for (unsigned i = 0; i < 3; i++) {
    unsigned origDim = permutation[i];
    
    // 读取成本
    if (origDim < readPattern.dimPatterns.size()) {
      const auto &readDim = readPattern.dimPatterns[origDim];
      
      // 非线性访问成本高
      if (readDim.hasNonLinear) {
        cost += 10.0 * (3 - i);  // 越外层维度，成本越高
      }
      
      // 非连续访问成本
      if (!readDim.isContiguous) {
        cost += 2.0 * (readDim.minStride - 1) * (3 - i);
      }
    }
    
    // 写入成本（通常写入是连续的，但也要考虑）
    if (origDim < writePattern.dimPatterns.size()) {
      const auto &writeDim = writePattern.dimPatterns[origDim];
      if (writeDim.hasNonLinear) {
        cost += 5.0 * (3 - i);  // 写入非线性成本稍低
      }
    }
  }
  
  return cost;
}

double LayoutOptimizer::evaluateCacheLocality(
    const AccessPattern &writePattern,
    const AccessPattern &readPattern,
    const SmallVector<unsigned, 3> &permutation) {
  double locality = 0.0;
  
  // 最内层维度（i=2）的连续性最重要
  for (unsigned i = 0; i < 3; i++) {
    unsigned origDim = permutation[i];
    double weight = static_cast<double>(i + 1) / 3.0;  // 内层权重更高
    
    // 读取局部性
    if (origDim < readPattern.dimPatterns.size()) {
      const auto &readDim = readPattern.dimPatterns[origDim];
      if (readDim.isContiguous && !readDim.hasNonLinear) {
        locality += weight * 1.0;
      } else if (readDim.isContiguous) {
        locality += weight * 0.5;  // 连续但有非线性
      }
    }
    
    // 写入局部性
    if (origDim < writePattern.dimPatterns.size()) {
      const auto &writeDim = writePattern.dimPatterns[origDim];
      if (writeDim.isContiguous && !writeDim.hasNonLinear) {
        locality += weight * 0.8;  // 写入局部性稍低权重
      }
    }
  }
  
  return locality;
}

//===----------------------------------------------------------------------===//
// Layout Evaluation
//===----------------------------------------------------------------------===//

SmallVector<LayoutScoreResult, 6> LayoutOptimizer::evaluateAllLayouts(
    const AccessPattern &writePattern,
    const AccessPattern &readPattern,
    const SmallVector<int64_t, 3> &originalDims) {
  SmallVector<LayoutScoreResult, 6> results;
  
  auto permutations = generateAllPermutations();
  
  for (const auto &perm : permutations) {
    LayoutScoreResult result;
    result.permutation = perm;
    result.reorderedDims = applyPermutation(originalDims, perm);
    
    // 评估成本
    result.memoryCost = evaluateMemoryCost(writePattern, readPattern, perm);
    result.cacheLocality = evaluateCacheLocality(writePattern, readPattern, perm);
    
    // 综合评分：memoryCost - cacheLocality（越低越好）
    result.totalScore = result.memoryCost - result.cacheLocality;
    
    results.push_back(result);
    
    LLVM_DEBUG({
      llvm::dbgs() << "Layout [" << perm[0] << ", " << perm[1] << ", " << perm[2] << "]: "
                   << "cost=" << result.memoryCost << ", "
                   << "locality=" << result.cacheLocality << ", "
                   << "score=" << result.totalScore << "\n";
    });
  }
  
  return results;
}

LayoutScoreResult LayoutOptimizer::selectBestLayout(
    const SmallVector<LayoutScoreResult, 6> &layouts) {
  if (layouts.empty()) {
    LayoutScoreResult defaultResult;
    defaultResult.permutation = {0, 1, 2};
    return defaultResult;
  }
  
  // 选择总评分最低的布局
  auto bestIt = std::min_element(layouts.begin(), layouts.end(),
      [](const LayoutScoreResult &a, const LayoutScoreResult &b) {
        return a.totalScore < b.totalScore;
      });
  
  return *bestIt;
}

