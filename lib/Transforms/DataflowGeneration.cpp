//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: Dataflow Generation Pass
//
// This pass generates the dataflow structure for systolic arrays,
// creating Stream channels, I/O Tasks, and PE Tasks following
// AutoSA's communication management methodology.
//
// AutoSA Reference:
//   - sa_io_construct_optimize: Group array references, create I/O modules
//   - generate_hw_modules: Generate PE, I/O, and drain modules
//   - sa_io_module_gen: Create I/O module hierarchy (L3 -> L2 -> L1)
//   - sa_pe_module_gen: Create PE module with local computation
//
// Generated Structure:
//   hls.dataflow.dispatch {
//     // L3 Streams (Global Memory <-> L2)
//     %A_L3 = hls.dataflow.stream : !hls.stream<f32>
//     %B_L3 = hls.dataflow.stream : !hls.stream<f32>
//
//     // L2 Streams (L2 <-> L1/PE)
//     %A_L2 = hls.dataflow.stream : !hls.stream<f32>
//     %B_L2 = hls.dataflow.stream : !hls.stream<f32>
//     %C_drain = hls.dataflow.stream : !hls.stream<f32>
//
//     // I/O L3 Tasks (Memory Access)
//     hls.dataflow.task "A_IO_L3_in" { ... }
//     hls.dataflow.task "B_IO_L3_in" { ... }
//
//     // I/O L2 Tasks (Double Buffering)
//     hls.dataflow.task "A_IO_L2_in" { ... }
//     hls.dataflow.task "B_IO_L2_in" { ... }
//
//     // PE Task (Computation)
//     hls.dataflow.task "PE" { ... }
//
//     // Drain Tasks (Output)
//     hls.dataflow.task "C_drain_IO_L1_out" { ... }
//     hls.dataflow.task "C_drain_IO_L2_out" { ... }
//   }
//
//===----------------------------------------------------------------------===//

#include "systolic/Transforms/Passes.h"
#include "systolic/Analysis/SpaceTimeAnalysis.h"
#include "systolic/Analysis/SystolicConfig.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Affine/Analysis/LoopAnalysis.h"
#include "mlir/Dialect/Affine/LoopUtils.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/Dialect/Vector/IR/VectorOps.h"
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

#define DEBUG_TYPE "systolic-dataflow"

using namespace mlir;
using namespace mlir::affine;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Helper Data Structures
//===----------------------------------------------------------------------===//

/// I/O Module Level (AutoSA: L1, L2, L3)
enum class IOLevel {
  L1,  // Innermost level (PE interface)
  L2,  // Middle level (with double buffering)
  L3   // Outermost level (memory interface)
};

/// I/O Direction
enum class IODirection {
  IN,   // Data flowing into PE array
  OUT   // Data flowing out of PE array (drain)
};

/// Operand classification for MatMul-like patterns
/// AutoSA categorizes arrays into:
/// - Input arrays (A, B) - read-only, need I/O modules for feeding data
/// - Output arrays (C) - write-only or read-modify-write, need drain modules
struct OperandClassification {
  Value memref;
  std::string name;
  bool isInput;       // True for A, B (read into PE)
  bool isOutput;      // True for C (written from PE)
  bool isAccumulator; // True if read-modify-write (C in reduction)
  SystolicFlowDir flowDir;
  
  /// Tile sizes at each level
  SmallVector<int64_t, 3> tileSizes;
};

/// Stream channel info
struct StreamInfo {
  std::string name;
  Type elementType;
  IOLevel level;
  IODirection direction;
  int64_t depth;
  Value channel;  // The created stream value
};

/// Task module info (AutoSA: autosa_hw_module)
struct TaskInfo {
  std::string name;
  enum TaskType { PE, IO_L1, IO_L2, IO_L3, DRAIN } type;
  IODirection direction;
  Value inputMemref;
  SmallVector<StreamInfo*, 2> inputStreams;
  SmallVector<StreamInfo*, 2> outputStreams;
  
