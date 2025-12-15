# Transforms library - Systolic array transformations

add_mlir_library(SystolicTransforms
  SystolicTransform.cpp
  DataflowGeneration.cpp
  SystolicDataflowGeneration.cpp
  SystolicDataflowToHLS.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/systolic/Transforms

  DEPENDS
  MLIRSystolicHLSOpsIncGen
  MLIRSystolicHLSTypesIncGen
  MLIRSystolicHLSDialectIncGen
  MLIRSystolicSystolicDataflowOpsIncGen
  MLIRSystolicSystolicDataflowDialectIncGen
  MLIRAffineDialect
  MLIRArithDialect
  MLIRMemRefDialect
  MLIRSCFDialect

  LINK_LIBS PUBLIC
  SystolicAnalysis
  SystolicHLSDialect
  SystolicSystolicDataflowDialect
  MLIRAffineDialect
  MLIRAffineAnalysis
  MLIRAffineUtils
  MLIRArithDialect
  MLIRFuncDialect
  MLIRMemRefDialect
  MLIRSCFDialect
  MLIRVectorDialect
  MLIRIR
  MLIRPass
  MLIRSupport
  MLIRTransformUtils
)
