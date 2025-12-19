# systolic-opt - Main optimization tool

add_llvm_executable(systolic-opt
  systolic-opt.cpp
)

llvm_update_compile_flags(systolic-opt)

target_link_libraries(systolic-opt
  PRIVATE
  SystolicAnalysis
  SystolicTransforms
  SystolicSystolicDataflowDialect
  # Core MLIR libraries
  MLIRAffineDialect
  MLIRAffineAnalysis
  MLIRAffineUtils
  MLIRAffineTransformOps
  MLIRArithDialect
  MLIRArithTransforms
  MLIRArithValueBoundsOpInterfaceImpl
  MLIRFuncDialect
  MLIRMemRefDialect
  MLIRIR
  MLIROptLib
  MLIRParser
  MLIRPass
  MLIRSupport
  MLIRTransformUtils
  MLIRTransformDialect
  MLIRSCFDialect
  MLIRVectorDialect
)

# Note: mlir_check_all_link_libraries is disabled to avoid recursive dependency issues
# If linking errors occur, add missing libraries manually
