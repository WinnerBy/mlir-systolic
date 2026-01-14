//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Parametric Space-Time Configuration
//
// This module defines the parametric representation of space-time 
// configurations for systolic array generation, supporting ST0–ST5 and
// beyond. Unlike the hardcoded spacetime=3 in the original implementation,
// this provides a unified, extensible framework.
//
// AutoSA Reference: struct autosa_kernel in autosa_common.h
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_ANALYSIS_PARAMETRISPACETIME_H
#define SYSTOLIC_ANALYSIS_PARAMETRISPACETIME_H

#include "mlir/IR/Value.h"
#include "llvm/ADT/DenseMap.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include <string>

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Data Flow Direction (from SpaceTimeAnalysis.h)
//===----------------------------------------------------------------------===//

enum class SystolicFlowDir {
  NONE,               // No systolic flow, local access (e.g., C in output-stationary)
  HORIZONTAL,         // Data flows along j-axis (PE[i,j] -> PE[i,j+1])
  VERTICAL,           // Data flows along i-axis (PE[i,j] -> PE[i+1,j])
  REDUCE_HORIZONTAL,  // Reduction flows horizontally
  REDUCE_VERTICAL,    // Reduction flows vertically
};

//===----------------------------------------------------------------------===//
// Space-Time Configuration
//===----------------------------------------------------------------------===//

/// Describes the configuration for a single space dimension.
struct SpaceDimConfig {
  /// Loop dimension index in the original nest
  unsigned loopDim;
  
  /// Loop name for debugging (e.g., "i", "j")
  std::string loopName;
  
  /// PE array size in this dimension (e.g., 16, 32)
  int64_t peArraySize;
  
  /// Parallelism level (number of PEs along this dimension)
  unsigned parallelism;
  
  /// Pipeline initiation interval (II) for this dimension
  unsigned pipelineII;
  
  /// SIMD width for this dimension (1 = no vectorization)
  unsigned simdWidth;
  
  SpaceDimConfig()
      : loopDim(0), loopName(""), peArraySize(1), parallelism(1),
        pipelineII(1), simdWidth(1) {}
};

/// Describes the configuration for the time dimension(s).
struct TimeDimConfig {
  /// Loop dimension indices in the original nest (can be multiple for reduction)
  llvm::SmallVector<unsigned, 2> loopDims;
  
  /// Loop names for debugging
  llvm::SmallVector<std::string, 2> loopNames;
  
  /// Target pipeline depth for the innermost time loop
  unsigned pipelineDepth;
  
  /// Latency hiding length (-1 for auto, 0 for disabled)
  int latencyHidingLength;
  
  /// Whether to enable double buffering for time loops
  bool doubleBuffer;
  
  TimeDimConfig()
      : pipelineDepth(1), latencyHidingLength(-1), doubleBuffer(true) {}
};

/// Describes the configuration for reduction dimensions (if present).
/// Reduction loops are typically processed differently from space/time loops.
struct ReductionDimConfig {
  /// Loop dimension index in the original nest
  unsigned loopDim;
  
  /// Loop name for debugging
  std::string loopName;
  
  /// Reduction operation (e.g., "add", "mul")
  std::string reductionOp;
  
  ReductionDimConfig() : loopDim(0), loopName(""), reductionOp("") {}
};

//===----------------------------------------------------------------------===//
// Parametric Space-Time Configuration
//===----------------------------------------------------------------------===//

/// Unified parametric representation of space-time configuration.
/// This replaces the hardcoded "spacetime=3" assumption and supports ST0–ST5.
///
/// Example instantiations:
///   ST0: space=[0], time=[1,2], pe_dims=1
///   ST1: space=[1], time=[0,2], pe_dims=1
///   ST3: space=[0,1], time=[2], pe_dims=2 (output-stationary)
///   ST4: space=[0,2], time=[1], pe_dims=2 (weight-stationary)
///   ST5: space=[1,2], time=[0], pe_dims=2 (activation-stationary)
class ParametricSpaceTime {
public:
  //===--------------------------------------------------------------------===//
  // Constructors & Accessors
  //===--------------------------------------------------------------------===//
  
  ParametricSpaceTime();
  ~ParametricSpaceTime() = default;
  
