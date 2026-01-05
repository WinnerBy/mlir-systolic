// ============================================================================
// 基于 ISL 的多面体分析 - 写时重排实现方案
// ============================================================================
// 这个文件展示如何使用 ISL 库进行更精细的重排分析
// ============================================================================

#include <isl/space.h>
#include <isl/map.h>
#include <isl/union_map.h>
#include <isl/val.h>
#include <isl/aff.h>
#include <isl/polynomial.h>

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/AffineStructures.h"

using namespace mlir;
using namespace mlir::affine;

namespace mlir {
namespace systolic {

// ============================================================================
// 第 1 部分：ISL 转换工具
// ============================================================================

class ISLConverter {
public:
  /// 将 AffineExpr 转换为 ISL 表达式
  static isl_aff *convertAffineExprToISL(
      AffineExpr expr, isl_space *space) {
    if (auto cstExpr = expr.dyn_cast<IntegerLiteralExpr>()) {
      return isl_aff_val_on_domain(
          isl_local_space_from_space(isl_space_copy(space)),
          isl_val_int_from_si(isl_space_get_ctx(space), cstExpr.getValue()));
    }
    
    if (auto dimExpr = expr.dyn_cast<AffineDimExpr>()) {
      auto ls = isl_local_space_from_space(isl_space_copy(space));
      return isl_aff_var_on_domain(ls, isl_dim_set, dimExpr.getPosition());
    }
    
    if (auto binExpr = expr.dyn_cast<AffineBinaryOpExpr>()) {
      auto lhs = convertAffineExprToISL(binExpr.getLHS(), space);
      auto rhs = convertAffineExprToISL(binExpr.getRHS(), space);
      
      switch (binExpr.getKind()) {
        case AffineExprKind::Add:
          return isl_aff_add(lhs, rhs);
        case AffineExprKind::Mul:
          // ISL 不支持任意乘法，只能用于仿射表达式
          // 如果 rhs 是常数，我们可以处理
          {
            auto rhsVal = isl_aff_get_constant_val(rhs);
            if (isl_val_is_int(rhsVal)) {
              lhs = isl_aff_scale_val(lhs, isl_val_copy(rhsVal));
              isl_aff_free(rhs);
              isl_val_free(rhsVal);
              return lhs;
            }
            isl_val_free(rhsVal);
            isl_aff_free(lhs);
            isl_aff_free(rhs);
            return nullptr;
          }
        case AffineExprKind::FloorDiv:
        case AffineExprKind::CeilDiv:
        case AffineExprKind::Mod:
          // 这些是非线性的，需要特殊处理
          isl_aff_free(lhs);
          isl_aff_free(rhs);
          return nullptr;
        default:
          isl_aff_free(lhs);
          isl_aff_free(rhs);
          return nullptr;
      }
    }
    
    return nullptr;
  }
  
  /// 将 AffineMap 转换为 ISL map
  static isl_map *convertAffineMapToISL(AffineMap affineMap) {
    auto ctx = isl_ctx_alloc();
    
    // 创建空间：域是循环变量，值域是数组索引
    unsigned numDims = affineMap.getNumDims();
    unsigned numResults = affineMap.getNumResults();
    
    auto domain = isl_space_alloc(ctx, 0, numDims, 0);
    auto space = isl_space_map_from_domain_and_range(
        isl_space_copy(domain),
        isl_space_alloc(ctx, 0, numResults, 0));
    
    // 从仿射表达式构造映射
    auto islMap = isl_map_universe(space);
    
    for (unsigned i = 0; i < numResults; ++i) {
      auto expr = affineMap.getResult(i);
      auto aff = convertAffineExprToISL(expr, domain);
      
      if (!aff) {
        // 非线性表达式，标记为非线性访问
        isl_space_free(domain);
        isl_map_free(islMap);
        return nullptr;
      }
      
      auto constraint = isl_constraint_alloc_equality(
          isl_local_space_from_space(isl_space_copy(domain)));
      // 添加约束：out[i] = expr(in)
      // 这需要复杂的 ISL API 调用，这里简化处理
    }
    
    isl_space_free(domain);
    return islMap;
  }
};

// ============================================================================
// 第 2 部分：多面体分析核心
// ============================================================================

class PolyhedralAccessAnalyzer {
public:
  /// 分析访问的 stride 模式
  struct AccessPattern {
    // 访问的最小 stride
    SmallVector<int64_t, 3> minStride = {1, 1, 1};
    
