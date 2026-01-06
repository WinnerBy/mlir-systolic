//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Polyhedral Access Pattern Analyzer Implementation
//
//===----------------------------------------------------------------------===//

#include "systolic/Analysis/PolyhedralAccessAnalyzer.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/AffineStructures.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "polyhedral-access-analyzer"

using namespace mlir;
using namespace mlir::systolic;

//===----------------------------------------------------------------------===//
// Dimension Access Pattern Analysis
//===----------------------------------------------------------------------===//

DimensionAccessPattern PolyhedralAccessAnalyzer::analyzeDimension(
    unsigned dimIdx,
    const SmallVector<AffineMap, 8> &maps,
    const SmallVector<int64_t, 3> &arrayDims) {
  DimensionAccessPattern pattern;
  
  if (maps.empty() || dimIdx >= arrayDims.size()) {
    return pattern;
  }
  
  // 分析该维度在所有访问映射中的模式
  bool hasNonLinear = false;
  int64_t minStride = arrayDims[dimIdx];  // 初始化为最大可能值
  unsigned accessCount = 0;
  
  for (auto map : maps) {
    if (dimIdx >= map.getNumResults()) {
      continue;
    }
    
    auto expr = map.getResult(dimIdx);
    accessCount++;
    
    // 检查是否有非线性表达式
    if (auto binOp = expr.dyn_cast<AffineBinaryOpExpr>()) {
      auto kind = binOp.getKind();
      if (kind == AffineExprKind::Mul || 
          kind == AffineExprKind::FloorDiv ||
          kind == AffineExprKind::CeilDiv ||
          kind == AffineExprKind::Mod) {
        hasNonLinear = true;
        pattern.hasNonLinear = true;
      }
    }
    
    // 计算 stride
    int64_t stride = computeStride(expr, dimIdx);
    if (stride > 0 && stride < minStride) {
      minStride = stride;
    }
  }
  
  pattern.minStride = minStride;
  pattern.isContiguous = (minStride == 1);
  pattern.hasNonLinear = hasNonLinear;
  pattern.accessFrequency = accessCount;
  
  return pattern;
}

int64_t PolyhedralAccessAnalyzer::computeStride(AffineExpr expr, unsigned dimIdx) {
  // 简化实现：检查表达式是否直接使用 dimIdx
  // 如果是 dim[i]，stride = 1
  // 如果是 dim[i] * k，stride = k
  // 如果是 dim[i] + k，stride = 1
  
  if (auto dimExpr = expr.dyn_cast<AffineDimExpr>()) {
    if (dimExpr.getPosition() == dimIdx) {
      return 1;  // 直接使用该维度，stride = 1
    }
    return -1;  // 不使用该维度
  }
  
  if (auto binOp = expr.dyn_cast<AffineBinaryOpExpr>()) {
    if (binOp.getKind() == AffineExprKind::Add) {
      // 检查左右操作数
      int64_t lhsStride = computeStride(binOp.getLHS(), dimIdx);
      int64_t rhsStride = computeStride(binOp.getRHS(), dimIdx);
      
      if (lhsStride > 0 && rhsStride < 0) {
        return lhsStride;
      }
      if (rhsStride > 0 && lhsStride < 0) {
        return rhsStride;
      }
      // 如果两边都使用该维度，取最小值
      if (lhsStride > 0 && rhsStride > 0) {
        return std::min(lhsStride, rhsStride);
      }
    } else if (binOp.getKind() == AffineExprKind::Mul) {
      // 检查是否是常数乘法
      if (auto constExpr = binOp.getLHS().dyn_cast<AffineConstantExpr>()) {
        int64_t rhsStride = computeStride(binOp.getRHS(), dimIdx);
        if (rhsStride > 0) {
          return constExpr.getValue() * rhsStride;
        }
      }
      if (auto constExpr = binOp.getRHS().dyn_cast<AffineConstantExpr>()) {
        int64_t lhsStride = computeStride(binOp.getLHS(), dimIdx);
        if (lhsStride > 0) {
          return constExpr.getValue() * lhsStride;
        }
      }
    }
  }
  
  return -1;  // 无法确定 stride
}

bool PolyhedralAccessAnalyzer::isLinearInDimension(AffineExpr expr, unsigned dimIdx) {
  // 检查表达式是否在该维度上是线性的
  if (auto dimExpr = expr.dyn_cast<AffineDimExpr>()) {
    return dimExpr.getPosition() == dimIdx;
  }
  
  if (auto binOp = expr.dyn_cast<AffineBinaryOpExpr>()) {
    if (binOp.getKind() == AffineExprKind::Add) {
      return isLinearInDimension(binOp.getLHS(), dimIdx) ||
             isLinearInDimension(binOp.getRHS(), dimIdx);
    }
    if (binOp.getKind() == AffineExprKind::Mul) {
      // 检查是否是常数乘法
      bool lhsIsConst = binOp.getLHS().isa<AffineConstantExpr>();
      bool rhsIsConst = binOp.getRHS().isa<AffineConstantExpr>();
      if (lhsIsConst) {
        return isLinearInDimension(binOp.getRHS(), dimIdx);
      }
      if (rhsIsConst) {
        return isLinearInDimension(binOp.getLHS(), dimIdx);
      }
      return false;  // 两个变量相乘是非线性的
    }
    // 其他操作（除法、取模）都是非线性的
    return false;
  }
  
  return false;
}

