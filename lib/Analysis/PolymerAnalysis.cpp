//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Polymer Polyhedral Analysis Implementation
//
// This module implements the interface to Polymer for polyhedral analysis.
// It provides SCoP extraction, schedule tree computation, and dependence
// analysis using ISL.
//
//===----------------------------------------------------------------------===//

#include "systolic/Analysis/PolymerAnalysis.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/AffineAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/AffineStructures.h"
#include "mlir/Dialect/Affine/Analysis/LoopAnalysis.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Value.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"

// Polymer includes (conditional compilation)
#ifdef SYSTOLIC_ENABLE_POLYMER
// Try to include Polymer headers
#if __has_include("polymer/Support/IslScop.h")
#include "polymer/Support/IslScop.h"
#define POLYMER_AVAILABLE 1
#elif __has_include("polymer/Support/Scop.h")
#include "polymer/Support/Scop.h"
#define POLYMER_AVAILABLE 1
#else
#define POLYMER_AVAILABLE 0
#endif

// ISL includes (usually provided by Polymer)
#if __has_include("isl/isl.h")
#include "isl/isl.h"
#elif __has_include("isl.h")
#include "isl.h"
#endif
#else
#define POLYMER_AVAILABLE 0
#endif

#define DEBUG_TYPE "polymer-analysis"

using namespace mlir;
using namespace mlir::systolic;

//===----------------------------------------------------------------------===//
// Polymer Availability Check
//===----------------------------------------------------------------------===//

bool systolic::isPolymerAvailable() {
  return POLYMER_AVAILABLE != 0;
}

//===----------------------------------------------------------------------===//
// PolymerScop Implementation
//===----------------------------------------------------------------------===//

std::unique_ptr<PolymerScop> PolymerScop::extract(func::FuncOp func) {
#if POLYMER_AVAILABLE
  LLVM_DEBUG(llvm::dbgs() << "Extracting SCoP with Polymer for function: "
                          << func.getName() << "\n");
  
  try {
    // Create IslScop object
    polymer::IslScop *scop = new polymer::IslScop();
    
    // Initialize symbol table from function
    scop->initializeSymbolTable(func, nullptr);
    
    // Step 1: Find all affine.for loops in the function
    SmallVector<affine::AffineForOp, 8> loops;
    func.walk([&](affine::AffineForOp forOp) {
      loops.push_back(forOp);
    });
    
    if (loops.empty()) {
      LLVM_DEBUG(llvm::dbgs() << "No affine.for loops found in function\n");
      delete scop;
      return nullptr;
    }
    
    // Step 2: Extract the innermost loop nest (SCoP region)
    // For now, we assume a perfect loop nest starting from the outermost loop
    affine::AffineForOp outerLoop = loops[0];
    
    // Step 3: Build domain and access relations for each statement
    // A statement is typically an operation inside the innermost loop
    int stmtId = 0;
    
    // Walk operations in the innermost loop body to find statements
    affine::AffineForOp innermostLoop = loops.back();
    SmallVector<Operation *, 8> statements;
    
    innermostLoop.getBody()->walk([&](Operation *op) {
      // Skip affine.for operations (they are loops, not statements)
      if (isa<affine::AffineForOp>(op))
        return;
      
      // Skip terminator operations
      if (op->hasTrait<OpTrait::IsTerminator>())
        return;
      
      // Collect load/store operations as statements
      if (isa<affine::AffineLoadOp>(op) || isa<affine::AffineStoreOp>(op) ||
          isa<arith::ArithFastMathInterface>(op)) {
        statements.push_back(op);
      }
    });
    
    // For each statement, create a SCoP statement
    // Note: The actual domain and access relation extraction requires
    // more detailed analysis. For now, we create statements and let
    // buildSchedule handle the basic structure.
    for (Operation *stmt : statements) {
      scop->createStatement();
      stmtId++;
    }
    
    LLVM_DEBUG(llvm::dbgs() << "Created " << stmtId << " SCoP statements\n");
    
    // Step 4: Build schedule from the loop structure
    // Polymer's buildSchedule can automatically build a schedule from operations
    // We need to collect all operations in the loop nest in execution order
    SmallVector<Operation *, 8> scheduleOps;
    
    // Collect operations in execution order (loops and statements)
    outerLoop.getBody()->walk([&](Operation *op) {
      // Include loops and memory operations in the schedule
      if (isa<affine::AffineForOp>(op) || isa<affine::AffineLoadOp>(op) || 
          isa<affine::AffineStoreOp>(op)) {
        scheduleOps.push_back(op);
      }
    });
    
    if (!scheduleOps.empty()) {
      scop->buildSchedule(scheduleOps);
      LLVM_DEBUG(llvm::dbgs() << "Built schedule with " << scheduleOps.size() 
                              << " operations\n");
    } else {
      LLVM_DEBUG(llvm::dbgs() << "Warning: No operations found for schedule building\n");
    }
    
    LLVM_DEBUG(llvm::dbgs() << "SCoP extraction completed with " << stmtId 
                            << " statements\n");
    return std::unique_ptr<PolymerScop>(new PolymerScop(scop));
  } catch (const std::exception &e) {
    LLVM_DEBUG(llvm::dbgs() << "Exception during SCoP extraction: " 
                            << e.what() << "\n");
    return nullptr;
  } catch (...) {
    LLVM_DEBUG(llvm::dbgs() << "Unknown exception during SCoP extraction\n");
    return nullptr;
  }
#else
  LLVM_DEBUG(llvm::dbgs() << "Polymer not available (compiled without Polymer support)\n");
  return nullptr;
#endif
}

