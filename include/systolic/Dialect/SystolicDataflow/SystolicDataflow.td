//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: SystolicDataflow Dialect Definition
//
// This dialect provides abstractions for representing systolic array
// hardware structures, including multi-level IO modules, PE arrays, and
// double buffering mechanisms.
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOW_TD
#define SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOW_TD

include "mlir/IR/OpBase.td"

//===----------------------------------------------------------------------===//
// SystolicDataflow Dialect Definition
//===----------------------------------------------------------------------===//

def SystolicDataflow_Dialect : Dialect {
  let name = "systolic.dataflow";
  let cppNamespace = "::mlir::systolic::dataflow";
  let summary = "Systolic array dataflow abstraction dialect";
  let description = [{
    This dialect defines operations for representing systolic array hardware
    structures at an abstract level, including:
    - Multi-level IO modules (L1, L2, L3)
    - PE (Processing Element) arrays
    - Double buffering (ping-pong) mechanisms
    - Data transfer operations (inter/intra transfer)
    
    This dialect serves as an intermediate representation between Affine IR
    and HLS Dialect, allowing for hardware-aware optimizations before code
    generation.
  }];

  let useDefaultTypePrinterParser = 1;
}

//===----------------------------------------------------------------------===//
// Base Classes
//===----------------------------------------------------------------------===//

class SystolicDataflowOp<string mnemonic, list<Trait> traits = []> :
    Op<SystolicDataflow_Dialect, mnemonic, traits>;

#endif // SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOW_TD

