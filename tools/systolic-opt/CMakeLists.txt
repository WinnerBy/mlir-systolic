# systolic-opt - Main optimization tool

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)
get_property(conversion_libs GLOBAL PROPERTY MLIR_CONVERSION_LIBS)

add_llvm_executable(systolic-opt
  systolic-opt.cpp
)

llvm_update_compile_flags(systolic-opt)

target_link_libraries(systolic-opt
  PRIVATE
  ${dialect_libs}
  ${conversion_libs}
  SystolicAnalysis
  SystolicTransforms
  MLIRAffineDialect
  MLIRAffineAnalysis
  MLIRAffineUtils
  MLIRFuncDialect
  MLIRMemRefDialect
  MLIRIR
  MLIROptLib
  MLIRParser
  MLIRPass
  MLIRSupport
  MLIRTransformUtils
)

mlir_check_all_link_libraries(systolic-opt)
