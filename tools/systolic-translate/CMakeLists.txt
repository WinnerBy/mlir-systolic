# systolic-translate - HLS C++ Translation Tool

get_property(dialect_libs GLOBAL PROPERTY MLIR_DIALECT_LIBS)

add_llvm_executable(systolic-translate
  systolic-translate.cpp
)

llvm_update_compile_flags(systolic-translate)

target_link_libraries(systolic-translate
  PRIVATE
  ${dialect_libs}
  SystolicHLSDialect
  SystolicAnalysis
  SystolicTransforms
  MLIRAffineDialect
  MLIRArithDialect
  MLIRFuncDialect
  MLIRMemRefDialect
  MLIRIR
  MLIRParser
  MLIRSupport
  MLIRTransformUtils
)

mlir_check_all_link_libraries(systolic-translate)

install(TARGETS systolic-translate DESTINATION bin)