    // 访问的 reuse distance
    SmallVector<int64_t, 3> reuseDistance = {-1, -1, -1};
    
    // 是否是连续访问（stride == 1）
    SmallVector<bool, 3> isContiguous = {false, false, false};
    
    // 随机访问指数（0 = 完全连续，1 = 完全随机）
    double randomnessScore = 0.0;
  };
  
  /// 分析写入访问模式
  AccessPattern analyzeWriteAccess(
      const SmallVector<AffineMap, 8> &storeMaps,
      const SmallVector<int64_t, 3> &arrayDims) {
    AccessPattern pattern;
    
    if (storeMaps.empty()) return pattern;
    
    // 简化分析：检查每个维度的写入模式
    // 实际实现需要更复杂的多面体分析
    
    return pattern;
  }
  
  /// 分析读取访问模式
  AccessPattern analyzeReadAccess(
      const SmallVector<AffineMap, 8> &loadMaps,
      const SmallVector<int64_t, 3> &arrayDims) {
    AccessPattern pattern;
    
    if (loadMaps.empty()) return pattern;
    
    for (size_t mapIdx = 0; mapIdx < loadMaps.size(); ++mapIdx) {
      auto map = loadMaps[mapIdx];
      
      // 对每个结果（数组维度）分析
      for (unsigned dimIdx = 0; dimIdx < map.getNumResults(); ++dimIdx) {
        auto expr = map.getResult(dimIdx);
        
        // 检查表达式的复杂度
        auto complexity = analyzeExpressionComplexity(expr);
        
        if (complexity > 1.0) {
          // 非线性表达式 -> 随机访问
          pattern.isContiguous[dimIdx] = false;
          pattern.randomnessScore += 0.5;  // 增加随机性评分
        } else {
          // 线性表达式 -> 检查 stride
          auto stride = extractStride(expr);
          if (stride == 1) {
            pattern.isContiguous[dimIdx] = true;
            pattern.minStride[dimIdx] = 1;
          } else {
            pattern.isContiguous[dimIdx] = false;
            pattern.minStride[dimIdx] = stride;
          }
        }
      }
    }
    
    return pattern;
  }
  
private:
  /// 分析表达式的复杂度
  /// 返回值：1.0 = 线性，> 1.0 = 非线性
  double analyzeExpressionComplexity(AffineExpr expr) {
    if (expr.isa<IntegerLiteralExpr>() || expr.isa<AffineDimExpr>()) {
      return 1.0;  // 线性项
    }
    
    if (auto binExpr = expr.dyn_cast<AffineBinaryOpExpr>()) {
      auto lhsComplexity = analyzeExpressionComplexity(binExpr.getLHS());
      auto rhsComplexity = analyzeExpressionComplexity(binExpr.getRHS());
      
      switch (binExpr.getKind()) {
        case AffineExprKind::Add:
        case AffineExprKind::Sub:
          return std::max(lhsComplexity, rhsComplexity);  // 加减不改变线性性
        case AffineExprKind::Mul:
          return std::max(lhsComplexity, rhsComplexity) * 2;  // 乘法引入非线性
        case AffineExprKind::FloorDiv:
        case AffineExprKind::CeilDiv:
        case AffineExprKind::Mod:
          return 3.0;  // 严格非线性
        default:
          return 1.0;
      }
    }
    
    return 1.0;
  }
  
