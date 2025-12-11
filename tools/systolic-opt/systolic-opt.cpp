//===----------------------------------------------------------------------===//
//
// systolic-opt - Systolic Array Optimizer Tool
//
// This tool applies systolic array transformations to MLIR programs.
//
//===----------------------------------------------------------------------===//

#include "systolic/Transforms/Passes.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Vector/IR/VectorOps.h"
#include "mlir/IR/Dialect.h"
#include "mlir/IR/MLIRContext.h"
#include "mlir/InitAllDialects.h"
#include "mlir/InitAllPasses.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Support/FileUtilities.h"
#include "mlir/Tools/mlir-opt/MlirOptMain.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/ToolOutputFile.h"

using namespace mlir;

int main(int argc, char **argv) {
  // Register all MLIR dialects
  DialectRegistry registry;
  registerAllDialects(registry);
  
  // Register all MLIR passes
  registerAllPasses();
  
  // Register systolic passes
  mlir::systolic::registerSystolicPasses();
  
  return mlir::asMainReturnCode(
      mlir::MlirOptMain(argc, argv, "Systolic Array Optimizer\n", registry));
}
