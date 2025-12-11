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
#include "mlir/Interfaces/SideEffectInterfaces.h"

//===----------------------------------------------------------------------===//
// HLS Dialect
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/HLS/HLSDialect.h.inc"

//===----------------------------------------------------------------------===//
// HLS Types
//===----------------------------------------------------------------------===//

#define GET_TYPEDEF_CLASSES
#include "systolic/Dialect/HLS/HLSTypes.h.inc"

//===----------------------------------------------------------------------===//
// HLS Operations
//===----------------------------------------------------------------------===//

#define GET_OP_CLASSES
#include "systolic/Dialect/HLS/HLSOps.h.inc"

namespace mlir {
namespace systolic {
namespace hls {

/// Register the HLS dialect and types.
void registerHLSDialect(DialectRegistry &registry);

} // namespace hls
} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_DIALECT_HLS_HLS_H
