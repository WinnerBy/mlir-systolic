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
#include "mlir/Dialect/Affine/IR/AffineValueMap.h"
#include "mlir/Dialect/Affine/Analysis/AffineAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/AffineStructures.h"
#include "mlir/Dialect/Affine/Analysis/LoopAnalysis.h"
#include "mlir/Dialect/Affine/Analysis/Utils.h"
#include "mlir/Dialect/Affine/Utils.h"
#include "mlir/Dialect/Affine/LoopUtils.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/Operation.h"
#include "mlir/IR/Value.h"
#include "mlir/Pass/PassManager.h"
#include "mlir/Transforms/Passes.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include <climits>

// Polymer includes (conditional compilation)
#ifdef SYSTOLIC_ENABLE_POLYMER
// Try to include Polymer headers
#if __has_include("polymer/Support/IslScop.h")
#include "polymer/Support/IslScop.h"
#include "polymer/Target/ISL.h"
#include "polymer/Transforms/ExtractScopStmt.h"
#include "polymer/Support/PolymerUtils.h"
#define POLYMER_AVAILABLE 1
#elif __has_include("polymer/Support/Scop.h")
#include "polymer/Support/Scop.h"
#define POLYMER_AVAILABLE 1
#else
#define POLYMER_AVAILABLE 0
#endif

