//===----------------------------------------------------------------------===//
//
// MLIR-Systolic: HLS Operations Definition
// Adapted from ScaleHLS
//
//===----------------------------------------------------------------------===//

#ifndef SYSTOLIC_DIALECT_HLS_OPS_TD
#define SYSTOLIC_DIALECT_HLS_OPS_TD

include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/IR/SymbolInterfaces.td"
include "systolic/Dialect/HLS/HLS.td"
include "systolic/Dialect/HLS/Types.td"

//===----------------------------------------------------------------------===//
// Type Constraints
//===----------------------------------------------------------------------===//

class StreamOf<list<Type> allowedTypes> : Type<And<[
    CPred<"$_self.isa<mlir::systolic::hls::StreamType>()">,
    Concat<"[](mlir::Type elementType) { return ",
      SubstLeaves<"$_self", "elementType", AnyTypeOf<allowedTypes>.predicate>,
      "; }($_self.cast<mlir::systolic::hls::StreamType>().getElementType())">
  ]>,
  "stream of " # AnyTypeOf<allowedTypes>.summary # " values",
  "mlir::systolic::hls::StreamType"
>;

def AnyStream : StreamOf<[AnyType]>;

//===----------------------------------------------------------------------===//
// High-Level Dataflow Operations
//===----------------------------------------------------------------------===//

def DispatchOp : HLSOp<"dataflow.dispatch", [
    RecursiveMemoryEffects,
    SingleBlockImplicitTerminator<"YieldOp">,
    ParentOneOf<["func::FuncOp", "affine::AffineForOp"]>
  ]> {
  let summary = "Represent a dataflow dispatch region";
  let description = [{
    A dispatch operation contains multiple task operations that execute
    concurrently in a dataflow manner. This represents the top-level
    structure of a systolic array.
  }];

  let results = (outs Variadic<AnyType>:$results);
  let regions = (region SizedRegion<1>:$body);
  let assemblyFormat = "attr-dict-with-keyword ( `:` type($results)^ )? $body";
  
  let hasVerifier = 1;

  let extraClassDeclaration = [{
    /// Get the terminator yield op.
    YieldOp getYieldOp();
  }];
}

def TaskOp : HLSOp<"dataflow.task", [
    RecursiveMemoryEffects,
    SingleBlockImplicitTerminator<"YieldOp">,
    HasParent<"DispatchOp">
  ]> {
  let summary = "Represent a dataflow task (PE or IO module)";
  let description = [{
    A task represents a single processing element (PE) or I/O module
    in the systolic array. Tasks communicate through streams.
  }];

  let arguments = (ins OptionalAttr<StrAttr>:$name);
  let results = (outs Variadic<AnyType>:$results);
  let regions = (region SizedRegion<1>:$body);
  let assemblyFormat = [{
    ($name^)? attr-dict-with-keyword ( `:` type($results)^ )? $body
  }];

  let hasVerifier = 1;

  let extraClassDeclaration = [{
    /// Get the parent dispatch op.
    DispatchOp getDispatchOp();

    /// Get the terminator yield op.
    YieldOp getYieldOp();
  }];
}

def YieldOp : HLSOp<"dataflow.yield", [
    NoMemoryEffect, 
    ReturnLike, 
    Terminator,
    ParentOneOf<["DispatchOp", "TaskOp"]>
  ]> {
  let summary = "Terminate and yield results of a dispatch or task op";

  let arguments = (ins Variadic<AnyType>:$results);
  let assemblyFormat = "$results attr-dict `:` type($results)";

  let builders = [
    OpBuilder<(ins), "build($_builder, $_state, std::nullopt);">
  ];
}

//===----------------------------------------------------------------------===//
// Stream Operations
//===----------------------------------------------------------------------===//

def StreamOp : HLSOp<"dataflow.stream", []> {
  let summary = "Declare a stream channel";
  let description = [{
    Declares a stream channel for communication between tasks.
    The depth parameter controls the FIFO depth.
  }];

  let arguments = (ins I32Attr:$depth);
  let results = (outs AnyStream:$channel);
  let assemblyFormat = "attr-dict `:` type($channel)";

  let builders = [
    OpBuilder<(ins "Type":$channelType), [{
      build($_builder, $_state, channelType, 1);
    }]>
  ];
}

def StreamReadOp : HLSOp<"dataflow.stream_read", []> {
  let summary = "Read from a stream channel";
  let description = [{
    Read/pop a value from a stream channel. This is a blocking operation.
  }];

  let arguments = (ins AnyStream:$channel);
  let results = (outs Optional<AnyType>:$result);
  let assemblyFormat = [{
    $channel attr-dict `:` functional-type($channel, $result)
  }];

  let hasVerifier = 1;
}

def StreamWriteOp : HLSOp<"dataflow.stream_write", []> {
  let summary = "Write to a stream channel";
  let description = [{
    Write/push a value to a stream channel.
  }];

  let arguments = (ins AnyStream:$channel, AnyType:$value);
  let assemblyFormat = [{
    $channel `,` $value attr-dict `:` type($channel) `,` type($value)
  }];

  let hasVerifier = 1;
}

//===----------------------------------------------------------------------===//
// Buffer Operations
//===----------------------------------------------------------------------===//

def BufferOp : HLSOp<"dataflow.buffer", []> {
  let summary = "Declare a local buffer";
  let description = [{
    Declares a local buffer for double-buffering or local storage.
  }];

  let arguments = (ins I32Attr:$depth,
                       OptionalAttr<TypedAttrInterface>:$init_value);
  let results = (outs AnyMemRef:$memref);
  let assemblyFormat = "attr-dict `:` type($memref)";

  let builders = [
    OpBuilder<(ins "Type":$memrefType), [{
      build($_builder, $_state, memrefType, 1, nullptr);
    }]>
  ];
}

#endif // SYSTOLIC_DIALECT_HLS_OPS_TD