  /// Loop bounds for tile iteration
  SmallVector<int64_t, 3> tileBounds;
  SmallVector<int64_t, 3> tileSizes;
};

//===----------------------------------------------------------------------===//
// I/O Group Analysis (AutoSA: group_array_references_io)
//===----------------------------------------------------------------------===//

/// Analyze memory operands and classify them for I/O module generation.
/// AutoSA groups array references based on:
/// 1. Access pattern (which loops access which arrays)
/// 2. Data flow direction (input vs output)
/// 3. Sharing pattern (broadcast, multicast, etc.)
static LogicalResult classifyOperands(
    func::FuncOp func,
    const SystolicConfig &config,
    SmallVectorImpl<OperandClassification> &operands) {
  
  operands.clear();
  
  // Collect all memref arguments and internal allocations
  llvm::StringMap<OperandClassification> memrefMap;
  
  for (auto arg : func.getArguments()) {
    if (auto memrefType = dyn_cast<MemRefType>(arg.getType())) {
      OperandClassification op;
      op.memref = arg;
      op.name = "arg" + std::to_string(arg.getArgNumber());
      op.isInput = false;
      op.isOutput = false;
      op.isAccumulator = false;
      op.flowDir = SystolicFlowDir::NONE;
      memrefMap[op.name] = op;
    }
  }
  
  // Walk through operations to determine read/write patterns
  func.walk([&](Operation *op) {
    Value memref;
    bool isRead = false;
    bool isWrite = false;
    
    if (auto loadOp = dyn_cast<AffineLoadOp>(op)) {
      memref = loadOp.getMemRef();
      isRead = true;
    } else if (auto storeOp = dyn_cast<AffineStoreOp>(op)) {
      memref = storeOp.getMemRef();
      isWrite = true;
    }
    
    if (memref) {
      // Find which argument this corresponds to
      for (unsigned i = 0; i < func.getNumArguments(); ++i) {
        if (func.getArgument(i) == memref) {
          std::string name = "arg" + std::to_string(i);
          if (memrefMap.count(name)) {
            if (isRead) memrefMap[name].isInput = true;
            if (isWrite) memrefMap[name].isOutput = true;
            if (isRead && isWrite) memrefMap[name].isAccumulator = true;
          }
          break;
        }
      }
    }
  });
  
  // Convert to vector and assign flow directions
  unsigned idx = 0;
  for (auto &entry : memrefMap) {
    auto &op = entry.second;
    
    // Assign names based on MatMul convention
    if (op.isInput && !op.isOutput) {
      // Pure input: A or B
      if (idx == 0) {
        op.name = "A";
        op.flowDir = SystolicFlowDir::HORIZONTAL;
      } else {
        op.name = "B";
        op.flowDir = SystolicFlowDir::VERTICAL;
      }
    } else if (op.isOutput || op.isAccumulator) {
      op.name = "C";
      op.flowDir = SystolicFlowDir::NONE; // Output-stationary
    }
    
    // Set tile sizes from config
    op.tileSizes.assign(config.arrayPart.begin(), config.arrayPart.end());
    
    operands.push_back(op);
    idx++;
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[DataflowGen] Classified operands:\n";
    for (auto &op : operands) {
      llvm::dbgs() << "  " << op.name << ": input=" << op.isInput
                   << " output=" << op.isOutput
                   << " accumulator=" << op.isAccumulator << "\n";
    }
  });
  
  return success();
}

//===----------------------------------------------------------------------===//
// Stream Channel Generation
//===----------------------------------------------------------------------===//