  /// Get the number of space dimensions (1–2 for PE array)
  unsigned getNumSpaceDims() const { return spaceDimConfigs.size(); }
  
  /// Get the number of time dimensions
  unsigned getNumTimeDims() const { return timeDimConfig.loopDims.size(); }
  
  /// Get the number of reduction dimensions
  unsigned getNumReductionDims() const { return reductionDimConfig.loopDim > 0 ? 1 : 0; }
  
  /// Get PE array dimensions (= num space dims)
  unsigned getPEArrayDims() const { return getNumSpaceDims(); }
  
  /// Check if 2D PE array
  bool is2DArray() const { return getNumSpaceDims() == 2; }
  
  /// Check if 1D PE array
  bool is1DArray() const { return getNumSpaceDims() == 1; }
  
  //===--------------------------------------------------------------------===//
  // Configuration Accessors
  //===--------------------------------------------------------------------===//
  
  /// Get space dimension configuration at index i
  const SpaceDimConfig &getSpaceDimConfig(unsigned i) const {
    assert(i < spaceDimConfigs.size());
    return spaceDimConfigs[i];
  }
  
  /// Get mutable space dimension configuration
  SpaceDimConfig &getSpaceDimConfig(unsigned i) {
    assert(i < spaceDimConfigs.size());
    return spaceDimConfigs[i];
  }
  
  /// Add a space dimension
  void addSpaceDim(unsigned loopDim, llvm::StringRef loopName,
                   int64_t peArraySize = 16, unsigned parallelism = 16) {
    SpaceDimConfig cfg;
    cfg.loopDim = loopDim;
    cfg.loopName = loopName.str();
    cfg.peArraySize = peArraySize;
    cfg.parallelism = parallelism;
    spaceDimConfigs.push_back(cfg);
  }
  
  /// Get time dimension configuration
  const TimeDimConfig &getTimeDimConfig() const { return timeDimConfig; }
  
  /// Get mutable time dimension configuration
  TimeDimConfig &getTimeDimConfig() { return timeDimConfig; }
  
  /// Add a time dimension
  void addTimeDim(unsigned loopDim, llvm::StringRef loopName,
                  unsigned pipelineDepth = 1) {
    timeDimConfig.loopDims.push_back(loopDim);
    timeDimConfig.loopNames.push_back(loopName.str());
    timeDimConfig.pipelineDepth = std::max(timeDimConfig.pipelineDepth, pipelineDepth);
  }
  
  /// Get reduction dimension configuration
  const ReductionDimConfig &getReductionDimConfig() const {
    return reductionDimConfig;
  }
  
  /// Set reduction dimension
  void setReductionDim(unsigned loopDim, llvm::StringRef loopName,
                       llvm::StringRef reductionOp = "add") {
    reductionDimConfig.loopDim = loopDim;
    reductionDimConfig.loopName = loopName.str();
    reductionDimConfig.reductionOp = reductionOp.str();
  }
  
  /// Check if this configuration has a reduction dimension
  bool hasReductionDim() const { return reductionDimConfig.loopDim > 0; }
  
  //===--------------------------------------------------------------------===//
  // Data Flow Configuration
  //===--------------------------------------------------------------------===//
  
  /// Set data flow direction for a value/operand
  void setOperandFlow(mlir::Value operand, SystolicFlowDir dir) {
    operandFlows[operand] = dir;
  }
  
  /// Get data flow direction for a value/operand
  SystolicFlowDir getOperandFlow(mlir::Value operand) const {
    auto it = operandFlows.find(operand);
    return it != operandFlows.end() ? it->second : SystolicFlowDir::NONE;
  }
  
  /// Get all operands and their flow directions
  const llvm::DenseMap<mlir::Value, SystolicFlowDir> &getOperandFlows() const {
    return operandFlows;
  }
  
  /// Check if all flows are compatible (e.g., no conflicts)
  bool isFlowConfigurationValid() const;
  
  //===--------------------------------------------------------------------===//
  // Validation & Utility
  //===--------------------------------------------------------------------===//
  
  /// Validate this configuration against loop structure
  bool isValid() const;
  
  /// Get space-time type string (e.g., "ST3", "ST0", etc.)
  std::string getSpaceTimeTypeString() const;
  
  /// Get a detailed configuration string
  std::string toString() const;
  