// ISL includes - Polymer provides forward declarations, but we need full declarations
#if POLYMER_AVAILABLE
  // Try to find ISL headers
  #if __has_include("isl/isl.h")
    #include "isl/isl.h"
    #include "isl/schedule.h"
    #include "isl/schedule_node.h"
  #elif __has_include("isl.h")
    #include "isl.h"
  #else
    // ISL headers not found - provide minimal forward declarations
    // Functions will be resolved at link time through Polymer libraries
    extern "C" {
      struct isl_schedule;
      struct isl_union_map;
      struct isl_schedule_node;
      void isl_schedule_free(struct isl_schedule *);
      void isl_union_map_free(struct isl_union_map *);
      void isl_schedule_node_free(struct isl_schedule_node *);
      struct isl_schedule_node *isl_schedule_get_root(struct isl_schedule *);
      int isl_schedule_node_band_n_member(struct isl_schedule_node *);
      struct isl_schedule_node *isl_schedule_node_get_child(struct isl_schedule_node *, int);
      void isl_schedule_node_free(struct isl_schedule_node *);
      typedef enum {
        isl_schedule_node_error = 0,
        isl_schedule_node_band = 1,
        isl_schedule_node_leaf = 2,
        isl_schedule_node_sequence = 3,
        isl_schedule_node_set = 4
      } isl_schedule_node_type;
      isl_schedule_node_type isl_schedule_node_get_type(struct isl_schedule_node *);
      struct isl_schedule_node *isl_schedule_node_copy(struct isl_schedule_node *);
      int isl_schedule_node_n_children(struct isl_schedule_node *);
      struct isl_multi_union_pw_aff *isl_schedule_node_band_get_partial_schedule(struct isl_schedule_node *);
      void isl_multi_union_pw_aff_free(struct isl_multi_union_pw_aff *);
      struct isl_union_map *isl_schedule_get_map(struct isl_schedule *);
      void isl_union_map_free(struct isl_union_map *);
      struct isl_union_map *isl_union_map_copy(struct isl_union_map *);
      struct isl_union_map *isl_union_map_apply_range(struct isl_union_map *, struct isl_union_map *);
      struct isl_union_map *isl_union_map_apply_domain(struct isl_union_map *, struct isl_union_map *);
      struct isl_union_map *isl_union_map_subtract(struct isl_union_map *, struct isl_union_map *);
      struct isl_schedule *isl_schedule_copy(struct isl_schedule *);
      struct isl_union_set *isl_schedule_get_domain(struct isl_schedule *);
      void isl_union_set_free(struct isl_union_set *);
      struct isl_ctx *isl_union_set_get_ctx(struct isl_union_set *);
      struct isl_space *isl_union_set_get_space(struct isl_union_set *);
      struct isl_space *isl_space_copy(struct isl_space *);
      void isl_space_free(struct isl_space *);
      struct isl_union_map *isl_union_map_empty(struct isl_space *);
    }
  #endif
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
  
  // Step 1: Convert affine.for loops to scop.stmt format if needed
  // createIslFromFuncOp requires functions with scop.stmt structure (function calls with scop.stmt callees)
  // For direct affine.for loops, we need to run ExtractScopStmt pass first
  mlir::ModuleOp module = cast<mlir::ModuleOp>(func->getParentOp());
  
  // Check if function already has scop.stmt structure
  bool hasScopStmt = false;
  func.walk([&](mlir::func::CallOp callOp) {
    if (auto callee = module.lookupSymbol<mlir::func::FuncOp>(callOp.getCallee())) {
      if (callee->hasAttr(SCOP_STMT_ATTR_NAME)) {
        hasScopStmt = true;
      }
    }
  });
  
  if (!hasScopStmt) {
    LLVM_DEBUG(llvm::dbgs() << "Function does not have scop.stmt structure, "
                            << "running ExtractScopStmt transformation...\n");
    
    // Run ExtractScopStmt transformation directly (same as ExtractScopStmtPass does)
    // This is more efficient than using PassManager for a single function
    OpBuilder builder(module.getContext());
    
    // First, replace uses by stored values (as done in ExtractScopStmtPass)
    polymer::replaceUsesByStored(func, builder);
    
    // Then extract scop statements
    unsigned numStmts = polymer::extractScopStmt(func, builder);
    
    if (numStmts == 0) {
      LLVM_DEBUG(llvm::dbgs() << "ExtractScopStmt extracted 0 statements, "
                              << "function may not have suitable affine.for loops\n");
      return nullptr;
    }
    
    LLVM_DEBUG(llvm::dbgs() << "ExtractScopStmt extracted " << numStmts 
                            << " scop statements\n");
    
    // Verify that scop.stmt structure was created
    hasScopStmt = false;
    func.walk([&](mlir::func::CallOp callOp) {
      if (auto callee = module.lookupSymbol<mlir::func::FuncOp>(callOp.getCallee())) {
        if (callee->hasAttr(SCOP_STMT_ATTR_NAME)) {
          hasScopStmt = true;
          LLVM_DEBUG(llvm::dbgs() << "Found scop.stmt function: " << callee.getName() << "\n");
        }
      }
    });
    
    if (!hasScopStmt) {
      LLVM_DEBUG(llvm::dbgs() << "ExtractScopStmt did not create scop.stmt structure, "
                              << "even though it extracted " << numStmts << " statements\n");
      return nullptr;
    }
    
    LLVM_DEBUG(llvm::dbgs() << "ExtractScopStmt successfully created scop.stmt structure\n");
  } else {
    LLVM_DEBUG(llvm::dbgs() << "Function already has scop.stmt structure\n");
  }
  
  // Step 2: Use Polymer's createIslFromFuncOp which uses IslScopBuilder internally
  // This handles all the context building, symbol table initialization, etc.
  LLVM_DEBUG(llvm::dbgs() << "Calling polymer::createIslFromFuncOp...\n");
  
  auto scop = polymer::createIslFromFuncOp(func);
  
  if (!scop) {
    LLVM_DEBUG(llvm::dbgs() << "createIslFromFuncOp returned nullptr\n");
    LLVM_DEBUG(llvm::dbgs() << "This may indicate an issue with the function structure\n");
    return nullptr;
  }
  
  LLVM_DEBUG(llvm::dbgs() << "Successfully built SCoP using Polymer's createIslFromFuncOp\n");
  return std::unique_ptr<PolymerScop>(new PolymerScop(scop.release()));
#else
  LLVM_DEBUG(llvm::dbgs() << "Polymer not available (compiled without Polymer support)\n");
  return nullptr;
#endif
}

// Manual extraction code removed - use createIslFromFuncOp instead
// For functions without scop.stmt structure, they need to be converted first

isl_schedule *PolymerScop::computeSchedule() {
#if POLYMER_AVAILABLE
  if (!isValid()) {
    LLVM_DEBUG(llvm::dbgs() << "Cannot compute schedule: invalid SCoP\n");
    return nullptr;
  }
  
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
  LLVM_DEBUG(llvm::dbgs() << "Schedule not yet built during extraction\n");
  LLVM_DEBUG(llvm::dbgs() << "Note: Schedule should be built during SCoP extraction\n");
  return nullptr;
#else
  return nullptr;
#endif
}

