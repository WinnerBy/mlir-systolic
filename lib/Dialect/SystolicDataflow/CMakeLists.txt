# SystolicDataflow Dialect implementation

add_mlir_dialect_library(SystolicSystolicDataflowDialect
  SystolicDataflowDialect.cpp

  DEPENDS
  MLIRSystolicSystolicDataflowOpsIncGen
  MLIRSystolicSystolicDataflowDialectIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRDialect
  MLIRFuncDialect
  MLIRAffineDialect
  MLIRMemRefDialect
  MLIRArithDialect
)

