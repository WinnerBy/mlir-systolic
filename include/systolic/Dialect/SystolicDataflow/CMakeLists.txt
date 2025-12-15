# SystolicDataflow Dialect TableGen

set(LLVM_TARGET_DEFINITIONS SystolicDataflowOps.td)
mlir_tablegen(Ops.h.inc -gen-op-decls)
mlir_tablegen(Ops.cpp.inc -gen-op-defs)
add_public_tablegen_target(MLIRSystolicSystolicDataflowOpsIncGen)

set(LLVM_TARGET_DEFINITIONS SystolicDataflow.td)
mlir_tablegen(SystolicDataflowDialect.h.inc -gen-dialect-decls)
mlir_tablegen(SystolicDataflowDialect.cpp.inc -gen-dialect-defs)
add_public_tablegen_target(MLIRSystolicSystolicDataflowDialectIncGen)

add_mlir_doc(SystolicDataflowOps SystolicSystolicDataflowOps Dialects/ -gen-op-doc)

