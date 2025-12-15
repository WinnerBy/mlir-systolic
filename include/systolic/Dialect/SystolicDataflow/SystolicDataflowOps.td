//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: SystolicDataflow Operations Definition
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOWOPS_TD
#define SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOWOPS_TD

include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/IR/SymbolInterfaces.td"
include "systolic/Dialect/SystolicDataflow/SystolicDataflow.td"

//===----------------------------------------------------------------------===//
// IO Module Operation
//===----------------------------------------------------------------------===//

/// IO module at a specific level (L1, L2, or L3)
/// Represents an I/O module that handles data transfer at a specific
/// level of the memory hierarchy.
def IOModuleOp : SystolicDataflowOp<"io.module", [
    RecursiveMemoryEffects,
    SingleBlockImplicitTerminator<"IOModuleYieldOp">,
    IsolatedFromAbove
  ]> {
  let summary = "IO module at a specific level (L1, L2, or L3)";
  let description = [{
    Represents an IO module at level L1, L2, or L3 in the memory hierarchy:
    - L3: Interface with global memory (DRAM)
    - L2: Intermediate level with double buffering (ping-pong)
    - L1: Interface with PE array
    
    The module contains buffer allocation and data transfer logic.
  }];

  let arguments = (ins
    I32Attr:$level,              // 1, 2, or 3
    StrAttr:$direction,          // "in" or "out"
    StrAttr:$arrayName,          // Array name (e.g., "A", "B", "C")
    OptionalAttr<I64ArrayAttr>:$bufferShape,  // Buffer dimensions
    OptionalAttr<BoolAttr>:$doubleBuffer,      // Enable ping-pong buffering
    OptionalAttr<StrAttr>:$name               // Optional module name
  );
  
  let results = (outs);
  let regions = (region SizedRegion<1>:$body);
  
  let assemblyFormat = [{
    ($name^)? attr-dict `level` `=` $level `direction` `=` $direction
    `array` `=` $arrayName
    (`buffer_shape` `=` $bufferShape^)?
    (`double_buffer` `=` $doubleBuffer^)?
    `:` $body
  }];
  
  let hasVerifier = 1;
  
  let extraClassDeclaration = [{
    /// Get the terminator yield op.
    IOModuleYieldOp getYieldOp();
    
    /// Check if double buffering is enabled.
    bool hasDoubleBuffer();
  }];
}

/// Terminator for IO module
def IOModuleYieldOp : SystolicDataflowOp<"io.module.yield", [
    NoMemoryEffect,
    ReturnLike,
    Terminator,
    ParentOneOf<["IOModuleOp"]>
  ]> {
  let summary = "Terminate an IO module";
  let arguments = (ins);
  let assemblyFormat = "attr-dict";
}

//===----------------------------------------------------------------------===//
// PE Array Operation
//===----------------------------------------------------------------------===//

/// Processing Element array
/// Represents a 2D array of PEs that perform systolic computation.
def PEArrayOp : SystolicDataflowOp<"pe.array", [
    RecursiveMemoryEffects,
    SingleBlockImplicitTerminator<"PEArrayYieldOp">,
    IsolatedFromAbove
  ]> {
  let summary = "Processing Element array";
  let description = [{
    Represents a 2D PE array with systolic data flow. The array performs
    the core computation of the systolic array, with data flowing
    between adjacent PEs.
  }];

  let arguments = (ins
    I64ArrayAttr:$arraySize,      // [rows, cols] - PE array dimensions
    I64ArrayAttr:$tileSize,        // Tile dimensions for computation
    OptionalAttr<StrAttr>:$name    // Optional array name
  );
  
  let results = (outs);
  let regions = (region SizedRegion<1>:$body);
  
  let assemblyFormat = [{
    ($name^)? attr-dict `array_size` `=` $arraySize `tile_size` `=` $tileSize
    `:` $body
  }];
  
  let hasVerifier = 1;
  
  let extraClassDeclaration = [{
    /// Get the terminator yield op.
    PEArrayYieldOp getYieldOp();
  }];
}

/// Terminator for PE array
def PEArrayYieldOp : SystolicDataflowOp<"pe.array.yield", [
    NoMemoryEffect,
    ReturnLike,
    Terminator,
    ParentOneOf<["PEArrayOp"]>
  ]> {
  let summary = "Terminate a PE array";
  let arguments = (ins);
  let assemblyFormat = "attr-dict";
}

