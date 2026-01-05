//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Write-time Reordering Analysis Implementation
//
// This module analyzes array access patterns using polyhedral model (ISL)
// to detect random access issues and compute optimal data layout transformations.
//
//===----------------------------------------------------------------------===//

#include "systolic/Analysis/WriteTimeReorderingAnalysis.h"
#include "systolic/Analysis/PolymerAnalysis.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/AffineStructures.h"
#include "mlir/Dialect/Affine/Analysis/Utils.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "write-time-reordering-analysis"

using namespace mlir;
using namespace mlir::affine;
using namespace mlir::systolic;

//===----------------------------------------------------------------------===//
// WriteTimeReorderingAnalyzer Implementation
//===----------------------------------------------------------------------===//

WriteTimeReorderingAnalyzer::WriteTimeReorderingAnalyzer(func::FuncOp func)
    : func(func) {}

LogicalResult WriteTimeReorderingAnalyzer::analyze() {
  // Step 1: Get array reference groups from SystolicDataflowGeneration
  // For now, we'll analyze directly from the function
  // TODO: Integrate with SystolicDataflowGeneration pass to reuse ArrayRefGroup
  
  // Step 2: Extract access patterns
  // Collect all load/store operations and their access maps
  func.walk([&](Operation *op) {
    if (auto loadOp = dyn_cast<AffineLoadOp>(op)) {
      Value memref = loadOp.getMemRef();
      std::string name = "unknown";
      
      // Get array name
      if (auto memrefType = memref.getType().dyn_cast<MemRefType>()) {
        for (auto arg : func.getArguments()) {
          if (arg == memref) {
            unsigned argNum = arg.getArgNumber();
            if (auto attr = func.getArgAttrOfType<StringAttr>(argNum, "mlir.name")) {
              name = attr.getValue().str();
            } else {
              name = "arg" + std::to_string(argNum);
            }
            break;
          }
        }
      }
      
      // Find or create pattern
      size_t patternIdx;
      if (auto it = arrayNameToPattern.find(name); it != arrayNameToPattern.end()) {
        patternIdx = it->second;
      } else {
        patternIdx = patterns.size();
        patterns.emplace_back(memref, name);
        arrayNameToPattern[name] = patternIdx;
        
        // Get array dimensions
        if (auto memrefType = memref.getType().dyn_cast<MemRefType>()) {
          for (int64_t dim : memrefType.getShape()) {
            patterns[patternIdx].originalDims.push_back(dim);
          }
        }
      }
      
      patterns[patternIdx].loadOps.push_back(loadOp);
      
      // Get the AffineMap from the load operation
      AffineMap loadMap = loadOp.getAffineMap();
      patterns[patternIdx].loadMaps.push_back(loadMap);
      
      // Check if any index operand comes from affine.apply
      // This is important for detecting non-linear access patterns
      // The map operands are the SSA values used as indices
      for (Value idx : loadOp.getMapOperands()) {
        if (auto applyOp = idx.getDefiningOp<AffineApplyOp>()) {
          // This index comes from affine.apply, check its map for non-linear expressions
          AffineMap applyMap = applyOp.getAffineMap();
          // Add the apply map to check for non-linear expressions
          patterns[patternIdx].loadMaps.push_back(applyMap);
          LLVM_DEBUG(llvm::dbgs() << "Found affine.apply for index: " << applyMap << "\n");
        }
      }
    } else if (auto storeOp = dyn_cast<AffineStoreOp>(op)) {
      Value memref = storeOp.getMemRef();
      std::string name = "unknown";
      
      if (auto memrefType = memref.getType().dyn_cast<MemRefType>()) {
        for (auto arg : func.getArguments()) {
          if (arg == memref) {
            unsigned argNum = arg.getArgNumber();
            if (auto attr = func.getArgAttrOfType<StringAttr>(argNum, "mlir.name")) {
              name = attr.getValue().str();
            } else {
              name = "arg" + std::to_string(argNum);
            }
            break;
          }
        }
      }
      
      size_t patternIdx;
      if (auto it = arrayNameToPattern.find(name); it != arrayNameToPattern.end()) {
        patternIdx = it->second;
      } else {
        patternIdx = patterns.size();
        patterns.emplace_back(memref, name);
        arrayNameToPattern[name] = patternIdx;
        
        if (auto memrefType = memref.getType().dyn_cast<MemRefType>()) {
          for (int64_t dim : memrefType.getShape()) {
            patterns[patternIdx].originalDims.push_back(dim);
          }
        }
      }
      
      patterns[patternIdx].storeOps.push_back(storeOp);
      patterns[patternIdx].storeMaps.push_back(storeOp.getAffineMap());
    }
  });
  
  // Step 3: Analyze each pattern
  for (auto &pattern : patterns) {
    if (failed(analyzePattern(pattern))) {
      LLVM_DEBUG(llvm::dbgs() << "Failed to analyze pattern for " 
                              << pattern.arrayName << "\n");
      continue;
    }
  }
  
  return success();
}

