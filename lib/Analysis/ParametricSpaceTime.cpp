//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Parametric Space-Time Configuration Implementation
//
//===----------------------------------------------------------------------===//

#include "systolic/Analysis/ParametricSpaceTime.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/StringRef.h"
#include <sstream>

#define DEBUG_TYPE "systolic-parametric-spacetime"

using namespace mlir;
using namespace llvm;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// ParametricSpaceTime Implementation
//===----------------------------------------------------------------------===//

ParametricSpaceTime::ParametricSpaceTime() {
  timeDimConfig.pipelineDepth = 1;
  timeDimConfig.latencyHidingLength = -1;  // Auto
  timeDimConfig.doubleBuffer = true;
}

bool ParametricSpaceTime::isFlowConfigurationValid() const {
  // Check for conflicting flows (simplistic check)
  // In a more complex system, we might validate against actual hardware constraints
  return true;
}

bool ParametricSpaceTime::isValid() const {
  // A valid configuration must have at least one space dimension
  // and at least one time dimension
  if (spaceDimConfigs.empty()) {
    LLVM_DEBUG(dbgs() << "Invalid: No space dimensions\n");
    return false;
  }
  if (timeDimConfig.loopDims.empty()) {
    LLVM_DEBUG(dbgs() << "Invalid: No time dimensions\n");
    return false;
  }
  
  // PE array size should be reasonable
  for (const auto &spaceDim : spaceDimConfigs) {
    if (spaceDim.peArraySize <= 0 || spaceDim.parallelism <= 0) {
      LLVM_DEBUG(dbgs() << "Invalid: Non-positive array size or parallelism\n");
      return false;
    }
  }
  
  return isFlowConfigurationValid();
}

std::string ParametricSpaceTime::getSpaceTimeTypeString() const {
  // Determine ST type based on space loop indices
  unsigned numSpaceDims = getNumSpaceDims();
  
  if (numSpaceDims == 1) {
    // ST0, ST1, or ST2 depending on which loop is space
    unsigned spaceDim = spaceDimConfigs[0].loopDim;
    switch (spaceDim) {
      case 0: return "ST0";
      case 1: return "ST1";
      case 2: return "ST2";
      default: return "ST1D";  // Generic 1D
    }
  } else if (numSpaceDims == 2) {
    // ST3, ST4, or ST5 depending on which loops are space
    unsigned dim0 = spaceDimConfigs[0].loopDim;
    unsigned dim1 = spaceDimConfigs[1].loopDim;
    
    // Normalize to (min, max) for comparison
    if (dim0 > dim1) std::swap(dim0, dim1);
    
    if (dim0 == 0 && dim1 == 1) return "ST3";  // [0,1]
    if (dim0 == 0 && dim1 == 2) return "ST4";  // [0,2]
    if (dim0 == 1 && dim1 == 2) return "ST5";  // [1,2]
    return "ST2D";  // Generic 2D
  }
  
  return "STUnknown";
}

std::string ParametricSpaceTime::toString() const {
  std::stringstream ss;
  ss << "SpaceTime(" << getSpaceTimeTypeString() << ")\n";
  
  ss << "  Space Dims: ";
  for (unsigned i = 0; i < spaceDimConfigs.size(); ++i) {
    const auto &cfg = spaceDimConfigs[i];
    ss << "[" << cfg.loopName << "(dim=" << cfg.loopDim << ",size=" 
       << cfg.peArraySize << ",par=" << cfg.parallelism << ")]";
    if (i < spaceDimConfigs.size() - 1) ss << " ";
  }
  ss << "\n";
  
  ss << "  Time Dims: ";
  for (unsigned i = 0; i < timeDimConfig.loopDims.size(); ++i) {
    ss << "[" << timeDimConfig.loopNames[i] << "(dim=" 
       << timeDimConfig.loopDims[i] << ",depth=" << timeDimConfig.pipelineDepth 
       << ")]";
    if (i < timeDimConfig.loopDims.size() - 1) ss << " ";
  }
  ss << "\n";
  
  if (hasReductionDim()) {
    ss << "  Reduction Dim: [" << reductionDimConfig.loopName 
       << "(dim=" << reductionDimConfig.loopDim << ",op=" 
       << reductionDimConfig.reductionOp << ")]\n";
  }
  
  ss << "  Data Flows: " << operandFlows.size() << " operands\n";
  
  return ss.str();
}

void ParametricSpaceTime::dump() const {
  print(llvm::errs());
}

void ParametricSpaceTime::print(llvm::raw_ostream &os) const {
  os << toString();
}