isl_union_map *PolymerScop::computeDependences() {
#if POLYMER_AVAILABLE
  if (!isValid()) {
    return nullptr;
  }
  
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
    
    // Get ISL context (for future use)
    // isl_ctx *ctx = isl_union_set_get_ctx(domain);
    
    // Get statement names to iterate over statements
    auto *scopStmtNames = scop->getScopStmtNames();
    
    if (!scopStmtNames || scopStmtNames->empty()) {
      LLVM_DEBUG(llvm::dbgs() << "No statements found in SCoP\n");
      isl_union_set_free(domain);
      return nullptr;
    }
    
    // Build read and write access relations from SCoP statements
    // We need to access the internal ISL structures
    // Note: Polymer's IslScop stores access relations in private islStmts
    // We'll use a workaround: access them through a friend class or
    // use ISL's ability to compute dependences from schedule
    
    // Initialize empty union maps for reads and writes
    isl_space *space = isl_union_set_get_space(domain);
    isl_union_map *reads = isl_union_map_empty(isl_space_copy(space));
    isl_union_map *writes = isl_union_map_empty(isl_space_copy(space));
    isl_space_free(space);
    
    // Try to extract access relations from the SCoP
    // Since we can't directly access private members, we'll use
    // ISL's schedule-based dependence computation
    // However, we still need the access relations
    
    // Alternative: Use ISL's isl_union_map_compute_flow with
    // access relations extracted from the schedule domain
    // For now, we'll use a simplified approach that works with
    // the information we have
    
    // Get the schedule map
    isl_union_map *scheduleMap = isl_schedule_get_map(schedule);
    
    // For dependence computation, we need:
    // 1. Read access relations (source -> array element)
    // 2. Write access relations (source -> array element)
    // 3. Domain (iteration space)
    
    // Since Polymer's IslScop doesn't expose access relations directly,
    // we'll need to use ISL's ability to compute dependences from
    // the schedule and domain alone, or find another way
    
    // For now, we'll return nullptr and note that this requires
    // either extending Polymer's API or using a different approach
    
    LLVM_DEBUG(llvm::dbgs() << "Computing dependences from " 
                            << scopStmtNames->size() << " statements\n");
    LLVM_DEBUG(llvm::dbgs() << "Note: Access relations are stored in private IslStmt structures\n");
    LLVM_DEBUG(llvm::dbgs() << "This requires either:\n");
    LLVM_DEBUG(llvm::dbgs() << "  1. Extending Polymer's API to expose access relations\n");
    LLVM_DEBUG(llvm::dbgs() << "  2. Using ISL's schedule-based dependence computation\n");
    LLVM_DEBUG(llvm::dbgs() << "  3. Reconstructing access relations from MLIR operations\n");
    
    // Cleanup
    isl_union_set_free(domain);
    isl_union_map_free(scheduleMap);
    isl_union_map_free(reads);
    isl_union_map_free(writes);
    
    // TODO: Implement proper dependence computation
    // or reconstructing access relations from the MLIR operations
    
    return nullptr;
#else
  return nullptr;
#endif
}

