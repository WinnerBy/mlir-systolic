# Translation library - HLS C++ emission

add_mlir_library(SystolicTranslation
  EmitHLSCpp.cpp

  DEPENDS
  MLIRSystolicIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRFuncDialect
  MLIRAffineDialect
  MLIRMemRefDialect
  MLIRArithDialect
  MLIRSupport
  MLIRTranslateLib
  SystolicDialect
)