//===----------------------------------------------------------------------===//
// Predefined Configurations (ST0–ST5)
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Factory Methods
//===----------------------------------------------------------------------===//

ParametricSpaceTime ParametricSpaceTime::createFromLoopIndices(
    const SmallVector<unsigned> &spaceLoopIndices,
    const SmallVector<unsigned> &timeLoopIndices,
    const SmallVector<StringRef> &loopNames) {
  
  ParametricSpaceTime config;
  
  // Add space dimensions
  for (unsigned idx : spaceLoopIndices) {
    std::string loopName = "loop";
    if (idx < loopNames.size()) {
      loopName = loopNames[idx].str();
    } else {
      loopName += std::to_string(idx);
    }
    config.addSpaceDim(idx, loopName, 16, 16);
  }
  
  // Add time dimensions
  for (unsigned idx : timeLoopIndices) {
    std::string loopName = "loop";
    if (idx < loopNames.size()) {
      loopName = loopNames[idx].str();
    } else {
      loopName += std::to_string(idx);
    }
    config.addTimeDim(idx, loopName, 1);
  }
  
  return config;
}

namespace presets {

ParametricSpaceTime createST0() {
  // ST0: Space=[0] (loop i), Time=[1,2] (loops j,k)
  // 1D PE array along dimension i (row)
  ParametricSpaceTime st;
  st.addSpaceDim(0, "i", 16, 16);     // Space dimension at loop 0
  st.addTimeDim(1, "j", 1);           // Time dimension at loop 1
  st.addTimeDim(2, "k", 1);           // Time dimension at loop 2
  return st;
}

ParametricSpaceTime createST1() {
  // ST1: Space=[1] (loop j), Time=[0,2] (loops i,k)
  // 1D PE array along dimension j (column)
  ParametricSpaceTime st;
  st.addSpaceDim(1, "j", 16, 16);     // Space dimension at loop 1
  st.addTimeDim(0, "i", 1);           // Time dimension at loop 0
  st.addTimeDim(2, "k", 1);           // Time dimension at loop 2
  return st;
}

ParametricSpaceTime createST2() {
  // ST2: Space=[2] (loop k), Time=[0,1] (loops i,j)
  // 1D PE array along dimension k (depth, rare)
  ParametricSpaceTime st;
  st.addSpaceDim(2, "k", 16, 16);     // Space dimension at loop 2
  st.addTimeDim(0, "i", 1);           // Time dimension at loop 0
  st.addTimeDim(1, "j", 1);           // Time dimension at loop 1
  return st;
}

ParametricSpaceTime createST3() {
  // ST3: Space=[0,1] (loops i,j), Time=[2] (loop k)
  // 2D PE array with output-stationary pattern
  // Typical: MatMul with A[i,k], B[k,j], C[i,j]
  //   - A flows HORIZONTAL along i
  //   - B flows VERTICAL along j
  //   - C stays local
  ParametricSpaceTime st;
  st.addSpaceDim(0, "i", 16, 16);     // PE rows
  st.addSpaceDim(1, "j", 16, 16);     // PE columns
  st.addTimeDim(2, "k", 8);           // Reduction over k
  st.getTimeDimConfig().pipelineDepth = 8;
  st.getTimeDimConfig().latencyHidingLength = 4;
  st.getTimeDimConfig().doubleBuffer = true;
  return st;
}

ParametricSpaceTime createST4() {
  // ST4: Space=[0,2] (loops i,k), Time=[1] (loop j)
  // 2D PE array with weight-stationary pattern
  ParametricSpaceTime st;
  st.addSpaceDim(0, "i", 16, 16);     // PE rows
  st.addSpaceDim(2, "k", 16, 16);     // PE depth
  st.addTimeDim(1, "j", 8);           // Time along j
  st.getTimeDimConfig().pipelineDepth = 8;
  st.getTimeDimConfig().latencyHidingLength = 4;
  st.getTimeDimConfig().doubleBuffer = true;
  return st;
}

ParametricSpaceTime createST5() {
  // ST5: Space=[1,2] (loops j,k), Time=[0] (loop i)
  // 2D PE array with activation-stationary pattern
  ParametricSpaceTime st;
  st.addSpaceDim(1, "j", 16, 16);     // PE columns
  st.addSpaceDim(2, "k", 16, 16);     // PE depth
  st.addTimeDim(0, "i", 8);           // Time along i
  st.getTimeDimConfig().pipelineDepth = 8;
  st.getTimeDimConfig().latencyHidingLength = 4;
  st.getTimeDimConfig().doubleBuffer = true;
  return st;
}

} // namespace presets

} // namespace systolic
} // namespace mlir
