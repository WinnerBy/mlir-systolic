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
if(SYSTOLIC_ENABLE_POLYMER AND POLYMER_AVAILABLE)
  # Get Polygeist build directory from parent scope
  if(DEFINED POLYGEIST_BUILD_DIR)
    set(POLYGEIST_BUILD_DIR_VAR ${POLYGEIST_BUILD_DIR})
  elseif(DEFINED CACHE{POLYGEIST_BUILD_DIR})
    set(POLYGEIST_BUILD_DIR_VAR $CACHE{POLYGEIST_BUILD_DIR})
  elseif(DEFINED ENV{POLYGEIST_BUILD})
    set(POLYGEIST_BUILD_DIR_VAR $ENV{POLYGEIST_BUILD})
  endif()
  
  if(POLYGEIST_BUILD_DIR_VAR AND EXISTS ${POLYGEIST_BUILD_DIR_VAR})
    # Link Polymer libraries
    target_link_libraries(SystolicAnalysis PRIVATE
      ${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerSupport.a
      ${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerTargetISL.a
    )
    
    # Link GMP if found
    if(GMP_FOUND)
      target_link_libraries(SystolicAnalysis PRIVATE ${GMP_LIBRARIES})
    endif()
    
    # Link Polly libraries (Polymer depends on them)
    # These should be available through LLVM build
    if(TARGET PollyISL)
      target_link_libraries(SystolicAnalysis PRIVATE PollyISL)
    endif()
    if(TARGET Polly)
      target_link_libraries(SystolicAnalysis PRIVATE Polly)
    endif()
    
    # Add compile definition
    target_compile_definitions(SystolicAnalysis PRIVATE SYSTOLIC_ENABLE_POLYMER=1)
    
    message(STATUS "Linked Polymer libraries to SystolicAnalysis from: ${POLYGEIST_BUILD_DIR_VAR}")
  else()
    message(WARNING "Polymer enabled but POLYGEIST_BUILD_DIR not found")
    message(WARNING "Polymer libraries will not be linked")
  endif()
else()
  message(STATUS "Polymer integration disabled or not available")
endif()
