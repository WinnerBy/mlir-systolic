# Translation library - HLS C++ emission

add_mlir_library(SystolicTranslation
  EmitHLSCpp.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/systolic/Translation

  DEPENDS
  MLIRSystolicHLSOpsIncGen
  MLIRSystolicHLSTypesIncGen
  MLIRSystolicHLSDialectIncGen

  LINK_LIBS PUBLIC
  MLIRIR
  MLIRFuncDialect
  MLIRAffineDialect
  MLIRMemRefDialect
  MLIRArithDialect
  MLIRSupport
  MLIRTranslateLib
  SystolicHLSDialect
)
