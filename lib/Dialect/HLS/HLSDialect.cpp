//===- HLSDialect.cpp - HLS Dialect Implementation ---------------*- C++ -*-===//
//
// Part of the mlir-systolic project.
//
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/HLS/HLS.h"

#include "mlir/IR/DialectImplementation.h"
#include "mlir/IR/OpImplementation.h"
#include "llvm/ADT/TypeSwitch.h"

using namespace mlir;
using namespace mlir::systolic::hls;

//===----------------------------------------------------------------------===//
// HLS Dialect
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/HLS/HLSDialect.cpp.inc"

void HLSDialect::initialize() {
  addOperations<
#define GET_OP_LIST
#include "systolic/Dialect/HLS/Ops.cpp.inc"
      >();
  addTypes<
#define GET_TYPEDEF_LIST
#include "systolic/Dialect/HLS/Types.cpp.inc"
      >();
}

//===----------------------------------------------------------------------===//
// Stream Type
//===----------------------------------------------------------------------===//

#define GET_TYPEDEF_CLASSES
#include "systolic/Dialect/HLS/Types.cpp.inc"

//===----------------------------------------------------------------------===//
// Operations
//===----------------------------------------------------------------------===//

#define GET_OP_CLASSES
#include "systolic/Dialect/HLS/Ops.cpp.inc"

//===----------------------------------------------------------------------===//
// DispatchOp
//===----------------------------------------------------------------------===//

YieldOp DispatchOp::getYieldOp() {
  return cast<YieldOp>(getBody().front().getTerminator());
}

LogicalResult DispatchOp::verify() {
  // Verify that all nested operations are either TaskOps or control flow
  for (Operation &op : getBody().front()) {
    if (!isa<TaskOp, YieldOp>(&op)) {
      return emitOpError("expected only hls.dataflow.task or hls.dataflow.yield ops inside");
    }
  }
  return success();
}

//===----------------------------------------------------------------------===//
// TaskOp
//===----------------------------------------------------------------------===//

DispatchOp TaskOp::getDispatchOp() {
  return cast<DispatchOp>((*this)->getParentOp());
}

YieldOp TaskOp::getYieldOp() {
  return cast<YieldOp>(getBody().front().getTerminator());
}

LogicalResult TaskOp::verify() {
  // Basic verification - ensure parent is a DispatchOp
  if (!isa<DispatchOp>((*this)->getParentOp())) {
    return emitOpError("expected parent to be hls.dataflow.dispatch");
  }
  return success();
}

//===----------------------------------------------------------------------===//
// StreamReadOp
//===----------------------------------------------------------------------===//

LogicalResult StreamReadOp::verify() {
  auto streamType = getChannel().getType().cast<StreamType>();
  if (getResult()) {
    if (getResult().getType() != streamType.getElementType()) {
      return emitOpError("result type must match stream element type");
    }
  }
  return success();
}

//===----------------------------------------------------------------------===//
// StreamWriteOp
//===----------------------------------------------------------------------===//

LogicalResult StreamWriteOp::verify() {
  auto streamType = getChannel().getType().cast<StreamType>();
  if (getValue().getType() != streamType.getElementType()) {
    return emitOpError("value type must match stream element type");
  }
  return success();
}
