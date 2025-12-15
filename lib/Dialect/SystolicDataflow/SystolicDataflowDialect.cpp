//===- SystolicDataflowDialect.cpp - SystolicDataflow Dialect Implementation -==//
//
// Part of the mlir-systolic project.
//
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/SystolicDataflow/SystolicDataflow.h"

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
using namespace mlir::systolic::dataflow;

//===----------------------------------------------------------------------===//
// SystolicDataflow Dialect
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/SystolicDataflow/SystolicDataflowDialect.cpp.inc"

void SystolicDataflowDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "systolic/Dialect/SystolicDataflow/Ops.cpp.inc"
      >();
}

// Print a type registered to this dialect.
void SystolicDataflowDialect::printType(Type type, DialectAsmPrinter &os) const {
  // No custom types yet, so nothing to print
  llvm_unreachable("SystolicDataflow dialect has no types");
}

// Parse a type registered to this dialect.
Type SystolicDataflowDialect::parseType(DialectAsmParser &parser) const {
  // No custom types yet, so nothing to parse
  return Type();
}

//===----------------------------------------------------------------------===//
// Operations
//===----------------------------------------------------------------------===//

#define GET_OP_CLASSES
#include "systolic/Dialect/SystolicDataflow/Ops.cpp.inc"

//===----------------------------------------------------------------------===//
// IOModuleOp
//===----------------------------------------------------------------------===//

IOModuleYieldOp IOModuleOp::getYieldOp() {
  return cast<IOModuleYieldOp>(getBody().front().getTerminator());
}

LogicalResult IOModuleOp::verify() {
  // Verify level is 1, 2, or 3
  int32_t level = getLevel();
  if (level < 1 || level > 3) {
    return emitOpError("level must be 1, 2, or 3, got ") << level;
  }
  
  // Verify direction is "in" or "out"
  StringRef direction = getDirection();
  if (direction != "in" && direction != "out") {
    return emitOpError("direction must be 'in' or 'out', got '")
           << direction << "'";
  }
  
  // Verify buffer shape if specified
  if (getBufferShapeAttr()) {
    for (auto attr : getBufferShapeAttr().getAsRange<IntegerAttr>()) {
      if (attr.getInt() <= 0) {
        return emitOpError("buffer shape dimensions must be positive");
      }
    }
  }
  
  return success();
}

bool IOModuleOp::hasDoubleBuffer() {
  return getDoubleBufferAttr() && getDoubleBufferAttr().getValue();
}

//===----------------------------------------------------------------------===//
// PEArrayOp
//===----------------------------------------------------------------------===//

PEArrayYieldOp PEArrayOp::getYieldOp() {
  return cast<PEArrayYieldOp>(getBody().front().getTerminator());
}

LogicalResult PEArrayOp::verify() {
  // Verify array size has exactly 2 dimensions (rows, cols)
  auto arraySize = getArraySizeAttr().getAsRange<IntegerAttr>();
  size_t count = 0;
  for (auto attr : arraySize) {
    count++;
    if (attr.getInt() <= 0) {
      return emitOpError("array_size dimensions must be positive");
    }
  }
  if (count != 2) {
    return emitOpError("array_size must have exactly 2 dimensions [rows, cols]");
  }
  
  // Verify tile size
  for (auto attr : getTileSizeAttr().getAsRange<IntegerAttr>()) {
    if (attr.getInt() <= 0) {
      return emitOpError("tile_size dimensions must be positive");
    }
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// DoubleBufferOp
//===----------------------------------------------------------------------===//

LogicalResult DoubleBufferOp::verify() {
  // Verify ping and pong buffers have the same type
  if (getPingBuffer().getType() != getPongBuffer().getType()) {
    return emitOpError("ping and pong buffers must have the same type");
  }
  
  // Verify arbiter is a memref of i1
  auto arbiterType = getArbiter().getType().cast<MemRefType>();
  if (!arbiterType.getElementType().isInteger(1)) {
    return emitOpError("arbiter must be a memref of i1");
  }
  
  // Verify intra enable is a memref of i1
  auto intraEnableType = getIntraEnable().getType().cast<MemRefType>();
  if (!intraEnableType.getElementType().isInteger(1)) {
    return emitOpError("intra_enable must be a memref of i1");
  }
  
  // Verify regions are not empty
  if (getInterTransfer().empty()) {
    return emitOpError("inter_transfer region cannot be empty");
  }
  if (getIntraTransfer().empty()) {
    return emitOpError("intra_transfer region cannot be empty");
  }
  
  return success();
}

DoubleBufferYieldOp DoubleBufferOp::getInterYieldOp() {
  return cast<DoubleBufferYieldOp>(getInterTransfer().front().getTerminator());
}

DoubleBufferYieldOp DoubleBufferOp::getIntraYieldOp() {
  return cast<DoubleBufferYieldOp>(getIntraTransfer().front().getTerminator());
}

//===----------------------------------------------------------------------===//
// DrainModuleOp
//===----------------------------------------------------------------------===//

DrainModuleYieldOp DrainModuleOp::getYieldOp() {
  return cast<DrainModuleYieldOp>(getBody().front().getTerminator());
}

LogicalResult DrainModuleOp::verify() {
  // Verify level is 1, 2, or 3
  int32_t level = getLevel();
  if (level < 1 || level > 3) {
    return emitOpError("level must be 1, 2, or 3, got ") << level;
  }
  
  // Verify buffer shape if specified
  if (getBufferShapeAttr()) {
    auto shape = getBufferShapeAttr().getAsRange<IntegerAttr>();
    for (auto attr : shape) {
      if (attr.getInt() <= 0) {
        return emitOpError("buffer shape dimensions must be positive");
      }
    }
  }
  
  return success();
}

//===----------------------------------------------------------------------===//
// Dialect Registration
//===----------------------------------------------------------------------===//

void mlir::systolic::dataflow::registerSystolicDataflowDialect(
    DialectRegistry &registry) {
  registry.insert<SystolicDataflowDialect>();
}

