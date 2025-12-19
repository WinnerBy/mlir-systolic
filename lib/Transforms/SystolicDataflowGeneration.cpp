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
    
    // Determine IO level based on access pattern and loop nesting
    // Heuristic: Analyze the loop nesting depth where the array is accessed
    if (group.type == ArrayRefGroup::IO_GROUP || 
        group.type == ArrayRefGroup::DRAIN_GROUP) {
      
      // Find the minimum loop nesting depth for this array's accesses
      int minDepth = 1000;  // Large number
      int maxDepth = 0;
      
      for (auto loadOp : group.loads) {
        int depth = 0;
        Operation *parent = loadOp->getParentOp();
        while (parent) {
          if (isa<AffineForOp>(parent)) {
            depth++;
          }
          parent = parent->getParentOp();
        }
        minDepth = std::min(minDepth, depth);
        maxDepth = std::max(maxDepth, depth);
      }
      
      for (auto storeOp : group.stores) {
        int depth = 0;
        Operation *parent = storeOp->getParentOp();
        while (parent) {
          if (isa<AffineForOp>(parent)) {
            depth++;
          }
          parent = parent->getParentOp();
        }
        minDepth = std::min(minDepth, depth);
        maxDepth = std::max(maxDepth, depth);
      }
      
      // Determine IO level based on access depth:
      // - L1: Accesses at innermost loops (PE interface)
      // - L2: Accesses at middle loops (double buffering)
      // - L3: Accesses at outermost loops (global memory)
      // Typical MatMul: L3 (outermost) -> L2 (middle) -> L1 (innermost) -> PE
      if (minDepth >= 4) {
        // Access at very outer loops -> L3 (global memory interface)
        group.ioLevel = 3;
        group.needsDoubleBuffer = false;  // L3 typically doesn't need double buffer
      } else if (minDepth >= 2) {
        // Access at middle loops -> L2 (double buffering)
        group.ioLevel = 2;
        group.needsDoubleBuffer = true;  // L2 typically needs double buffering
      } else {
        // Access at inner loops -> L1 (PE interface)
        group.ioLevel = 1;
        group.needsDoubleBuffer = false;  // L1 typically doesn't need double buffer
      }
      
      LLVM_DEBUG(llvm::dbgs() << "  " << group.arrayName 
                              << ": minDepth=" << minDepth 
                              << ", maxDepth=" << maxDepth
                              << ", ioLevel=" << group.ioLevel << "\n");
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
  
  // Step 2: Extract configuration from function attributes (set by SystolicTransform)
  SmallVector<int64_t, 2> peArraySize = {2, 2};  // Default
  SmallVector<int64_t, 2> tileSize = {8, 8};     // Default (latency factors)
  SmallVector<int64_t, 3> arrayPart = {16, 16, 16};  // Default
  SmallVector<int64_t, 2> latency = {8, 8};      // Default
  
  // Try to read from function attributes first (set by SystolicTransform)
  if (auto peArrayAttr = func->getAttrOfType<ArrayAttr>("systolic.pe_array_size")) {
    peArraySize.clear();
    for (auto attr : peArrayAttr) {
      if (auto intAttr = attr.dyn_cast<IntegerAttr>()) {
        peArraySize.push_back(intAttr.getInt());
      }
    }
    LLVM_DEBUG(llvm::dbgs() << "Read PE array size from attributes: ["
                            << peArraySize[0] << ", " << peArraySize[1] << "]\n");
  }
  
  if (auto latencyAttr = func->getAttrOfType<ArrayAttr>("systolic.latency")) {
    tileSize.clear();
    latency.clear();
    for (auto attr : latencyAttr) {
      if (auto intAttr = attr.dyn_cast<IntegerAttr>()) {
        int64_t val = intAttr.getInt();
        tileSize.push_back(val);
        latency.push_back(val);
      }
    }
    LLVM_DEBUG(llvm::dbgs() << "Read latency/tile size from attributes: ["
                            << tileSize[0] << ", " << tileSize[1] << "]\n");
  }
  
  if (auto arrayPartAttr = func->getAttrOfType<ArrayAttr>("systolic.array_part")) {
    arrayPart.clear();
    for (auto attr : arrayPartAttr) {
      if (auto intAttr = attr.dyn_cast<IntegerAttr>()) {
        arrayPart.push_back(intAttr.getInt());
      }
    }
    LLVM_DEBUG(llvm::dbgs() << "Read array_part from attributes: ["
                            << arrayPart[0] << ", " << arrayPart[1] << ", "
                            << arrayPart[2] << "]\n");
  }
  
  // Fallback: Try to infer from loop structure if attributes not available
  if (peArraySize[0] == 2 && peArraySize[1] == 2) {  // Still using defaults
    AffineForOp outermostLoop = nullptr;
    func.walk([&](AffineForOp forOp) {
      if (!forOp->getParentOfType<AffineForOp>()) {
        outermostLoop = forOp;
        return WalkResult::interrupt();
      }
      return WalkResult::advance();
    });
    
    if (outermostLoop) {
      SmallVector<AffineForOp, 3> loopNest;
      AffineForOp current = outermostLoop;
      while (current && loopNest.size() < 3) {
        loopNest.push_back(current);
        AffineForOp inner = nullptr;
        for (auto &op : *current.getBody()) {
          if (auto nestedFor = dyn_cast<AffineForOp>(op)) {
            inner = nestedFor;
            break;
          }
        }
        current = inner;
      }
      
      if (loopNest.size() >= 2) {
        if (loopNest[0].hasConstantUpperBound() && loopNest[1].hasConstantUpperBound()) {
          int64_t bound0 = loopNest[0].getConstantUpperBound();
          int64_t bound1 = loopNest[1].getConstantUpperBound();
          peArraySize[0] = std::max((int64_t)1, bound0 / 8);
          peArraySize[1] = std::max((int64_t)1, bound1 / 8);
          LLVM_DEBUG(llvm::dbgs() << "Inferred PE array size from loop bounds: ["
                                  << peArraySize[0] << ", " << peArraySize[1] << "]\n");
        }
      }
    }
  }
  
  OpBuilder builder(func.getContext());
  builder.setInsertionPointToStart(&func.getBody().front());
  
  Location loc = func.getLoc();
  
  // Step 3: Generate IO modules for input arrays (L1, L2, L3)
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
  
  // Step 4: Generate PE Array
  // Find PE group (accumulator arrays with both loads and stores)
  bool hasPEGroup = false;
  for (const auto &group : groups) {
    if (group.type == ArrayRefGroup::PE_GROUP) {
      hasPEGroup = true;
      break;
    }
  }
  
  if (hasPEGroup) {
    auto peArray = builder.create<dataflow::PEArrayOp>(
        loc,
        /*arraySize=*/builder.getI64ArrayAttr(peArraySize),
        /*tileSize=*/builder.getI64ArrayAttr(tileSize),
        /*name=*/StringAttr());
    
    // Create body block
    Block *bodyBlock = builder.createBlock(&peArray.getBody());
    builder.setInsertionPointToStart(bodyBlock);
    
    // Create yield terminator
    builder.create<dataflow::PEArrayYieldOp>(loc);
    
    LLVM_DEBUG(llvm::dbgs() << "Created PE array with size [" 
                            << peArraySize[0] << ", " << peArraySize[1] << "]\n");
  }
  
  // Step 5: Generate Drain modules for output arrays
  for (const auto &group : groups) {
    if (group.type == ArrayRefGroup::DRAIN_GROUP) {
      // Determine drain level (typically L2 for output)
      int drainLevel = group.ioLevel > 0 ? group.ioLevel : 2;
      
      auto drainModule = builder.create<dataflow::DrainModuleOp>(
          loc,
          /*level=*/builder.getI32IntegerAttr(drainLevel),
          /*arrayName=*/builder.getStringAttr(group.arrayName),
          /*bufferShape=*/group.bufferShape.empty()
              ? ArrayAttr()
              : builder.getI64ArrayAttr(group.bufferShape),
          /*name=*/StringAttr());
      
      // Create body block
      Block *bodyBlock = builder.createBlock(&drainModule.getBody());
      builder.setInsertionPointToStart(bodyBlock);
      
      // Create yield terminator
      builder.create<dataflow::DrainModuleYieldOp>(loc);
      
      LLVM_DEBUG(llvm::dbgs() << "Created drain module for " << group.arrayName
                              << " at level " << drainLevel << "\n");
    }
  }
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

