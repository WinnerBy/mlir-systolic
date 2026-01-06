//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Polyhedral Access Pattern Analyzer
//
// This module analyzes array access patterns to compute stride, reuse distance,
// and randomness scores for layout optimization.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_POLYHEDRALACCESSANALYZER_H
#define SYSTOLIC_ANALYSIS_POLYHEDRALACCESSANALYZER_H

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "llvm/ADT/SmallVector.h"
#include <vector>

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Access Pattern Analysis
//===----------------------------------------------------------------------===//

/// Represents the access pattern characteristics for a dimension
struct DimensionAccessPattern {
  // 最小 stride（相邻访问之间的距离）
  int64_t minStride = 1;
  
  // 重用距离（同一数据被重复访问的间隔）
  int64_t reuseDistance = -1;  // -1 表示未计算
  
  // 是否是连续访问（stride == 1）
  bool isContiguous = false;
  
  // 是否有非线性访问（除法、取模等）
  bool hasNonLinear = false;
  
  // 访问频率（在循环中的访问次数）
  unsigned accessFrequency = 0;
};

/// Represents the complete access pattern for an array
struct AccessPattern {
  // 每个维度的访问模式
  SmallVector<DimensionAccessPattern, 3> dimPatterns;
  
  // 随机性评分（0.0 = 完全连续，1.0 = 完全随机）
  double randomnessScore = 0.0;
  
  // 写入访问模式（如果有）
  SmallVector<DimensionAccessPattern, 3> writeDimPatterns;
  
  // 读取访问模式
  SmallVector<DimensionAccessPattern, 3> readDimPatterns;
};

//===----------------------------------------------------------------------===//
// Polyhedral Access Analyzer
//===----------------------------------------------------------------------===//

/// Analyzes array access patterns to compute stride, reuse distance, and
/// randomness scores for layout optimization.
class PolyhedralAccessAnalyzer {
public:
  /// Analyze write access patterns
  AccessPattern analyzeWriteAccess(
      const SmallVector<AffineMap, 8> &storeMaps,
      const SmallVector<int64_t, 3> &arrayDims);
  
  /// Analyze read access patterns
  AccessPattern analyzeReadAccess(
      const SmallVector<AffineMap, 8> &loadMaps,
      const SmallVector<int64_t, 3> &arrayDims);
  
  /// Analyze combined access patterns (both read and write)
  AccessPattern analyzeCombinedAccess(
      const SmallVector<AffineMap, 8> &loadMaps,
      const SmallVector<AffineMap, 8> &storeMaps,
      const SmallVector<int64_t, 3> &arrayDims);

private:
  /// Analyze a single dimension's access pattern
  DimensionAccessPattern analyzeDimension(
      unsigned dimIdx,
      const SmallVector<AffineMap, 8> &maps,
      const SmallVector<int64_t, 3> &arrayDims);
  
  /// Compute stride for a dimension
  int64_t computeStride(AffineExpr expr, unsigned dimIdx);
  
  /// Check if expression is linear in a specific dimension
  bool isLinearInDimension(AffineExpr expr, unsigned dimIdx);
  
  /// Compute randomness score
  double computeRandomnessScore(const AccessPattern &pattern);
};

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_POLYHEDRALACCESSANALYZER_H