isl_schedule *PolymerScop::computeSchedule() {
#if POLYMER_AVAILABLE
  if (!isValid()) {
    LLVM_DEBUG(llvm::dbgs() << "Cannot compute schedule: invalid SCoP\n");
    return nullptr;
  }
  
  try {
    polymer::IslScop *scop = static_cast<polymer::IslScop*>(this->scop);
    
    // Get existing schedule if available (built during extraction)
    isl_schedule *schedule = scop->getSchedule();
    if (schedule) {
      // Take ownership (ISL convention: caller takes ownership)
      LLVM_DEBUG(llvm::dbgs() << "Using existing schedule from SCoP\n");
      return isl_schedule_copy(schedule);
    }
    
    // If schedule is not built, we need to compute it using ISL
    // This requires:
    // 1. Get the domain (union of all statement domains)
    // 2. Get the dependences
    // 3. Call isl_schedule_compute_schedule() or similar
    
    // For now, if schedule is not available, return nullptr
    // The schedule should have been built during SCoP extraction
    LLVM_DEBUG(llvm::dbgs() << "Schedule not yet built during extraction\n");
    LLVM_DEBUG(llvm::dbgs() << "Note: Schedule should be built during SCoP extraction\n");
    return nullptr;
  } catch (const std::exception &e) {
    LLVM_DEBUG(llvm::dbgs() << "Exception during schedule computation: " 
                            << e.what() << "\n");
    return nullptr;
  } catch (...) {
    LLVM_DEBUG(llvm::dbgs() << "Unknown exception during schedule computation\n");
    return nullptr;
  }
#else
  return nullptr;
#endif
}

isl_union_map *PolymerScop::computeDependences() {
#if POLYMER_AVAILABLE
  if (!isValid()) {
    return nullptr;
  }
  
  try {
    polymer::IslScop *scop = static_cast<polymer::IslScop*>(this->scop);
    
    // Get the schedule (needed for dependence computation)
    isl_schedule *schedule = scop->getSchedule();
    if (!schedule) {
      LLVM_DEBUG(llvm::dbgs() << "Cannot compute dependences: no schedule available\n");
      return nullptr;
    }
    
    // Get the domain from the schedule
    isl_union_set *domain = isl_schedule_get_domain(schedule);
    if (!domain) {
      LLVM_DEBUG(llvm::dbgs() << "Failed to get domain from schedule\n");
      return nullptr;
    }
    
    // Get read and write access relations from the SCoP
    // For now, we need to build these from the SCoP statements
    // This is a simplified version - full implementation would extract
    // all read/write relations from the SCoP
    
    // Compute flow dependences (RAW - Read After Write)
    // and output dependences (WAW - Write After Write)
    // Using ISL's dependence computation
    
    // Build access relations (simplified - would need to extract from SCoP)
    // For a proper implementation, we would:
    // 1. Extract all read relations from SCoP statements
    // 2. Extract all write relations from SCoP statements
    // 3. Use isl_union_map_compute_flow() to compute dependences
    
    // For now, return nullptr to indicate this needs proper implementation
    // The full implementation requires extracting access relations from
    // the SCoP's statement map
    LLVM_DEBUG(llvm::dbgs() << "Dependence computation requires access relation extraction\n");
    LLVM_DEBUG(llvm::dbgs() << "This will be implemented after SCoP extraction is complete\n");
    
    isl_union_set_free(domain);
    return nullptr;
  } catch (const std::exception &e) {
    LLVM_DEBUG(llvm::dbgs() << "Exception during dependence computation: " 
                            << e.what() << "\n");
    return nullptr;
  } catch (...) {
    LLVM_DEBUG(llvm::dbgs() << "Unknown exception during dependence computation\n");
    return nullptr;
  }
#else
  return nullptr;
#endif
}

