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

# Link Polymer Transforms library if available (required for ExtractScopStmt)
if(SYSTOLIC_ENABLE_POLYMER AND POLYMER_AVAILABLE)
  # In unified build, try to link to Polymer target directly
  # Otherwise, link to static library file
  if(TARGET PolymerTransforms)
    # Unified build: link to CMake target
    message(STATUS "Unified build: linking to PolymerTransforms CMake target")
    target_link_libraries(SystolicTransforms PRIVATE PolymerTransforms)
  else()
    # Standalone build: link to static library file
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
      # Try different possible library names
      # add_mlir_conversion_library might generate libMLIRPolymerTransforms.a
      set(POLYMER_TRANSFORMS_LIB_CANDIDATES
        "${POLYGEIST_BUILD_DIR_VAR}/lib/libPolymerTransforms.a"
        "${POLYGEIST_BUILD_DIR_VAR}/lib/libMLIRPolymerTransforms.a"
        "${POLYGEIST_BUILD_DIR_VAR}/tools/polymer/lib/libPolymerTransforms.a"
      )
      
      set(POLYMER_TRANSFORMS_LIB_FOUND FALSE)
      foreach(candidate ${POLYMER_TRANSFORMS_LIB_CANDIDATES})
        if(EXISTS ${candidate})
          target_link_libraries(SystolicTransforms PRIVATE ${candidate})
          message(STATUS "Linked Polymer Transforms library to SystolicTransforms: ${candidate}")
          set(POLYMER_TRANSFORMS_LIB_FOUND TRUE)
          break()
        endif()
      endforeach()
      
      if(NOT POLYMER_TRANSFORMS_LIB_FOUND)
        message(WARNING "Polymer Transforms library not found. Tried:")
        foreach(candidate ${POLYMER_TRANSFORMS_LIB_CANDIDATES})
          message(WARNING "  - ${candidate}")
        endforeach()
        message(WARNING "Please rebuild Polygeist with: ninja PolymerTransforms")
        message(FATAL_ERROR "Polymer Transforms library is REQUIRED for SystolicTransform")
      endif()
    else()
      message(WARNING "POLYGEIST_BUILD_DIR not found. Tried:")
      message(WARNING "  - POLYGEIST_BUILD_DIR variable: ${POLYGEIST_BUILD_DIR}")
      message(WARNING "  - POLYGEIST_BUILD environment variable: $ENV{POLYGEIST_BUILD}")
      message(FATAL_ERROR "Polymer Transforms library is REQUIRED for SystolicTransform")
    endif()
  endif()
endif()