//===----------------------------------------------------------------------===//
// Double Buffer Operation
//===----------------------------------------------------------------------===//

/// Double buffer (ping-pong) operation
/// Manages two buffers that alternate between inter-transfer (loading)
/// and intra-transfer (sending) phases.
def DoubleBufferOp : SystolicDataflowOp<"double_buffer", [
    RecursiveMemoryEffects,
    SingleBlockImplicitTerminator<"DoubleBufferYieldOp">
  ]> {
  let summary = "Double buffer (ping-pong)";
  let description = [{
    Represents a ping-pong double buffering mechanism. The operation
    manages two buffers (ping and pong) that alternate roles:
    - While one buffer is being filled (inter-transfer), the other is
      being consumed (intra-transfer)
    - This hides data loading latency by overlapping computation with
      communication
  }];

  let arguments = (ins
    AnyMemRef:$pingBuffer,          // Ping buffer
    AnyMemRef:$pongBuffer,          // Pong buffer
    MemRefOf<[I1]>:$arbiter,        // Arbiter flag (which buffer is active)
    MemRefOf<[I1]>:$intraEnable     // Intra-transfer enable flag
  );
  
  let regions = (region SizedRegion<1>:$interTransfer,  // Inter-transfer logic (loading)
                 SizedRegion<1>:$intraTransfer);  // Intra-transfer logic (sending)
  
  let results = (outs);
  
  let assemblyFormat = [{
    $pingBuffer `:` type($pingBuffer) `,` $pongBuffer `:` type($pongBuffer) `,`
    $arbiter `:` type($arbiter) `,` $intraEnable `:` type($intraEnable)
    attr-dict `inter_transfer` `=` $interTransfer
    `intra_transfer` `=` $intraTransfer
  }];
  
  let hasVerifier = 1;
  
  let extraClassDeclaration = [{
    /// Get the terminator yield op for inter-transfer region.
    DoubleBufferYieldOp getInterYieldOp();
    
    /// Get the terminator yield op for intra-transfer region.
    DoubleBufferYieldOp getIntraYieldOp();
  }];
}

/// Terminator for double buffer regions
def DoubleBufferYieldOp : SystolicDataflowOp<"double_buffer.yield", [
    NoMemoryEffect,
    ReturnLike,
    Terminator,
    ParentOneOf<["DoubleBufferOp"]>
  ]> {
  let summary = "Terminate a double buffer region";
  let arguments = (ins);
  let assemblyFormat = "attr-dict";
}

//===----------------------------------------------------------------------===//
// Drain Module Operation
//===----------------------------------------------------------------------===//

/// Drain module for output data
/// Collects results from PE array and writes them back to memory.
def DrainModuleOp : SystolicDataflowOp<"drain.module", [
    RecursiveMemoryEffects,
    SingleBlockImplicitTerminator<"DrainModuleYieldOp">,
    IsolatedFromAbove
  ]> {
  let summary = "Drain module for output data";
  let description = [{
    Represents a drain module that collects results from the PE array
    and writes them back to memory. Similar to IO modules but for
    output data flow.
  }];

  let arguments = (ins
    I32Attr:$level,              // 1, 2, or 3
    StrAttr:$arrayName,          // Output array name (e.g., "C")
    OptionalAttr<I64ArrayAttr>:$bufferShape,
    OptionalAttr<StrAttr>:$name
  );
  
  let results = (outs);
  let regions = (region SizedRegion<1>:$body);
  
  let assemblyFormat = [{
    ($name^)? attr-dict `level` `=` $level `array` `=` $arrayName
    (`buffer_shape` `=` $bufferShape^)?
    `:` $body
  }];
  
  let hasVerifier = 1;
  
  let extraClassDeclaration = [{
    /// Get the terminator yield op.
    DrainModuleYieldOp getYieldOp();
  }];
}

/// Terminator for drain module
def DrainModuleYieldOp : SystolicDataflowOp<"drain.module.yield", [
    NoMemoryEffect,
    ReturnLike,
    Terminator,
    ParentOneOf<["DrainModuleOp"]>
  ]> {
  let summary = "Terminate a drain module";
  let arguments = (ins);
  let assemblyFormat = "attr-dict";
}

#endif // SYSTOLIC_DIALECT_SYSTOLICDATAFLOW_SYSTOLICDATAFLOWOPS_TD

