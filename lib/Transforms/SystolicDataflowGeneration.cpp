//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: SystolicDataflow Generation Pass
//
// This pass generates SystolicDataflow Dialect operations from Affine IR,
// creating multi-level IO modules, PE arrays, and double buffering structures.
//
// AutoSA Reference:
//   - sa_io_construct_optimize: Group array references, create I/O modules
//   - generate_hw_modules: Generate PE, I/O, and drain modules
//   - sa_io_module_gen: Create I/O module hierarchy (L3 -> L2 -> L1)
//
//===----------------------------------------------------------------------===//

#include "systolic/Transforms/Passes.h"
#include "systolic/Analysis/SpaceTimeAnalysis.h"
#include "systolic/Analysis/SystolicConfig.h"
#include "systolic/Dialect/SystolicDataflow/SystolicDataflow.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/LoopAnalysis.h"
#include "mlir/Dialect/Affine/LoopUtils.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/IRMapping.h"
#include "mlir/Pass/Pass.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

#include <map>
#include <string>
#include <optional>

#define DEBUG_TYPE "systolic-dataflow-generation"

using namespace mlir;
using namespace mlir::affine;
using namespace mlir::systolic::dataflow;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Array Reference Group (AutoSA: autosa_array_ref_group)
//===----------------------------------------------------------------------===//

/// Represents a group of array references that should be handled together.
/// Similar to AutoSA's autosa_array_ref_group.
struct ArrayRefGroup {
  Value memref;
  std::string arrayName;
  
  // Access operations
  SmallVector<AffineLoadOp, 8> loads;
  SmallVector<AffineStoreOp, 8> stores;
  
  // Classification
  enum GroupType { IO_GROUP, PE_GROUP, DRAIN_GROUP } type;
  
  // IO level (1-3) - only for IO_GROUP
  int ioLevel = 0;
  
  // Direction
  bool isInput;   // True for input arrays (A, B)
  bool isOutput; // True for output arrays (C)
  
  // Buffer information
  bool needsDoubleBuffer = false;
  SmallVector<int64_t, 3> bufferShape;
  
  // Tile information
  SmallVector<int64_t, 3> tileSizes;
  
  ArrayRefGroup(Value memref, StringRef name)
      : memref(memref), arrayName(name.str()), type(PE_GROUP),
        isInput(false), isOutput(false) {}
};

//===----------------------------------------------------------------------===//
// Array Reference Analysis
//===----------------------------------------------------------------------===//

/// Analyze array references in a function and group them.
/// This corresponds to AutoSA's group_array_references_io.
static LogicalResult analyzeArrayReferences(
    func::FuncOp func,
    std::vector<ArrayRefGroup> &groups) {
  
  groups.clear();
  
  // Map from memref to group
  llvm::DenseMap<Value, size_t> memrefToGroup;
  
  // Collect all load/store operations
  func.walk([&](Operation *op) {
    if (auto loadOp = dyn_cast<AffineLoadOp>(op)) {
      Value memref = loadOp.getMemRef();
      
      // Find or create group
      size_t groupIdx;
      if (auto it = memrefToGroup.find(memref); it != memrefToGroup.end()) {
        groupIdx = it->second;
      } else {
        // Create new group
        std::string name = "unknown";
        if (auto memrefType = memref.getType().dyn_cast<MemRefType>()) {
          // Try to get name from function arguments
          for (auto arg : func.getArguments()) {
            if (arg == memref) {
              if (auto attr = func.getArgAttrOfType<StringAttr>(
                      arg.getArgNumber(), "mlir.name")) {
                name = attr.getValue().str();
              } else {
                name = "arg" + std::to_string(arg.getArgNumber());
              }
              break;
            }
          }
        }
        
        groupIdx = groups.size();
        groups.emplace_back(memref, name);
        memrefToGroup[memref] = groupIdx;
      }
      
      groups[groupIdx].loads.push_back(loadOp);
    } else if (auto storeOp = dyn_cast<AffineStoreOp>(op)) {
      Value memref = storeOp.getMemRef();
      
      size_t groupIdx;
      if (auto it = memrefToGroup.find(memref); it != memrefToGroup.end()) {
        groupIdx = it->second;
      } else {
        std::string name = "unknown";
        if (auto memrefType = memref.getType().dyn_cast<MemRefType>()) {
          for (auto arg : func.getArguments()) {
            if (arg == memref) {
              if (auto attr = func.getArgAttrOfType<StringAttr>(
                      arg.getArgNumber(), "mlir.name")) {
                name = attr.getValue().str();
              } else {
                name = "arg" + std::to_string(arg.getArgNumber());
              }
              break;
            }
          }
        }
        
        groupIdx = groups.size();
        groups.emplace_back(memref, name);
        memrefToGroup[memref] = groupIdx;
      }
      
      groups[groupIdx].stores.push_back(storeOp);
    }
  });
  
  // Classify groups as IO, PE, or Drain
  // Simple heuristic: if only loads -> IO, if only stores -> Drain,
  // if both -> PE (accumulator)
  for (auto &group : groups) {
    bool hasLoads = !group.loads.empty();
    bool hasStores = !group.stores.empty();
    
    if (hasLoads && !hasStores) {
      group.type = ArrayRefGroup::IO_GROUP;
      group.isInput = true;
    } else if (hasStores && !hasLoads) {
      group.type = ArrayRefGroup::DRAIN_GROUP;
      group.isOutput = true;
    } else if (hasLoads && hasStores) {
      // Read-modify-write pattern (e.g., C += A * B)
      group.type = ArrayRefGroup::PE_GROUP;
      group.isOutput = true;
    }
    
    // Determine IO level based on loop nesting depth
    // TODO: More sophisticated analysis using Polymer/ISL
    if (group.type == ArrayRefGroup::IO_GROUP || 
        group.type == ArrayRefGroup::DRAIN_GROUP) {
      // For now, use a simple heuristic: L2 for most cases
      group.ioLevel = 2;
      if (group.type == ArrayRefGroup::IO_GROUP) {
        group.needsDoubleBuffer = true; // L2 typically needs double buffering
      }
    }
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "Array Reference Groups:\n";
    for (const auto &group : groups) {
      llvm::dbgs() << "  " << group.arrayName << ": type=";
      if (group.type == ArrayRefGroup::IO_GROUP) llvm::dbgs() << "IO";
      else if (group.type == ArrayRefGroup::PE_GROUP) llvm::dbgs() << "PE";
      else llvm::dbgs() << "DRAIN";
      llvm::dbgs() << ", level=" << group.ioLevel << "\n";
    }
  });
  
  return success();
}

