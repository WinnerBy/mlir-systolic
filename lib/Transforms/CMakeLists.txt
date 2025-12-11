# Transforms library - Systolic array transformations

add_mlir_library(SystolicTransforms
  SystolicTransform.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/systolic/Transforms

  DEPENDS
  MLIRAffineDialect
  MLIRMemRefDialect

  LINK_LIBS PUBLIC
  SystolicAnalysis
  MLIRAffineDialect
  MLIRAffineAnalysis
  MLIRAffineUtils
  MLIRFuncDialect
  MLIRMemRefDialect
  MLIRIR
  MLIRPass
  MLIRSupport
  MLIRTransformUtils
)