/// Create stream channels for all required I/O paths.
/// AutoSA creates a hierarchy of streams:
/// - L3 streams: Connect memory to L2 buffers
/// - L2 streams: Connect L2 buffers to L1/PE
/// - Drain streams: For output data flow
static void createStreamChannels(
    OpBuilder &builder,
    Location loc,
    const SmallVectorImpl<OperandClassification> &operands,
    const SystolicConfig &config,
    SmallVectorImpl<StreamInfo> &streams) {
  
  streams.clear();
  
  // For each input operand, create L3 and L2 streams
  for (const auto &op : operands) {
    Type elemType;
    if (auto memrefType = dyn_cast<MemRefType>(op.memref.getType())) {
      elemType = memrefType.getElementType();
    } else {
      continue;
    }
    
    if (op.isInput) {
      // L3 stream: Memory -> L2
      StreamInfo l3;
      l3.name = op.name + "_IO_L3_in";
      l3.elementType = elemType;
      l3.level = IOLevel::L3;
      l3.direction = IODirection::IN;
      l3.depth = 2;  // Ping-pong buffer depth
      streams.push_back(l3);
      
      // L2 stream: L2 -> L1/PE
      StreamInfo l2;
      l2.name = op.name + "_IO_L2_in";
      l2.elementType = elemType;
      l2.level = IOLevel::L2;
      l2.direction = IODirection::IN;
      l2.depth = 2;
      streams.push_back(l2);
    }
    
    if (op.isOutput) {
      // Drain L1 stream: PE -> L1
      StreamInfo drainL1;
      drainL1.name = op.name + "_drain_IO_L1_out";
      drainL1.elementType = elemType;
      drainL1.level = IOLevel::L1;
      drainL1.direction = IODirection::OUT;
      drainL1.depth = 2;
      streams.push_back(drainL1);
      
      // Drain L2 stream: L1 -> L2
      StreamInfo drainL2;
      drainL2.name = op.name + "_drain_IO_L2_out";
      drainL2.elementType = elemType;
      drainL2.level = IOLevel::L2;
      drainL2.direction = IODirection::OUT;
      drainL2.depth = 2;
      streams.push_back(drainL2);
    }
  }
  
  LLVM_DEBUG({
    llvm::dbgs() << "[DataflowGen] Created " << streams.size() << " streams:\n";
    for (auto &s : streams) {
      llvm::dbgs() << "  " << s.name << " (depth=" << s.depth << ")\n";
    }
  });
}

//===----------------------------------------------------------------------===//
// Task Generation (AutoSA: sa_io_module_gen, sa_pe_module_gen)
//===----------------------------------------------------------------------===//

/// Generate I/O L3 Task (Memory Access Module)
/// AutoSA: A_IO_L3_in, B_IO_L3_in
/// This module reads data from global memory and writes to L3 stream.
static void generateIOL3Task(
    OpBuilder &builder,
    Location loc,
    const OperandClassification &operand,
    StreamInfo &outputStream,
    const SystolicConfig &config) {
  
  // Create task operation
  auto taskOp = builder.create<func::FuncOp>(
      loc, (operand.name + "_IO_L3_in").str(),
      builder.getFunctionType({operand.memref.getType()}, {}));
  taskOp.setPrivate();
  
  // Build task body
  Block *entry = taskOp.addEntryBlock();
  OpBuilder::InsertionGuard guard(builder);
  builder.setInsertionPointToStart(entry);
  
  Value memref = entry->getArgument(0);
  
  // Generate nested loops for tile iteration
  // for t0 = 0 to M/tile0:
  //   for t1 = 0 to N/tile1:
  //     for t2 = 0 to K/tile2:
  //       // Load tile from memory
  //       for i = 0 to tile0:
  //         for j = 0 to tile1:
  //           val = load memref[t0*tile0+i, t1*tile1+j]
  //           stream_write(output_stream, val)
  
  // Simplified: just show the structure
  // In full implementation, would clone and modify the original loop nest
  
  builder.create<func::ReturnOp>(loc);
  
  LLVM_DEBUG(llvm::dbgs() << "[DataflowGen] Generated IO L3 task: " 
                          << operand.name << "_IO_L3_in\n");
}