//===----------------------------------------------------------------------===//
// SystolicDataflowGeneration Pass
//===----------------------------------------------------------------------===//

namespace {
struct SystolicDataflowGenerationPass
    : public PassWrapper<SystolicDataflowGenerationPass,
                        OperationPass<func::FuncOp>> {
  
  void runOnOperation() override;
  
  StringRef getArgument() const override {
    return "systolic-dataflow-generation";
  }
  
  StringRef getDescription() const override {
    return "Generate SystolicDataflow Dialect from Affine IR";
  }
};
} // namespace

void SystolicDataflowGenerationPass::runOnOperation() {
  func::FuncOp func = getOperation();
  
  // Step 1: Analyze array references
  std::vector<ArrayRefGroup> groups;
  if (failed(analyzeArrayReferences(func, groups))) {
    signalPassFailure();
    return;
  }
  
  if (groups.empty()) {
    LLVM_DEBUG(llvm::dbgs() << "No array references found, skipping\n");
    return;
  }
  
  // Step 2: Generate SystolicDataflow operations
  // For now, we'll create a simple structure
  // TODO: Implement full multi-level IO generation
  
  OpBuilder builder(func.getContext());
  builder.setInsertionPointToStart(&func.getBody().front());
  
  Location loc = func.getLoc();
  
  // Create IO modules for input arrays
  for (const auto &group : groups) {
    if (group.type == ArrayRefGroup::IO_GROUP && group.ioLevel > 0) {
      // Create IO module
      auto ioModule = builder.create<dataflow::IOModuleOp>(
          loc,
          /*level=*/builder.getI32IntegerAttr(group.ioLevel),
          /*direction=*/builder.getStringAttr(group.isInput ? "in" : "out"),
          /*arrayName=*/builder.getStringAttr(group.arrayName),
          /*bufferShape=*/group.bufferShape.empty() 
              ? ArrayAttr() 
              : builder.getI64ArrayAttr(group.bufferShape),
          /*doubleBuffer=*/group.needsDoubleBuffer
              ? builder.getBoolAttr(true)
              : BoolAttr(),
          /*name=*/StringAttr());
      
      // Create body block
      Block *bodyBlock = builder.createBlock(&ioModule.getBody());
      builder.setInsertionPointToStart(bodyBlock);
      
      // Create yield terminator
      builder.create<dataflow::IOModuleYieldOp>(loc);
      
      LLVM_DEBUG(llvm::dbgs() << "Created IO module for " << group.arrayName
                              << " at level " << group.ioLevel << "\n");
    }
  }
  
  // TODO: Generate PE array and drain modules
}

//===----------------------------------------------------------------------===//
// Pass Registration
//===----------------------------------------------------------------------===//

std::unique_ptr<Pass> createSystolicDataflowGenerationPass() {
  return std::make_unique<SystolicDataflowGenerationPass>();
}

// Register the pass
static PassRegistration<SystolicDataflowGenerationPass> passRegistration;

} // namespace systolic
} // namespace mlir

