//===- EmitHLSCpp.h - HLS C++ Emission Interface ----------------*- C++ -*-===//
//
// Part of the mlir-systolic project.
//
//===----------------------------------------------------------------------===//
//
// This file declares the interface for emitting HLS C++ from MLIR.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_TRANSLATION_EMITHLSCPP_H
#define SYSTOLIC_TRANSLATION_EMITHLSCPP_H

#include "mlir/IR/BuiltinOps.h"
#include "mlir/Support/LogicalResult.h"
#include "llvm/Support/raw_ostream.h"

namespace mlir {
namespace systolic {

/// Configuration options for HLS C++ emission
struct HLSEmitOptions {
  /// Target platform (xilinx, intel)
  std::string targetPlatform = "xilinx";
  
  /// Enable dataflow pragma generation
  bool enableDataflow = true;
  
  /// Enable pipeline pragma generation
  bool enablePipeline = true;
  
  /// Enable array partitioning pragmas
  bool enableArrayPartition = true;
  
  /// Default initiation interval for pipeline
  int defaultII = 1;
  
  /// Generate double buffer for data reuse
  bool enableDoubleBuffer = true;
  
  /// Top function name
  std::string topFunctionName = "kernel";
};

/// Emit HLS C++ code for the given module
LogicalResult emitHLSCpp(ModuleOp module, llvm::raw_ostream &os,
                         const HLSEmitOptions &options = HLSEmitOptions());

/// Register HLS C++ translation
void registerHLSCppTranslation();

} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_TRANSLATION_EMITHLSCPP_H