  /// 从线性表达式提取 stride
  /// 假设表达式形式：c0 * d0 + c1 * d1 + ... + const
  int64_t extractStride(AffineExpr expr) {
    // 简化实现：只处理简单的情况
    // 完整实现需要完全展开表达式
    
    if (auto dimExpr = expr.dyn_cast<AffineDimExpr>()) {
      return 1;  // stride = 1
    }
    
    if (auto binExpr = expr.dyn_cast<AffineBinaryOpExpr>()) {
      if (binExpr.getKind() == AffineExprKind::Mul) {
        // 尝试提取乘法系数
        // 形式：c * d，其中 c 是常数，d 是维度
        auto lhs = binExpr.getLHS();
        auto rhs = binExpr.getRHS();
        
        if (auto lhsCst = lhs.dyn_cast<IntegerLiteralExpr>()) {
          return lhsCst.getValue();
        }
        if (auto rhsCst = rhs.dyn_cast<IntegerLiteralExpr>()) {
          return rhsCst.getValue();
        }
      }
    }
    
    return 1;
  }
};

// ============================================================================
// 第 3 部分：布局变换枚举和评分
// ============================================================================

class LayoutOptimizer {
public:
  /// 评分函数类型
  using ScoreFunction = std::function<double(
      const PolyhedralAccessAnalyzer::AccessPattern &,
      const PolyhedralAccessAnalyzer::AccessPattern &,
      const SmallVector<unsigned, 3> &)>;
  
  struct LayoutScoreResult {
    SmallVector<unsigned, 3> permutation;
    double score;
  };
  
  /// 枚举所有 3D 排列并评分
  static SmallVector<LayoutScoreResult, 6> evaluateAllLayouts(
      const PolyhedralAccessAnalyzer::AccessPattern &writeAccess,
      const PolyhedralAccessAnalyzer::AccessPattern &readAccess,
      ScoreFunction scoreFunc) {
    
    SmallVector<LayoutScoreResult, 6> results;
    
    // 3 维有 6 种排列
    unsigned perms[6][3] = {
      {0, 1, 2}, {0, 2, 1}, {1, 0, 2},
      {1, 2, 0}, {2, 0, 1}, {2, 1, 0}
    };
    
    for (int i = 0; i < 6; ++i) {
      SmallVector<unsigned, 3> perm(perms[i], perms[i] + 3);
      double score = scoreFunc(writeAccess, readAccess, perm);
      
      results.push_back({perm, score});
    }
    
    // 按评分排序（升序，低分更好）
    std::sort(results.begin(), results.end(),
              [](const LayoutScoreResult &a, const LayoutScoreResult &b) {
                return a.score < b.score;
              });
    
    return results;
  }
  
  /// 内存访问成本评估函数
  static double evaluateMemoryCost(
      const PolyhedralAccessAnalyzer::AccessPattern &writeAccess,
      const PolyhedralAccessAnalyzer::AccessPattern &readAccess,
      const SmallVector<unsigned, 3> &permutation) {
    
    double cost = 0.0;
    
    // 权重：读取比写入更重要（假设读取更频繁）
    const double readWeight = 0.7;
    const double writeWeight = 0.3;
    
    // 评估读取访问成本
    for (unsigned i = 0; i < 3; ++i) {
      unsigned newDim = permutation[i];
      
      // 如果在重排后是连续访问，成本低
      if (readAccess.isContiguous[newDim]) {
        cost += 0.0 * readWeight;  // 零成本
      } else {
        // 非连续 -> 随机访问 -> 高成本
        cost += readAccess.randomnessScore * readWeight;
      }
    }
    
    // 评估写入访问成本（通常不如读取重要）
    for (unsigned i = 0; i < 3; ++i) {
      unsigned newDim = permutation[i];
      
      if (writeAccess.isContiguous[newDim]) {
        cost += 0.0 * writeWeight;
      } else {
        cost += writeAccess.randomnessScore * writeWeight;
      }
    }
    
    return cost;
  }
  