LogicalResult WriteTimeReorderingAnalyzer::analyzePattern(ArrayAccessPattern &pattern) {
  // Check if any load map contains non-linear expressions
  for (auto loadMap : pattern.loadMaps) {
    if (hasNonLinearExpression(loadMap)) {
      pattern.hasNonLinearAccess = true;
      break;
    }
  }
  
  // If no non-linear access, no reordering needed
  if (!pattern.hasNonLinearAccess) {
    LLVM_DEBUG(llvm::dbgs() << "Array " << pattern.arrayName 
                            << " has no non-linear access, no reordering needed\n");
    return success();
  }
  
  // Find which dimension has non-linear index
  // For each load map, check each result expression
  for (auto loadMap : pattern.loadMaps) {
    for (unsigned i = 0; i < loadMap.getNumResults(); i++) {
      auto expr = loadMap.getResult(i);
      if (isNonLinearExpr(expr)) {
        pattern.nonLinearDim = i;
        LLVM_DEBUG(llvm::dbgs() << "Found non-linear access in dimension " 
                                << i << " for array " << pattern.arrayName << "\n");
        break;
      }
    }
    if (pattern.nonLinearDim >= 0) break;
  }
  
  // Compute reordering
  if (pattern.nonLinearDim >= 0) {
    return computeReordering(pattern);
  }
  
  return success();
}

bool WriteTimeReorderingAnalyzer::hasNonLinearExpression(AffineMap map) {
  for (auto expr : map.getResults()) {
    if (isNonLinearExpr(expr)) {
      return true;
    }
  }
  return false;
}

bool WriteTimeReorderingAnalyzer::isNonLinearExpr(AffineExpr expr) {
  // Check if expression contains multiplication, division, or modulo
  // Walk the expression tree
  if (expr.isa<AffineBinaryOpExpr>()) {
    auto binOp = expr.cast<AffineBinaryOpExpr>();
    auto kind = binOp.getKind();
    
    // Multiplication, division, or modulo are non-linear
    if (kind == AffineExprKind::Mul || 
        kind == AffineExprKind::FloorDiv ||
        kind == AffineExprKind::CeilDiv ||
        kind == AffineExprKind::Mod) {
      return true;
    }
    
    // Recursively check operands
    return isNonLinearExpr(binOp.getLHS()) || isNonLinearExpr(binOp.getRHS());
  }
  
  return false;
}

LogicalResult WriteTimeReorderingAnalyzer::computeReordering(ArrayAccessPattern &pattern) {
  // For 3D arrays, move non-linear dimension to middle
  if (pattern.originalDims.size() != 3) {
    LLVM_DEBUG(llvm::dbgs() << "Reordering only supported for 3D arrays, "
                            << "array " << pattern.arrayName 
                            << " has " << pattern.originalDims.size() << " dimensions\n");
    return success();  // Not an error, just not supported yet
  }
  
  // Strategy: move non-linear dimension to middle position
  // Original: [dim0][dim1][dim2]
  // If non-linear at dim0: [dim1][dim0][dim2]
  // If non-linear at dim2: [dim1][dim2][dim0]
  // If non-linear at dim1: no change needed (already in middle)
  
  pattern.reorderedDims.clear();
  pattern.dimPermutation.clear();
  
  if (pattern.nonLinearDim == 0) {
    // Move first to middle: [0,1,2] -> [1,0,2]
    pattern.reorderedDims = {
      pattern.originalDims[1],
      pattern.originalDims[0],
      pattern.originalDims[2]
    };
    pattern.dimPermutation = {1, 0, 2};
  } else if (pattern.nonLinearDim == 2) {
    // Move last to middle: [0,1,2] -> [1,2,0]
    pattern.reorderedDims = {
      pattern.originalDims[1],
      pattern.originalDims[2],
      pattern.originalDims[0]
    };
    pattern.dimPermutation = {1, 2, 0};
  } else {
    // Already in middle, no change
    pattern.reorderedDims = pattern.originalDims;
    pattern.dimPermutation = {0, 1, 2};
  }
  
  LLVM_DEBUG(llvm::dbgs() << "Computed reordering for " << pattern.arrayName 
                          << ": [" << pattern.originalDims[0] << "][" 
                          << pattern.originalDims[1] << "][" 
                          << pattern.originalDims[2] << "] -> ["
                          << pattern.reorderedDims[0] << "]["
                          << pattern.reorderedDims[1] << "]["
                          << pattern.reorderedDims[2] << "]\n");
  
  return success();
}

LogicalResult WriteTimeReorderingAnalyzer::computeReorderingWithISL(ArrayAccessPattern &pattern) {
  // TODO: Use ISL to compute optimal reordering
  // This would involve:
  // 1. Building access relations from AffineMaps
  // 2. Using ISL to analyze access patterns
  // 3. Computing layout transformation that minimizes non-linear access
  // 4. Computing loop transformations to match layout transformation
  
  // For now, use simple heuristic
  return computeReordering(pattern);
}

bool WriteTimeReorderingAnalyzer::needsReordering(StringRef arrayName) const {
  auto it = arrayNameToPattern.find(arrayName);
  if (it == arrayNameToPattern.end()) {
    return false;
  }
  return patterns[it->second].hasNonLinearAccess && 
         !patterns[it->second].reorderedDims.empty();
}

bool WriteTimeReorderingAnalyzer::getReordering(StringRef arrayName,
                                                SmallVector<int64_t, 3> &reorderedDims,
                                                SmallVector<unsigned, 3> &dimPermutation) const {
  auto it = arrayNameToPattern.find(arrayName);
  if (it == arrayNameToPattern.end()) {
    return false;
  }
  
  const auto &pattern = patterns[it->second];
  if (!pattern.hasNonLinearAccess || pattern.reorderedDims.empty()) {
    return false;
  }
  
  reorderedDims = pattern.reorderedDims;
  dimPermutation = pattern.dimPermutation;
  return true;
}

