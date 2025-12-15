# Analysis library - Polyhedral analysis for systolic arrays

add_mlir_library(SystolicAnalysis
  SpaceTimeAnalysis.cpp
  PolymerAnalysis.cpp

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

# Link Polymer libraries if available
if(SYSTOLIC_ENABLE_POLYMER AND DEFINED POLYGEIST_BUILD_DIR)
  target_link_libraries(SystolicAnalysis PRIVATE
    ${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a
    ${POLYGEIST_BUILD_DIR}/lib/libPolymerTargetISL.a
  )
  # Add compile definition to enable Polymer code
  target_compile_definitions(SystolicAnalysis PRIVATE SYSTOLIC_ENABLE_POLYMER=1)
  message(STATUS "Linked Polymer libraries to SystolicAnalysis")
endif()