  /// Dump debug information to stderr
  void dump() const;
  
  /// Print to stream
  void print(llvm::raw_ostream &os) const;
  
  /// Create a parametric configuration from loop indices
  /// This dynamically constructs a configuration based on which loops
  /// are designated as space vs time loops.
  static ParametricSpaceTime createFromLoopIndices(
      const llvm::SmallVector<unsigned> &spaceLoopIndices,
      const llvm::SmallVector<unsigned> &timeLoopIndices,
      const llvm::SmallVector<llvm::StringRef> &loopNames);
  
  /// Create a parametric configuration from a numeric mode (0-5)
  /// This factory function maps AutoSA's spaceTimeMode enum to ParametricSpaceTime
  static ParametricSpaceTime createFromMode(unsigned mode);
  
  //===--------------------------------------------------------------------===//
  // Configuration ID (for dynamic enumeration)
  //===--------------------------------------------------------------------===//
  
  /// Set configuration ID (corresponds to AutoSA's space_time_id)
  void setConfigId(unsigned id) { configId = id; }
  
  /// Get configuration ID
  unsigned getConfigId() const { return configId; }

private:
  /// Configuration ID (for dynamic enumeration, corresponds to AutoSA's space_time_id)
  unsigned configId = 0;
  
  /// Configurations for each space dimension (1–2 elements for typical systolic)
  llvm::SmallVector<SpaceDimConfig, 2> spaceDimConfigs;
  
  /// Configuration for time dimensions
  TimeDimConfig timeDimConfig;
  
  /// Configuration for reduction dimension (if present)
  ReductionDimConfig reductionDimConfig;
  
  /// Data flow direction for each memory operand
  llvm::DenseMap<mlir::Value, SystolicFlowDir> operandFlows;
};

//===----------------------------------------------------------------------===//
// Predefined Space-Time Configurations (ST0–ST5)
//===----------------------------------------------------------------------===//

namespace presets {

/// ST0: Single space dimension at loop 0 (1D PE row).
/// Space: [0], Time: [1, 2], PE: 1D
/// Typical for: Output-stationary with single space dimension
ParametricSpaceTime createST0();

/// ST1: Single space dimension at loop 1 (1D PE column).
/// Space: [1], Time: [0, 2], PE: 1D
/// Typical for: Variant of output-stationary
ParametricSpaceTime createST1();

/// ST2: Single space dimension at loop 2 (1D PE depth, rare).
/// Space: [2], Time: [0, 1], PE: 1D
/// Typical for: Special cases with unusual access patterns
ParametricSpaceTime createST2();

/// ST3: Two space dimensions at loops 0, 1 (2D PE, output-stationary).
/// Space: [0, 1], Time: [2], PE: 2D
/// Typical for: Matrix multiplication and similar operations
ParametricSpaceTime createST3();

/// ST4: Two space dimensions at loops 0, 2 (2D PE, weight-stationary).
/// Space: [0, 2], Time: [1], PE: 2D
/// Typical for: Variant with different data flow pattern
ParametricSpaceTime createST4();

/// ST5: Two space dimensions at loops 1, 2 (2D PE, activation-stationary).
/// Space: [1, 2], Time: [0], PE: 2D
/// Typical for: Another variant with different data flow pattern
ParametricSpaceTime createST5();

} // namespace presets

/// Create a parametric configuration from a numeric mode (0-5)
/// This factory function maps AutoSA's spaceTimeMode enum to ParametricSpaceTime
/// 
/// Modes:
///   0: ST0 (space=[0], 1D PE)
///   1: ST1 (space=[1], 1D PE)
///   2: ST2 (space=[2], 1D PE)
///   3: ST3 (space=[0,1], 2D PE, default)
///   4: ST4 (space=[0,2], 2D PE)
///   5: ST5 (space=[1,2], 2D PE)
inline ParametricSpaceTime ParametricSpaceTime::createFromMode(unsigned mode) {
  switch (mode) {
    case 0: return presets::createST0();
    case 1: return presets::createST1();
    case 2: return presets::createST2();
    case 3: return presets::createST3();
    case 4: return presets::createST4();
    case 5: return presets::createST5();
    default: return ParametricSpaceTime();  // Invalid config
  }
}

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_ANALYSIS_PARAMETRISPACETIME_H
