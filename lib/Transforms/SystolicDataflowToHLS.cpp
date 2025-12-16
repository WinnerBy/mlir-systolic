//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: SystolicDataflow to HLS Dialect Lowering
//
// This pass lowers SystolicDataflow Dialect operations to HLS Dialect,
// converting high-level systolic array abstractions to concrete HLS
// dataflow structures.
//
// Lowering Strategy:
//   - systolic.io.module -> hls.dataflow.task (with appropriate name)
//   - systolic.pe.array -> hls.dataflow.task (PE computation)
//   - systolic.drain.module -> hls.dataflow.task (output)
//   - Double buffering logic -> Affine loops + SCF if (ping-pong)
//
//===----------------------------------------------------------------------===//

#include "systolic/Transforms/Passes.h"
#include "systolic/Dialect/SystolicDataflow/SystolicDataflow.h"
#include "systolic/Dialect/HLS/HLS.h"

#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/Arith/IR/Arith.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/SCF/IR/SCF.h"
#include "mlir/IR/Builders.h"
#include "mlir/IR/BuiltinOps.h"
#include "mlir/IR/PatternMatch.h"
#include "mlir/Pass/Pass.h"
#include "mlir/Transforms/DialectConversion.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "systolic-dataflow-to-hls"

using namespace mlir;
using namespace mlir::systolic;
using namespace mlir::systolic::dataflow;

namespace mlir {
namespace systolic {

//===----------------------------------------------------------------------===//
// Conversion Patterns
//===----------------------------------------------------------------------===//

/// Convert IOModuleOp to HLS dataflow task
struct IOModuleOpLowering : public OpConversionPattern<IOModuleOp> {
  using OpConversionPattern<IOModuleOp>::OpConversionPattern;

  LogicalResult matchAndRewrite(
      IOModuleOp op, OpAdaptor adaptor,
      ConversionPatternRewriter &rewriter) const override {
    
    Location loc = op.getLoc();
    
    // Build task name: e.g., "A_IO_L2_in"
    std::string taskName = adaptor.getArrayName().str();
    taskName += "_IO_L" + std::to_string(adaptor.getLevel()) + "_";
    taskName += adaptor.getDirection().str();
    
    // Create HLS dataflow task
    // Note: This assumes HLS Dialect has a dataflow.task operation
    // The exact API depends on the HLS Dialect definition
    rewriter.setInsertionPoint(op);
    
    // For now, we'll create a placeholder structure
    // TODO: Implement actual HLS task creation based on HLS Dialect API
    LLVM_DEBUG(llvm::dbgs() << "Lowering IO module " << taskName << "\n");
    
    // Move the body content to the new task
    // In a real implementation, we would:
    // 1. Create hls.dataflow.task with name=taskName
    // 2. Move the body region content
    // 3. Handle double buffering if enabled
    
    // For now, just remove the op (placeholder)
    rewriter.eraseOp(op);
    
    return success();
  }
};

/// Convert PEArrayOp to HLS dataflow task
struct PEArrayOpLowering : public OpConversionPattern<PEArrayOp> {
  using OpConversionPattern<PEArrayOp>::OpConversionPattern;

  LogicalResult matchAndRewrite(
      PEArrayOp op, OpAdaptor adaptor,
      ConversionPatternRewriter &rewriter) const override {
    
    Location loc = op.getLoc();
    
    // Build task name: e.g., "PE_array"
    std::string taskName = "PE_array";
    if (adaptor.getName())
      taskName = adaptor.getName()->str();
    
    LLVM_DEBUG(llvm::dbgs() << "Lowering PE array " << taskName << "\n");
    
    // TODO: Implement actual HLS task creation
    // 1. Create hls.dataflow.task with name=taskName
    // 2. Move the body region content
    // 3. Generate PE computation logic
    
    rewriter.eraseOp(op);
    
    return success();
  }
};

/// Convert DrainModuleOp to HLS dataflow task
struct DrainModuleOpLowering : public OpConversionPattern<DrainModuleOp> {
  using OpConversionPattern<DrainModuleOp>::OpConversionPattern;

