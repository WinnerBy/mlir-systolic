# Analysis library - Polyhedral analysis for systolic arrays

add_mlir_library(SystolicAnalysis
  SpaceTimeAnalysis.cpp

  ADDITIONAL_HEADER_DIRS
  ${PROJECT_SOURCE_DIR}/include/systolic/Analysis

  DEPENDS
  MLIRAffineDialect
  MLIRMemRefDialect

  LINK_LIBS PUBLIC
  MLIRAffineAnalysis
  MLIRAffineDialect
  MLIRAffineUtils
  MLIRMemRefDialect
  MLIRIR
  MLIRSupport
)