  /// 缓存局部性评估函数
  static double evaluateCacheLocality(
      const PolyhedralAccessAnalyzer::AccessPattern &,
      const PolyhedralAccessAnalyzer::AccessPattern &readAccess,
      const SmallVector<unsigned, 3> &permutation) {
    
    double score = 0.0;
    
    // 启发式：最后一个维度应该是连续的（最内层循环）
    unsigned lastDim = permutation[2];
    if (readAccess.isContiguous[lastDim]) {
      score += 1.0;  // 额外奖励
    }
    
    return score;
  }
};

// ============================================================================
// 第 4 部分：集成到分析器
// ============================================================================

class EnhancedWriteTimeReorderingAnalyzer {
public:
  /// 使用多面体分析的增强重排计算
  LogicalResult computeReorderingWithISL(ArrayAccessPattern &pattern) {
    // Step 1: 分析访问模式
    PolyhedralAccessAnalyzer analyzer;
    auto writeAccess = analyzer.analyzeWriteAccess(
        pattern.storeMaps, pattern.originalDims);
    auto readAccess = analyzer.analyzeReadAccess(
        pattern.loadMaps, pattern.originalDims);
    
    LLVM_DEBUG({
      llvm::dbgs() << "Write Access Pattern for " << pattern.arrayName << ":\n"
                   << "  Randomness Score: " << writeAccess.randomnessScore << "\n";
      llvm::dbgs() << "Read Access Pattern for " << pattern.arrayName << ":\n"
                   << "  Randomness Score: " << readAccess.randomnessScore << "\n";
    });
    
    // Step 2: 枚举并评分所有布局
    auto layoutScores = LayoutOptimizer::evaluateAllLayouts(
        writeAccess, readAccess,
        [](const auto &w, const auto &r, const auto &p) {
          // 组合评分函数
          auto memoryCost = LayoutOptimizer::evaluateMemoryCost(w, r, p);
          auto cacheScore = LayoutOptimizer::evaluateCacheLocality(w, r, p);
          return memoryCost - cacheScore;  // 最小化成本，最大化缓存局部性
        });
    
    // Step 3: 选择最佳布局
    if (!layoutScores.empty()) {
      auto bestLayout = layoutScores[0];
      pattern.dimPermutation = bestLayout.permutation;
      
      // 计算重排后的维度
      pattern.reorderedDims.clear();
      for (unsigned idx : pattern.dimPermutation) {
        pattern.reorderedDims.push_back(pattern.originalDims[idx]);
      }
      
      LLVM_DEBUG({
        llvm::dbgs() << "Selected layout for " << pattern.arrayName 
                     << " with score " << bestLayout.score << ":\n"
                     << "  Permutation: [" << bestLayout.permutation[0] << ", "
                     << bestLayout.permutation[1] << ", "
                     << bestLayout.permutation[2] << "]\n";
      });
    }
    
    return success();
  }
};

} // namespace systolic
} // namespace mlir

// ============================================================================
// 第 5 部分：使用说明和集成建议
// ============================================================================

/*
集成步骤：

1. 在 lib/Analysis/WriteTimeReorderingAnalysis.cpp 中：

   LogicalResult WriteTimeReorderingAnalyzer::computeReorderingWithISL(
       ArrayAccessPattern &pattern) {
     EnhancedWriteTimeReorderingAnalyzer enhancedAnalyzer;
     return enhancedAnalyzer.computeReorderingWithISL(pattern);
   }

2. 在 CMakeLists.txt 中添加 ISL 依赖：
   find_package(isl REQUIRED)
   target_link_libraries(systolic PUBLIC isl)

3. 修改 analyze() 方法以选择分析策略：

   for (auto &pattern : patterns) {
     if (useISL) {
       // 使用 ISL 多面体分析
       if (failed(computeReorderingWithISL(pattern))) {
         // 回退到简单启发式
         if (failed(computeReordering(pattern))) {
           continue;
         }
       }
     } else {
       // 使用简单启发式
       if (failed(computeReordering(pattern))) {
         continue;
       }
     }
   }

评分指标说明：

- Randomness Score (随机性评分)
  * 0.0 = 完全连续访问
  * 1.0 = 完全随机访问
  
- Memory Cost (内存成本)
  * 低 = 更多连续访问
  * 高 = 更多随机访问
  
- Cache Locality (缓存局部性)
  * 高 = 最内层循环是连续的
  * 低 = 最内层循环是随机的

布局选择策略：
  选择使 (memoryCost - cacheLocality) 最小的排列
*/