  LogicalResult matchAndRewrite(
      DrainModuleOp op, OpAdaptor adaptor,
      ConversionPatternRewriter &rewriter) const override {
    
    Location loc = op.getLoc();
    
    // Build task name: e.g., "C_drain_L2_out"
    std::string taskName = adaptor.getArrayName().str();
    taskName += "_drain_L" + std::to_string(adaptor.getLevel()) + "_out";
    
    LLVM_DEBUG(llvm::dbgs() << "Lowering drain module " << taskName << "\n");
    
    // TODO: Implement actual HLS task creation
    // 1. Create hls.dataflow.task with name=taskName
    // 2. Move the body region content
    // 3. Generate output logic
    
    rewriter.eraseOp(op);
    
    return success();
  }
};

/// Convert DoubleBufferOp to Affine loops + SCF if
struct DoubleBufferOpLowering : public OpConversionPattern<DoubleBufferOp> {
  using OpConversionPattern<DoubleBufferOp>::OpConversionPattern;

  LogicalResult matchAndRewrite(
      DoubleBufferOp op, OpAdaptor adaptor,
      ConversionPatternRewriter &rewriter) const override {
    
    Location loc = op.getLoc();
    
    LLVM_DEBUG(llvm::dbgs() << "Lowering double buffer\n");
    
    // Double buffering logic:
    // 1. Create a loop that alternates between ping and pong
    // 2. Use SCF if to select which buffer to use
    // 3. Inter-transfer: load data into inactive buffer
    // 4. Intra-transfer: send data from active buffer
    
    // TODO: Implement actual double buffering logic
    // This is complex and depends on the specific data flow pattern
    
    rewriter.eraseOp(op);
    
    return success();
  }
};

//===----------------------------------------------------------------------===//
// SystolicDataflowToHLS Pass
//===----------------------------------------------------------------------===//

namespace {
struct SystolicDataflowToHLSPass
    : public PassWrapper<SystolicDataflowToHLSPass, OperationPass<ModuleOp>> {
  
  void runOnOperation() override {
    ModuleOp module = getOperation();
    
    ConversionTarget target(getContext());
    
    // Mark SystolicDataflow operations as illegal
    target.addIllegalDialect<SystolicDataflowDialect>();
    
    // Mark HLS Dialect as legal
    // TODO: Add HLS Dialect to target when available
    
    // Mark standard dialects as legal
    target.addLegalDialect<affine::AffineDialect>();
    target.addLegalDialect<arith::ArithDialect>();
    target.addLegalDialect<func::FuncDialect>();
    target.addLegalDialect<memref::MemRefDialect>();
    target.addLegalDialect<scf::SCFDialect>();
    
    RewritePatternSet patterns(&getContext());
    
    // Add conversion patterns
    patterns.add<IOModuleOpLowering>(&getContext());
    patterns.add<PEArrayOpLowering>(&getContext());
    patterns.add<DrainModuleOpLowering>(&getContext());
    patterns.add<DoubleBufferOpLowering>(&getContext());
    
    if (failed(applyPartialConversion(module, target, std::move(patterns)))) {
      signalPassFailure();
      return;
    }
    
    LLVM_DEBUG(llvm::dbgs() << "SystolicDataflow to HLS lowering complete\n");
  }
  
  StringRef getArgument() const override {
    return "systolic-dataflow-to-hls";
  }
  
  StringRef getDescription() const override {
    return "Lower SystolicDataflow Dialect to HLS Dialect";
  }
};
} // namespace

//===----------------------------------------------------------------------===//
// Pass Registration
//===----------------------------------------------------------------------===//

std::unique_ptr<Pass> createSystolicDataflowToHLSPass() {
  return std::make_unique<SystolicDataflowToHLSPass>();
}

// Register the pass
static PassRegistration<SystolicDataflowToHLSPass> passRegistration;

} // namespace systolic
} // namespace mlir