isl_union_map *PolymerScop::computeDependenceDistances(isl_union_map *deps) {
#if POLYMER_AVAILABLE
  if (!deps) {
    return nullptr;
  }
  
  // TODO: Implement dependence distance computation
  // This requires:
  // 1. Extract distance vectors from dependence map
  // 2. Compute min/max distances for each loop dimension
  // 3. Return distance map
  
  return nullptr;
#else
  return nullptr;
#endif
}

PolymerScop::~PolymerScop() {
#if POLYMER_AVAILABLE
  if (scop) {
    polymer::IslScop *scopPtr = static_cast<polymer::IslScop*>(scop);
    delete scopPtr;
    scop = nullptr;
  }
#endif
}

//===----------------------------------------------------------------------===//
// Schedule Tree Analysis
//===----------------------------------------------------------------------===//

LogicalResult systolic::analyzeScheduleTree(isl_schedule *schedule,
                                              ScheduleTreeInfo &info) {
#if POLYMER_AVAILABLE
  if (!schedule) {
    return failure();
  }
  
  info.schedule = schedule;
  
  // TODO: Implement schedule tree analysis
  // This requires:
  // 1. Traverse the schedule tree
  // 2. Extract dependence distances from schedule nodes
  // 3. Identify space loop candidates (distance <= 1)
  
  LLVM_DEBUG(llvm::dbgs() << "Schedule tree analysis not yet implemented\n");
  return success();
#else
  return failure();
#endif
}

void ScheduleTreeInfo::release() {
#if POLYMER_AVAILABLE
  if (schedule) {
    isl_schedule_free(schedule);
    schedule = nullptr;
  }
#endif
}

//===----------------------------------------------------------------------===//
// Dependence Distance Analysis
//===----------------------------------------------------------------------===//

LogicalResult systolic::computeDependenceDistancesWithPolymer(
    func::FuncOp func,
    SmallVectorImpl<LoopDependenceDistance> &distances) {
  
  distances.clear();
  
#if POLYMER_AVAILABLE
  // Step 1: Extract SCoP
  auto scop = PolymerScop::extract(func);
  if (!scop || !scop->isValid()) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to extract SCoP with Polymer\n");
    return failure();
  }
  
  // Step 2: Compute schedule tree
  isl_schedule *schedule = scop->computeSchedule();
  if (!schedule) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to compute schedule tree\n");
    return failure();
  }
  
  // Step 3: Compute dependences
  isl_union_map *deps = scop->computeDependences();
  if (!deps) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to compute dependences\n");
    isl_schedule_free(schedule);
    return failure();
  }
  
  // Step 4: Compute dependence distances
  isl_union_map *distMap = scop->computeDependenceDistances(deps);
  
  // Step 5: Extract distances for each loop dimension
  // TODO: Parse the distance map to extract per-loop distances
  
  // Cleanup
  if (distMap) {
    isl_union_map_free(distMap);
  }
  isl_union_map_free(deps);
  isl_schedule_free(schedule);
  
  LLVM_DEBUG(llvm::dbgs() << "Polymer dependence distance computation not yet fully implemented\n");
  return success();
#else
  LLVM_DEBUG(llvm::dbgs() << "Polymer not available, cannot compute dependence distances\n");
  return failure();
#endif
}

