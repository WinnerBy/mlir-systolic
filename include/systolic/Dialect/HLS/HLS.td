//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: HLS Dialect Definition
// Adapted from ScaleHLS (Copyright 2020-2021 The ScaleHLS Authors)
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_HLS_HLS_TD
#define SYSTOLIC_DIALECT_HLS_HLS_TD

include "mlir/IR/OpBase.td"

//===----------------------------------------------------------------------===//
// HLS Dialect Definition
//===----------------------------------------------------------------------===//

def HLS_Dialect : Dialect {
  let name = "hls";
  let cppNamespace = "::mlir::systolic::hls";
  let summary = "HLS dialect for systolic array generation";
  let description = [{
    This dialect defines operations for representing dataflow structures
    commonly used in HLS (High-Level Synthesis) designs, particularly
    for systolic arrays.
  }];

  let useDefaultTypePrinterParser = 1;
  let useDefaultAttributePrinterParser = 1;
}

//===----------------------------------------------------------------------===//
// Base Classes
//===----------------------------------------------------------------------===//

class HLSOp<string mnemonic, list<Trait> traits = []> :
    Op<HLS_Dialect, mnemonic, traits>;

#endif // SYSTOLIC_DIALECT_HLS_HLS_TD
