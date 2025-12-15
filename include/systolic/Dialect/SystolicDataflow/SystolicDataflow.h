//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: SystolicDataflow Dialect Header
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOW_H
#define SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOW_H

#include "mlir/IR/Dialect.h"
#include "mlir/IR/OpDefinition.h"
#include "mlir/IR/BuiltinTypes.h"
#include "mlir/Bytecode/BytecodeOpInterface.h"
#include "mlir/Interfaces/SideEffectInterfaces.h"
#include "mlir/Interfaces/ControlFlowInterfaces.h"
#include "mlir/Dialect/Func/IR/FuncOps.h"
#include "mlir/Dialect/Affine/IR/AffineOps.h"
#include "mlir/Dialect/MemRef/IR/MemRef.h"
#include "mlir/Dialect/Arith/IR/Arith.h"

namespace mlir {
namespace systolic {
namespace dataflow {
} // namespace dataflow
} // namespace systolic
} // namespace mlir

//===----------------------------------------------------------------------===//
// SystolicDataflow Dialect
//===----------------------------------------------------------------------===//

#include "systolic/Dialect/SystolicDataflow/SystolicDataflowDialect.h.inc"

//===----------------------------------------------------------------------===//
// SystolicDataflow Operations
//===----------------------------------------------------------------------===//

#define GET_OP_CLASSES
#include "systolic/Dialect/SystolicDataflow/Ops.h.inc"

namespace mlir {
namespace systolic {
namespace dataflow {

/// Register the SystolicDataflow dialect.
void registerSystolicDataflowDialect(DialectRegistry &registry);

} // namespace dataflow
} // namespace systolic
} // namespace mlir

#endif // SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOW_H

