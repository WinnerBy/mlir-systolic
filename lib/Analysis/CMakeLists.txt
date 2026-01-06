# Analysis library - Polyhedral analysis for systolic arrays

add_mlir_library(SystolicAnalysis
  SpaceTimeAnalysis.cpp
  PolymerAnalysis.cpp
  WriteTimeReorderingAnalysis.cpp
  PolyhedralAccessAnalyzer.cpp
  LayoutOptimizer.cpp

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
  # In unified build, try to link to Polymer targets directly
  # Otherwise, link to static library files
  if(TARGET PolymerSupport AND TARGET PolymerTargetISL AND TARGET PolymerTransforms)
    # Unified build: link to CMake targets
    message(STATUS "Unified build: linking to Polymer CMake targets")
    target_link_libraries(SystolicAnalysis PRIVATE
      PolymerSupport
      PolymerTargetISL
      PolymerTransforms
    )
  else()
    # Standalone build: link to static library files
    # Get Polygeist build directory from parent scope
    if(DEFINED POLYGEIST_BUILD_DIR)
      set(POLYGEIST_BUILD_DIR_VAR ${POLYGEIST_BUILD_DIR})
    elseif(DEFINED CACHE{POLYGEIST_BUILD_DIR})
      set(POLYGEIST_BUILD_DIR_VAR $CACHE{POLYGEIST_BUILD_DIR})
    elseif(DEFINED ENV{POLYGEIST_BUILD})
      set(POLYGEIST_BUILD_DIR_VAR $ENV{POLYGEIST_BUILD})
    else()
      # In unified build, use current build directory
      set(POLYGEIST_BUILD_DIR_VAR ${CMAKE_BINARY_DIR})
    endif()
    
    if(POLYGEIST_BUILD_DIR_VAR AND EXISTS ${POLYGEIST_BUILD_DIR_VAR})
      # Link Polymer libraries
      # Note: ExtractScopStmt functions are in libPolymerTransforms.a
      set(POLYMER_TRANSFORMS_LIB "${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerTransforms.a")
      if(EXISTS ${POLYMER_TRANSFORMS_LIB})
        target_link_libraries(SystolicAnalysis PRIVATE
            ${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerSupport.a
            ${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerTargetISL.a
            ${POLYMER_TRANSFORMS_LIB}
        )
        message(STATUS "Linked Polymer libraries to SystolicAnalysis from: ${POLYGEIST_BUILD_DIR_VAR}")
      else()
        # Fallback: try without Transforms library (may not have extractScopStmt)
        target_link_libraries(SystolicAnalysis PRIVATE
            ${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerSupport.a
            ${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerTargetISL.a
        )
        message(WARNING "libPolymerTransforms.a not found, extractScopStmt functions may be unavailable")
      endif()
    else()
      message(WARNING "Polymer enabled but POLYGEIST_BUILD_DIR not found")
      message(WARNING "Polymer libraries will not be linked")
    endif()
  endif()
  
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
else()
  message(STATUS "Polymer integration disabled or not available")
endif()
