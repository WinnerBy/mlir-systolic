//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: HLS Dialect Header
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_HLS_HLS_H
#define SYSTOLIC_DIALECT_HLS_HLS_H

#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"

namespace mlir {
namespace systolic {
namespace hls {

//===----------------------------------------------------------------------===//
// HLS Types - Forward declarations
//===----------------------------------------------------------------------===//

class StreamType;

} // namespace hls
} // namespace systolic
} // namespace mlir

//===----------------------------------------------------------------------===//
// HLS Dialect
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/HLS/HLSDialect.h.inc"

//===----------------------------------------------------------------------===//
// HLS Types
//===----------------------------------------------------------------------===//

#define GET_TYPEDEF_CLASSES
#include "systolic/Dialect/HLS/Types.h.inc"

//===----------------------------------------------------------------------===//
// HLS Operations
//===----------------------------------------------------------------------===//

#define GET_OP_CLASSES
#include "systolic/Dialect/HLS/Ops.h.inc"

namespace mlir {
namespace systolic {
namespace hls {

/// Register the HLS dialect and types.
void registerHLSDialect(DialectRegistry &registry);

} // namespace hls
} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_DIALECT_HLS_HLS_H