isl_union_map *PolymerScop::computeDependenceDistances(isl_union_map *deps) {
#if POLYMER_AVAILABLE
  if (!deps) {
    return nullptr;
  }
  
    polymer::IslScop *scop = static_cast<polymer::IslScop*>(this->scop);
    
    // Get the schedule (needed for distance computation)
    isl_schedule *schedule = scop->getSchedule();
    if (!schedule) {
      LLVM_DEBUG(llvm::dbgs() << "Cannot compute dependence distances: no schedule available\n");
      return nullptr;
    }
    
    // Get the schedule map
    isl_union_map *scheduleMap = isl_schedule_get_map(schedule);
    if (!scheduleMap) {
      LLVM_DEBUG(llvm::dbgs() << "Failed to get schedule map\n");
      return nullptr;
    }
    
    // Compute dependence distances
    // Distance = schedule(target) - schedule(source)
    // This gives us the distance vector for each dependence
    
    // Apply schedule to both domain and range of dependences
    isl_union_map *scheduledDeps = isl_union_map_apply_range(
      isl_union_map_copy(deps),
      isl_union_map_copy(scheduleMap)
    );
    
    isl_union_map *scheduledDepsDomain = isl_union_map_apply_domain(
      isl_union_map_copy(deps),
      isl_union_map_copy(scheduleMap)
    );
    
    // Compute distances: range - domain
    // This gives us the distance vectors
    isl_union_map *distances = isl_union_map_subtract(
      isl_union_map_copy(scheduledDeps),
      scheduledDepsDomain
    );
    
    // Cleanup
    isl_union_map_free(scheduledDeps);
    isl_union_map_free(scheduleMap);
    
    LLVM_DEBUG(llvm::dbgs() << "Computed dependence distances\n");
    return distances;
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

#if POLYMER_AVAILABLE
// Helper function to traverse schedule tree (forward declaration)
static void traverseScheduleTree(isl_schedule_node *node,
                                 llvm::function_ref<void(isl_schedule_node *)> callback);
#endif

LogicalResult systolic::analyzeScheduleTree(isl_schedule *schedule,
                                              ScheduleTreeInfo &info) {
#if POLYMER_AVAILABLE
  if (!schedule) {
    return failure();
  }
  
  info.schedule = schedule;
  info.dependenceDistances.clear();
  info.spaceLoopCandidates.clear();
  
    // Traverse the schedule tree to extract information
    isl_schedule_node *root = isl_schedule_get_root(schedule);
    if (!root) {
      LLVM_DEBUG(llvm::dbgs() << "Failed to get schedule tree root\n");
      return failure();
    }
    
    // Traverse the schedule tree
    // We're looking for band nodes which represent loop dimensions
    traverseScheduleTree(root, [&](isl_schedule_node *node) {
      if (isl_schedule_node_get_type(node) == isl_schedule_node_band) {
        // Extract band information
        isl_multi_union_pw_aff *mupa = isl_schedule_node_band_get_partial_schedule(node);
        int nDims = isl_schedule_node_band_n_member(node);
        
        LLVM_DEBUG(llvm::dbgs() << "Found band node with " << nDims << " dimensions\n");
        
        // For each dimension in the band, we would extract dependence distances
        // This requires the dependence distance map which should be computed separately
        // For now, we'll just record the band dimensions
        
        isl_multi_union_pw_aff_free(mupa);
      }
    });
    
    isl_schedule_node_free(root);
    
    LLVM_DEBUG(llvm::dbgs() << "Schedule tree analysis completed\n");
    return success();
#else
  return failure();
#endif
}

#if POLYMER_AVAILABLE
// Helper function to traverse schedule tree
static void traverseScheduleTree(isl_schedule_node *node,
                                 llvm::function_ref<void(isl_schedule_node *)> callback) {
  if (!node)
    return;
  
  // Call callback for current node
  callback(node);
  
  // Traverse children
  int nChildren = isl_schedule_node_n_children(node);
  for (int i = 0; i < nChildren; ++i) {
    isl_schedule_node *child = isl_schedule_node_get_child(node, i);
    traverseScheduleTree(child, callback);
    isl_schedule_node_free(child);
  }
}

// Helper function to find first band node in schedule tree
static isl_schedule_node *findFirstBandNode(isl_schedule_node *node) {
  if (!node)
    return nullptr;
  
  if (isl_schedule_node_get_type(node) == isl_schedule_node_band) {
    return isl_schedule_node_copy(node);
  }
  
  // Traverse children
  int nChildren = isl_schedule_node_n_children(node);
  for (int i = 0; i < nChildren; ++i) {
    isl_schedule_node *child = isl_schedule_node_get_child(node, i);
    isl_schedule_node *band = findFirstBandNode(child);
    isl_schedule_node_free(child);
    if (band)
      return band;
  }
  
  return nullptr;
}

// Helper function to extract distances from ISL union map
static void extractDistancesFromMap(isl_union_map *distMap,
                                    SmallVectorImpl<LoopDependenceDistance> &distances) {
  if (!distMap || distances.empty())
    return;
  
  // For each basic map in the union map, extract distance information
  // This is a simplified version - a full implementation would properly
  // parse the ISL map structure to extract distance vectors
  
  // Note: ISL union maps are complex structures. For a proper implementation,
  // we would need to:
  // 1. Iterate over each basic map in the union using isl_union_map_foreach_map()
  // 2. Extract the distance vector from each basic map
  // 3. Update min/max distances for each loop dimension
  
  // For now, this is a placeholder that indicates the structure
  // A full implementation would use ISL's map iteration API:
  // isl_union_map_foreach_map(distMap, [&](isl_map *map) {
  //   // Extract distance vector from map
  //   // Update min/max for each dimension
  // });
  
  LLVM_DEBUG(llvm::dbgs() << "Extracting distances from union map\n");
  LLVM_DEBUG(llvm::dbgs() << "Note: Full distance extraction requires ISL map iteration\n");
  LLVM_DEBUG(llvm::dbgs() << "This will be implemented using isl_union_map_foreach_map()\n");
  
  // TODO: Implement proper distance extraction using ISL API
  // This would involve:
  // - Using isl_union_map_foreach_map() to iterate over maps
  // - Extracting distance vectors from each map
  // - Computing min/max for each dimension
  // - Handling parameterized distances
}
#endif // POLYMER_AVAILABLE

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
  if (!distMap) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to compute dependence distances\n");
    isl_union_map_free(deps);
    isl_schedule_free(schedule);
    return failure();
  }
  
  // Step 5: Extract distances for each loop dimension
  // Parse the distance map to extract per-loop distances
  // The distance map is a union map from iteration space to distance vectors
  
  // Get the number of loop dimensions from the schedule
  isl_schedule_node *root = isl_schedule_get_root(schedule);
  if (!root) {
    LLVM_DEBUG(llvm::dbgs() << "Failed to get schedule tree root\n");
    isl_union_map_free(distMap);
    isl_union_map_free(deps);
    isl_schedule_free(schedule);
    return failure();
  }
  
  // Find the first band node to determine number of loop dimensions
  int nLoops = 0;
  isl_schedule_node *bandNode = findFirstBandNode(root);
  if (bandNode) {
    nLoops = isl_schedule_node_band_n_member(bandNode);
    isl_schedule_node_free(bandNode);
  }
  isl_schedule_node_free(root);
  
  if (nLoops == 0) {
    LLVM_DEBUG(llvm::dbgs() << "No loop dimensions found in schedule\n");
    isl_union_map_free(distMap);
    isl_union_map_free(deps);
    isl_schedule_free(schedule);
    return failure();
  }
  
  // Initialize distance information for each loop dimension
  for (int i = 0; i < nLoops; ++i) {
    LoopDependenceDistance dist;
    dist.loopIndex = i;
    dist.minDistance = INT64_MAX;
    dist.maxDistance = INT64_MIN;
    dist.isUniform = true;
    dist.canBeSpaceLoop = false;
    distances.push_back(dist);
  }
  
  // Extract distances from the distance map
  // For each basic map in the union map, extract the distance vector
  extractDistancesFromMap(distMap, distances);
  
  // Determine space loop candidates (distance <= 1)
  for (auto &dist : distances) {
    if (dist.maxDistance <= 1 && dist.minDistance >= -1) {
      dist.canBeSpaceLoop = true;
    }
    // Check if uniform (min == max)
    if (dist.minDistance == dist.maxDistance) {
      dist.isUniform = true;
    } else {
      dist.isUniform = false;
    }
  }
  
  // Cleanup
  isl_union_map_free(distMap);
  isl_union_map_free(deps);
  isl_schedule_free(schedule);
  
  LLVM_DEBUG(llvm::dbgs() << "Computed dependence distances for " 
                          << distances.size() << " loop dimensions\n");
  for (const auto &dist : distances) {
    LLVM_DEBUG(llvm::dbgs() << "  Loop " << dist.loopIndex 
                            << ": min=" << dist.minDistance 
                            << ", max=" << dist.maxDistance
                            << ", canBeSpace=" << dist.canBeSpaceLoop << "\n");
  }
  
  return success();
#else
  LLVM_DEBUG(llvm::dbgs() << "Polymer not available, cannot compute dependence distances\n");
  return failure();
#endif
}

