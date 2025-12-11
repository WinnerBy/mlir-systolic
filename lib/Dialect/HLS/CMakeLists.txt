# HLS Dialect implementation

add_mlir_dialect_library(SystolicHLSDialect
  HLSDialect.cpp

  DEPENDS
  MLIRSystolicHLSOpsIncGen
  MLIRSystolicHLSTypesIncGen
  MLIRSystolicHLSDialectIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRDialect
  MLIRFuncDialect
  MLIRAffineDialect
)