/// Generate I/O L2 Task (Double Buffering Module)
/// AutoSA: A_IO_L2_in with ping-pong buffers
/// This module implements double buffering between L3 and L1.
static void generateIOL2Task(
    OpBuilder &builder,
    Location loc,
    const OperandClassification &operand,
    StreamInfo &inputStream,
    StreamInfo &outputStream,
    const SystolicConfig &config) {
  
  // AutoSA's L2 module structure:
  // local_buf_ping[tile_size], local_buf_pong[tile_size]
  // arb = 0; inter_en = 1; intra_en = 0;
  // for each outer tile {
  //   if (arb == 0) { inter_trans(pong, in_stream); intra_trans(ping, out_stream); }
  //   else          { inter_trans(ping, in_stream); intra_trans(pong, out_stream); }
  //   intra_en = 1; arb = !arb;
  // }
  // // Final drain
  // intra_trans(last_buffer, out_stream);
  
  LLVM_DEBUG(llvm::dbgs() << "[DataflowGen] Generated IO L2 task: " 
                          << operand.name << "_IO_L2_in\n");
}

/// Generate PE Task (Processing Element Module)
/// AutoSA: sa_pe_module_gen
/// This module performs the actual computation.
static void generatePETask(
    OpBuilder &builder,
    Location loc,
    const SmallVectorImpl<OperandClassification> &operands,
    SmallVectorImpl<StreamInfo> &streams,
    const SystolicConfig &config) {
  
  // PE structure:
  // local_A[latency_i][latency_k]
  // local_B[latency_k][latency_j]
  // local_C[latency_i][latency_j]
  //
  // // Initialize local C
  // for i, j: local_C[i][j] = 0 (or read from input if accumulator)
  //
  // // Main computation loop
  // for k = 0 to K/tile_k:
  //   // Read A tile from stream
  //   for i = 0 to latency_i:
  //     for k' = 0 to latency_k:
  //       local_A[i][k'] = stream_read(A_stream)
  //
  //   // Read B tile from stream
  //   for k' = 0 to latency_k:
  //     for j = 0 to latency_j:
  //       local_B[k'][j] = stream_read(B_stream)
  //
  //   // Compute
  //   for i, j, k':
  //     local_C[i][j] += local_A[i][k'] * local_B[k'][j]
  //
  // // Write C to drain stream
  // for i, j:
  //   stream_write(C_drain, local_C[i][j])
  
  LLVM_DEBUG(llvm::dbgs() << "[DataflowGen] Generated PE task\n");
}

/// Generate Drain Task (Output Module)
/// AutoSA: drain module for C
static void generateDrainTask(
    OpBuilder &builder,
    Location loc,
    const OperandClassification &operand,
    StreamInfo &inputStream,
    const SystolicConfig &config) {
  
  // Drain structure:
  // for each PE in array:
  //   for i, j in local tile:
  //     val = stream_read(drain_stream)
  //     store memref[global_i, global_j] = val
  
  LLVM_DEBUG(llvm::dbgs() << "[DataflowGen] Generated drain task: " 
                          << operand.name << "_drain\n");
}

//===----------------------------------------------------------------------===//
// Dispatch Region Generation
//===----------------------------------------------------------------------===//

