# HLS Dialect TableGen

set(LLVM_TARGET_DEFINITIONS Ops.td)
mlir_tablegen(Ops.h.inc -gen-op-decls)
mlir_tablegen(Ops.cpp.inc -gen-op-defs)
add_public_tablegen_target(MLIRSystolicHLSOpsIncGen)

set(LLVM_TARGET_DEFINITIONS Types.td)
mlir_tablegen(Types.h.inc -gen-typedef-decls)
mlir_tablegen(Types.cpp.inc -gen-typedef-defs)
add_public_tablegen_target(MLIRSystolicHLSTypesIncGen)

set(LLVM_TARGET_DEFINITIONS HLS.td)
mlir_tablegen(HLSDialect.h.inc -gen-dialect-decls)
mlir_tablegen(HLSDialect.cpp.inc -gen-dialect-defs)
add_public_tablegen_target(MLIRSystolicHLSDialectIncGen)

add_mlir_doc(Ops SystolicHLSOps Dialects/ -gen-op-doc)