double PolyhedralAccessAnalyzer::computeRandomnessScore(const AccessPattern &pattern) {
  double score = 0.0;
  unsigned numDims = pattern.dimPatterns.size();
  
  if (numDims == 0) {
    return 0.0;
  }
  
  // 对每个维度计算随机性
  for (const auto &dimPattern : pattern.dimPatterns) {
    double dimScore = 0.0;
    
    // 非线性访问增加随机性
    if (dimPattern.hasNonLinear) {
      dimScore += 0.5;
    }
    
    // 非连续访问增加随机性
    if (!dimPattern.isContiguous) {
      dimScore += 0.3 * (1.0 - 1.0 / static_cast<double>(dimPattern.minStride));
    }
    
    score += dimScore / numDims;
  }
  
  return std::min(1.0, score);
}

//===----------------------------------------------------------------------===//
// Access Pattern Analysis
//===----------------------------------------------------------------------===//

AccessPattern PolyhedralAccessAnalyzer::analyzeWriteAccess(
    const SmallVector<AffineMap, 8> &storeMaps,
    const SmallVector<int64_t, 3> &arrayDims) {
  AccessPattern pattern;
  
  if (storeMaps.empty() || arrayDims.size() != 3) {
    return pattern;
  }
  
  // 分析每个维度
  for (unsigned i = 0; i < 3; i++) {
    pattern.writeDimPatterns.push_back(analyzeDimension(i, storeMaps, arrayDims));
  }
  
  pattern.dimPatterns = pattern.writeDimPatterns;
  pattern.randomnessScore = computeRandomnessScore(pattern);
  
  return pattern;
}

AccessPattern PolyhedralAccessAnalyzer::analyzeReadAccess(
    const SmallVector<AffineMap, 8> &loadMaps,
    const SmallVector<int64_t, 3> &arrayDims) {
  AccessPattern pattern;
  
  if (loadMaps.empty() || arrayDims.size() != 3) {
    return pattern;
  }
  
  // 分析每个维度
  for (unsigned i = 0; i < 3; i++) {
    pattern.readDimPatterns.push_back(analyzeDimension(i, loadMaps, arrayDims));
  }
  
  pattern.dimPatterns = pattern.readDimPatterns;
  pattern.randomnessScore = computeRandomnessScore(pattern);
  
  return pattern;
}

AccessPattern PolyhedralAccessAnalyzer::analyzeCombinedAccess(
    const SmallVector<AffineMap, 8> &loadMaps,
    const SmallVector<AffineMap, 8> &storeMaps,
    const SmallVector<int64_t, 3> &arrayDims) {
  AccessPattern pattern;
  
  if (arrayDims.size() != 3) {
    return pattern;
  }
  
  // 分别分析读取和写入
  if (!loadMaps.empty()) {
    pattern.readDimPatterns.resize(3);
    for (unsigned i = 0; i < 3; i++) {
      pattern.readDimPatterns[i] = analyzeDimension(i, loadMaps, arrayDims);
    }
  }
  
  if (!storeMaps.empty()) {
    pattern.writeDimPatterns.resize(3);
    for (unsigned i = 0; i < 3; i++) {
      pattern.writeDimPatterns[i] = analyzeDimension(i, storeMaps, arrayDims);
    }
  }
  
  // 合并读取和写入模式（取更差的）
  pattern.dimPatterns.resize(3);
  for (unsigned i = 0; i < 3; i++) {
    DimensionAccessPattern combined;
    
    if (i < pattern.readDimPatterns.size() && i < pattern.writeDimPatterns.size()) {
      const auto &read = pattern.readDimPatterns[i];
      const auto &write = pattern.writeDimPatterns[i];
      
      // 取更差的模式（更高的随机性）
      combined.hasNonLinear = read.hasNonLinear || write.hasNonLinear;
      combined.minStride = std::max(read.minStride, write.minStride);
      combined.isContiguous = read.isContiguous && write.isContiguous;
      combined.accessFrequency = read.accessFrequency + write.accessFrequency;
    } else if (i < pattern.readDimPatterns.size()) {
      combined = pattern.readDimPatterns[i];
    } else if (i < pattern.writeDimPatterns.size()) {
      combined = pattern.writeDimPatterns[i];
    }
    
    pattern.dimPatterns[i] = combined;
  }
  
  pattern.randomnessScore = computeRandomnessScore(pattern);
  
  return pattern;
}