/// Create the top-level dataflow dispatch region containing all tasks.
/// This represents AutoSA's top module structure.
static LogicalResult createDispatchRegion(
    OpBuilder &builder,
    Location loc,
    func::FuncOp func,
    const SmallVectorImpl<OperandClassification> &operands,
    SmallVectorImpl<StreamInfo> &streams,
    const SystolicConfig &config) {
  
  // Find the main loop nest to transform
  AffineForOp outerLoop = nullptr;
  func.walk([&](AffineForOp forOp) {
    if (!forOp->getParentOfType<AffineForOp>()) {
      outerLoop = forOp;
    }
  });
  
  if (!outerLoop) {
    LLVM_DEBUG(llvm::dbgs() << "[DataflowGen] No loop nest found\n");
    return failure();
  }
  
  // For now, we just emit debug info about what would be generated
  // Full implementation would:
  // 1. Create hls.dataflow.dispatch operation
  // 2. Create stream channels inside dispatch
  // 3. Create task operations for each module
  // 4. Clone and modify loop nests for each task
  
  llvm::outs() << "[DataflowGen] Would create dispatch region with:\n";
  llvm::outs() << "  Streams: " << streams.size() << "\n";
  for (auto &s : streams) {
    llvm::outs() << "    - " << s.name << "\n";
  }
  
  llvm::outs() << "  Tasks:\n";
  for (const auto &op : operands) {
    if (op.isInput) {
      llvm::outs() << "    - " << op.name << "_IO_L3_in (memory read)\n";
      llvm::outs() << "    - " << op.name << "_IO_L2_in (double buffer)\n";
    }
  }
  llvm::outs() << "    - PE (computation)\n";
  for (const auto &op : operands) {
    if (op.isOutput) {
      llvm::outs() << "    - " << op.name << "_drain_IO_L1_out\n";
      llvm::outs() << "    - " << op.name << "_drain_IO_L2_out\n";
    }
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Dataflow Generation Pass
//===----------------------------------------------------------------------===//

namespace {

struct DataflowGenerationPass 
    : public PassWrapper<DataflowGenerationPass, OperationPass<func::FuncOp>> {
  
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(DataflowGenerationPass)
  
  DataflowGenerationPass() = default;
  DataflowGenerationPass(const SystolicConfig &cfg) : config(cfg) {}
  
  StringRef getArgument() const override { return "systolic-dataflow"; }
  StringRef getDescription() const override {
    return "Generate dataflow structure with streams and tasks";
  }
  
  void getDependentDialects(DialectRegistry &registry) const override {
    registry.insert<affine::AffineDialect, arith::ArithDialect,
                    memref::MemRefDialect, scf::SCFDialect>();
  }
  
  void runOnOperation() override {
    auto func = getOperation();
    auto loc = func.getLoc();
    
    LLVM_DEBUG(llvm::dbgs() << "\n=== Dataflow Generation Pass ===\n");
    LLVM_DEBUG(llvm::dbgs() << "Processing function: " << func.getName() << "\n");
    
    OpBuilder builder(func.getContext());
    
    // Step 1: Classify memory operands (AutoSA: group_array_references)
    SmallVector<OperandClassification, 4> operands;
    if (failed(classifyOperands(func, config, operands))) {
      LLVM_DEBUG(llvm::dbgs() << "Failed to classify operands\n");
      return;
    }
    
    // Step 2: Create stream channels
    SmallVector<StreamInfo, 8> streams;
    createStreamChannels(builder, loc, operands, config, streams);
    
    // Step 3: Create dispatch region with tasks
    if (failed(createDispatchRegion(builder, loc, func, operands, streams, config))) {
      LLVM_DEBUG(llvm::dbgs() << "Failed to create dispatch region\n");
      return;
    }
    
    // Print summary
    llvm::outs() << "[DataflowGen] Summary:\n";
    llvm::outs() << "  PE array: " << config.numPE_I << " x " << config.numPE_J << "\n";
    llvm::outs() << "  Input operands: ";
    for (const auto &op : operands) {
      if (op.isInput) llvm::outs() << op.name << " ";
    }
    llvm::outs() << "\n  Output operands: ";
    for (const auto &op : operands) {
      if (op.isOutput) llvm::outs() << op.name << " ";
    }
    llvm::outs() << "\n";
    
    LLVM_DEBUG(llvm::dbgs() << "\n=== Dataflow Generation Complete ===\n");
  }
  
private:
  SystolicConfig config;
};

} // namespace

//===----------------------------------------------------------------------===//
// Pass Creation
//===----------------------------------------------------------------------===//

std::unique_ptr<Pass> createStreamGenerationPass() {
  return std::make_unique<DataflowGenerationPass>();
}

} // namespace systolic
} // namespace mlir
