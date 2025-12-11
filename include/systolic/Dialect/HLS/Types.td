//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: HLS Types Definition
// Adapted from ScaleHLS
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_HLS_TYPES_TD
#define SYSTOLIC_DIALECT_HLS_TYPES_TD

include "mlir/IR/AttrTypeBase.td"
include "systolic/Dialect/HLS/HLS.td"

//===----------------------------------------------------------------------===//
// Type Definitions
//===----------------------------------------------------------------------===//

class HLSType<string name, string typeMnemonic, list<Trait> traits = []>
    : TypeDef<HLS_Dialect, name, traits> {
  let mnemonic = typeMnemonic;
}

def StreamType : HLSType<"Stream", "stream"> {
  let summary = "Stream channel type for dataflow communication";
  let description = [{
    A stream represents a FIFO channel used for communication between
    dataflow tasks. Streams are the primary mechanism for data movement
    in systolic arrays.
  }];

  let parameters = (ins "Type":$elementType, "unsigned":$depth);
  
  let assemblyFormat = "`<` $elementType `,` $depth `>`";
  
  let builders = [
    TypeBuilderWithInferredContext<(ins "Type":$elementType, 
                                        CArg<"unsigned", "1">:$depth), [{
      return $_get(elementType.getContext(), elementType, depth);
    }]>
  ];
  
  let extraClassDeclaration = [{
    /// Get a stream with default depth
    static StreamType get(Type elementType) {
      return get(elementType, 1);
    }
  }];
}

def BundleType : HLSType<"Bundle", "bundle"> {
  let summary = "AXI bundle type";
  let description = [{
    Represents an AXI interface bundle for external memory access.
  }];
  
  let parameters = (ins);
  let assemblyFormat = "";
}

def AxiType : HLSType<"Axi", "axi"> {
  let summary = "AXI port type";
  let description = [{
    Represents an AXI port for DDR memory access.
  }];
  
  let parameters = (ins "Type":$elementType);
  let assemblyFormat = "`<` $elementType `>`";
  
  let builders = [
    TypeBuilderWithInferredContext<(ins "Type":$elementType), [{
      return $_get(elementType.getContext(), elementType);
    }]>
  ];
}

#endif // SYSTOLIC_DIALECT_HLS_TYPES_TD
