# systolic-translate - HLS C++ Translation Tool

add_llvm_executable(systolic-translate
  systolic-translate.cpp
)

llvm_update_compile_flags(systolic-translate)

target_link_libraries(systolic-translate
  PRIVATE
  SystolicHLSDialect
  SystolicAnalysis
  SystolicTransforms
  # Core MLIR libraries
  MLIRAffineDialect
  MLIRAffineTransformOps
  MLIRArithDialect
  MLIRArithTransforms
  MLIRArithValueBoundsOpInterfaceImpl
  MLIRFuncDialect
  MLIRMemRefDialect
  MLIRIR
  MLIRParser
  MLIRSupport
  MLIRTransformUtils
  MLIRSCFDialect
)

# Note: mlir_check_all_link_libraries is disabled to avoid recursive dependency issues
# If linking errors occur, add missing libraries manually

install(TARGETS systolic-translate DESTINATION bin)
