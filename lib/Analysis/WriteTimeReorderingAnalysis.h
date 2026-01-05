//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Write-time Reordering Analysis
//
// This module analyzes array access patterns using polyhedral model (ISL)
// to detect random access issues and compute optimal data layout transformations.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_WRITETIMEREORDERINGANALYSIS_H
#define SYSTOLIC_ANALYSIS_WRITETIMEREORDERINGANALYSIS_H

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/IR/AffineValueMap.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/BuiltinOps.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/DenseMap.h"
#include <vector>
#include <string>

namespace mlir {
namespace affine {
class AffineLoadOp;
class AffineStoreOp;
}
namespace systolic {

//===----------------------------------------------------------------------===//
// Array Access Pattern Analysis
//===----------------------------------------------------------------------===//

/// Represents an array access pattern for write-time reordering analysis
struct ArrayAccessPattern {
  Value memref;
  std::string arrayName;
  
  // Access operations
  SmallVector<affine::AffineLoadOp, 8> loads;
  SmallVector<affine::AffineStoreOp, 8> stores;
  
  // Access maps extracted from operations
  SmallVector<AffineMap, 8> loadMaps;   // Maps from loop indices to array indices
  SmallVector<AffineMap, 8> storeMaps;
  
  // Original array dimensions
  SmallVector<int64_t, 3> originalDims;
  
  // Analysis results
  bool hasNonLinearAccess = false;
  int nonLinearDim = -1;  // Dimension with non-linear index expression
  
  // Reordering solution
  SmallVector<int64_t, 3> reorderedDims;
  SmallVector<unsigned, 3> dimPermutation;  // Permutation: new_dim[i] = originalDims[dimPermutation[i]]
  
  ArrayAccessPattern(Value memref, StringRef name)
      : memref(memref), arrayName(name.str()) {}
};

//===----------------------------------------------------------------------===//
// Write-time Reordering Analyzer
//===----------------------------------------------------------------------===//

/// Analyzes array access patterns using polyhedral model to detect
/// random access issues and compute optimal data layout transformations.
class WriteTimeReorderingAnalyzer {
public:
  WriteTimeReorderingAnalyzer(func::FuncOp func);
  
  /// Analyze all array access patterns in the function
  LogicalResult analyze();
  
  /// Get reordering information for a specific array
  /// Returns true if reordering is needed and computed
  bool getReordering(StringRef arrayName, 
                    SmallVector<int64_t, 3> &reorderedDims,
                    SmallVector<unsigned, 3> &dimPermutation) const;
  
  /// Check if an array needs reordering
  bool needsReordering(StringRef arrayName) const;
  
  /// Get all analyzed patterns
  const std::vector<ArrayAccessPattern> &getPatterns() const { return patterns; }

private:
  func::FuncOp func;
  std::vector<ArrayAccessPattern> patterns;
  llvm::DenseMap<StringRef, size_t> arrayNameToPattern;
  
  /// Extract access patterns from ArrayRefGroup
  LogicalResult extractAccessPatterns(const std::vector<ArrayRefGroup> &groups);
  
  /// Analyze a single access pattern
  LogicalResult analyzePattern(ArrayAccessPattern &pattern);
  
  /// Check if an AffineMap contains non-linear expressions
  bool hasNonLinearExpression(AffineMap map);
  
  /// Check if an AffineExpr is non-linear
  bool isNonLinearExpr(AffineExpr expr);
  
  /// Compute optimal reordering using polyhedral analysis
  LogicalResult computeReordering(ArrayAccessPattern &pattern);
  
  /// Use ISL to analyze access relations and compute layout transformation
  LogicalResult computeReorderingWithISL(ArrayAccessPattern &pattern);
};

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_WRITETIMEREORDERINGANALYSIS_H

