cmake_minimum_required(VERSION 3.20)

# Check if this is a unified build (as external LLVM project) or standalone build
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  # Standalone build: find MLIR/LLVM via find_package
  project(mlir-systolic LANGUAGES CXX C)
  
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  
  # Find MLIR and LLVM
  find_package(MLIR REQUIRED CONFIG)
  find_package(LLVM REQUIRED CONFIG)
  
  message(STATUS "Standalone build mode")
  message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
  message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
  
  set(LLVM_SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/Polygeist/llvm-project/llvm)
  
  list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
  list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
  
  include(TableGen)
  include(AddLLVM)
  include(AddMLIR)
  include(HandleLLVMOptions)
  
  include_directories(${LLVM_INCLUDE_DIRS})
  include_directories(${MLIR_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR}/include)
  include_directories(${PROJECT_BINARY_DIR}/include)
  
  link_directories(${LLVM_BUILD_LIBRARY_DIR})
  add_definitions(${LLVM_DEFINITIONS})
  
  set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})
  set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
  set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
else()
  # Unified build: as external LLVM project
  # LLVM/MLIR variables are already set by parent CMake
  message(STATUS "Unified build mode (as external LLVM project)")
  
  set(LLVM_SOURCE_DIR ${LLVM_MAIN_SRC_DIR})
  set(MLIR_MAIN_SRC_DIR ${LLVM_MAIN_SRC_DIR}/../mlir)
  set(MLIR_INCLUDE_DIRS ${MLIR_MAIN_SRC_DIR}/include)
  set(MLIR_CMAKE_DIR ${MLIR_MAIN_SRC_DIR}/cmake/modules)
  set(MLIR_TABLEGEN_EXE $<TARGET_FILE:mlir-tblgen>)
  set(MLIR_TABLEGEN_OUTPUT_DIR ${LLVM_BINARY_DIR}/tools/mlir/include)
  
  list(APPEND CMAKE_MODULE_PATH "${MLIR_CMAKE_DIR}")
  list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
  
  include(TableGen)
  include(AddLLVM)
  include(AddMLIR)
  include(HandleLLVMOptions)
  
  include_directories(SYSTEM ${MLIR_INCLUDE_DIR})
  include_directories(SYSTEM ${MLIR_TABLEGEN_OUTPUT_DIR})
  include_directories(${PROJECT_SOURCE_DIR}/include)
  include_directories(${PROJECT_BINARY_DIR}/include)
  
  set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})
  set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
  set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
endif()

#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------
option(SYSTOLIC_ENABLE_POLYMER "Enable Polymer integration for polyhedral analysis (REQUIRED)" ON)

#-------------------------------------------------------------------------------
# Polymer Integration (REQUIRED)
#-------------------------------------------------------------------------------
# Polymer is required for SystolicTransform pass - no fallback to heuristic methods
set(POLYMER_AVAILABLE FALSE)

if(SYSTOLIC_ENABLE_POLYMER)
  # Determine Polygeist source directory based on build mode
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Standalone build: Polygeist is in third_party
    set(POLYGEIST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/Polygeist)
  else()
    # Unified build: Polygeist is also an external project, find it relative to LLVM
    # In unified build, Polygeist should be in the same parent directory as mlir-systolic
    # or we can use LLVM_EXTERNAL_POLYGEIST_SOURCE_DIR if set
    if(DEFINED LLVM_EXTERNAL_POLYGEIST_SOURCE_DIR)
      set(POLYGEIST_SOURCE_DIR ${LLVM_EXTERNAL_POLYGEIST_SOURCE_DIR})
    else()
      # Try to find Polygeist relative to mlir-systolic source
      get_filename_component(MLIR_SYSTOLIC_PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
      set(POLYGEIST_SOURCE_DIR ${MLIR_SYSTOLIC_PARENT_DIR}/Polygeist)
      # If not found, try third_party
      if(NOT EXISTS ${POLYGEIST_SOURCE_DIR})
        set(POLYGEIST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Polygeist)
      endif()
    endif()
  endif()
  
  # Check if Polygeist submodule exists
  if(EXISTS ${POLYGEIST_SOURCE_DIR}/tools/polymer/include)
    message(STATUS "Found Polygeist submodule at: ${POLYGEIST_SOURCE_DIR}")
    
    # Add Polymer include directories
    include_directories(${POLYGEIST_SOURCE_DIR}/tools/polymer/include)
    message(STATUS "Added Polymer include directory: ${POLYGEIST_SOURCE_DIR}/tools/polymer/include")
    
    # Find Polygeist build directory
    # In unified build, Polygeist libraries are in the same build directory
    set(POLYGEIST_BUILD_DIR "")
    
    if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
      # Standalone build: use environment variable or relative path
      if(DEFINED ENV{POLYGEIST_BUILD})
        set(POLYGEIST_BUILD_DIR $ENV{POLYGEIST_BUILD})
        message(STATUS "Using POLYGEIST_BUILD from environment: ${POLYGEIST_BUILD_DIR}")
      elseif(DEFINED POLYGEIST_BUILD)
        set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD})
        message(STATUS "Using POLYGEIST_BUILD from CMake variable: ${POLYGEIST_BUILD_DIR}")
      elseif(EXISTS ${POLYGEIST_SOURCE_DIR}/build)
        set(POLYGEIST_BUILD_DIR ${POLYGEIST_SOURCE_DIR}/build)
        message(STATUS "Found Polygeist build directory at: ${POLYGEIST_BUILD_DIR}")
      endif()
    else()
      # Unified build: Polygeist libraries are in the same build directory as mlir-systolic
      set(POLYGEIST_BUILD_DIR ${CMAKE_BINARY_DIR})
      message(STATUS "Unified build: using same build directory for Polygeist: ${POLYGEIST_BUILD_DIR}")
    endif()
    
    # Verify build directory and check for Polymer libraries
    if(POLYGEIST_BUILD_DIR AND EXISTS ${POLYGEIST_BUILD_DIR})
      set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD_DIR} CACHE PATH "Polygeist build directory")
      
      # Required Polymer libraries
      set(POLYMER_SUPPORT_LIB "${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a")
      set(POLYMER_ISL_LIB "${POLYGEIST_BUILD_DIR}/lib/libPolymerTargetISL.a")
      set(POLYMER_TRANSFORMS_LIB "${POLYGEIST_BUILD_DIR}/lib/libPolymerTransforms.a")
      
      # In unified build, also check for target-based linking
      if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        # Try to find Polymer targets directly (if Polygeist is built as external project)
        if(TARGET PolymerSupport AND TARGET PolymerTargetISL AND TARGET PolymerTransforms)
          message(STATUS "Found Polymer targets in unified build")
          # We'll link to targets directly in CMakeLists.txt files
          set(POLYMER_AVAILABLE TRUE)
          add_compile_definitions(SYSTOLIC_ENABLE_POLYMER=1)
          message(STATUS "Polymer integration enabled (using targets)")
        endif()
      endif()
      
      # Check for library files (for standalone build or if targets not available)
      if(EXISTS ${POLYMER_SUPPORT_LIB} AND EXISTS ${POLYMER_ISL_LIB} AND EXISTS ${POLYMER_TRANSFORMS_LIB})
        message(STATUS "Found all required Polymer libraries:")
        message(STATUS "  - ${POLYMER_SUPPORT_LIB}")
        message(STATUS "  - ${POLYMER_ISL_LIB}")
        message(STATUS "  - ${POLYMER_TRANSFORMS_LIB}")
        
        # Add library directory
        link_directories(${POLYGEIST_BUILD_DIR}/lib)
        
        # Find GMP (required by Polymer)
        find_package(GMP REQUIRED)
        if(GMP_FOUND)
          message(STATUS "Found GMP: ${GMP_LIBRARIES}")
        endif()
        
        # Find ISL headers (from Polly in LLVM build)
        set(ISL_INCLUDE_CANDIDATES
          "${LLVM_BINARY_DIR}/tools/polly/lib/External/isl/include"
          "${MLIR_MAIN_SRC_DIR}/../polly/lib/External/isl/include"
          "${LLVM_MAIN_SRC_DIR}/tools/polly/lib/External/isl/include"
        )
        
        set(ISL_INCLUDE_DIR "")
        foreach(candidate ${ISL_INCLUDE_CANDIDATES})
          if(EXISTS ${candidate}/isl/isl.h)
            set(ISL_INCLUDE_DIR ${candidate})
            break()
          endif()
        endforeach()
        
        if(ISL_INCLUDE_DIR)
          include_directories(${ISL_INCLUDE_DIR})
          message(STATUS "Found ISL headers at: ${ISL_INCLUDE_DIR}")
        else()
          message(WARNING "ISL headers not found. Polymer may not compile correctly.")
        endif()
        
        # Enable Polymer
        add_compile_definitions(SYSTOLIC_ENABLE_POLYMER=1)
        set(POLYMER_AVAILABLE TRUE)
        message(STATUS "Polymer integration enabled successfully")
      elseif(NOT POLYMER_AVAILABLE)
        message(FATAL_ERROR "Required Polymer libraries not found:")
        if(NOT EXISTS ${POLYMER_SUPPORT_LIB})
          message(FATAL_ERROR "  Missing: ${POLYMER_SUPPORT_LIB}")
        endif()
        if(NOT EXISTS ${POLYMER_ISL_LIB})
          message(FATAL_ERROR "  Missing: ${POLYMER_ISL_LIB}")
        endif()
        if(NOT EXISTS ${POLYMER_TRANSFORMS_LIB})
          message(FATAL_ERROR "  Missing: ${POLYMER_TRANSFORMS_LIB}")
          message(FATAL_ERROR "  This library is REQUIRED for SystolicTransform pass")
          message(FATAL_ERROR "  Please build Polygeist with: ninja PolymerTransforms")
        endif()
        message(FATAL_ERROR "Please build Polygeist first. See docs/BUILD_STEPS.md")
      endif()
    else()
      message(FATAL_ERROR "Polygeist build directory not found.")
      message(FATAL_ERROR "Please build Polygeist first. See docs/BUILD_STEPS.md")
      if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        message(FATAL_ERROR "Then set POLYGEIST_BUILD environment variable:")
        message(FATAL_ERROR "  export POLYGEIST_BUILD=${POLYGEIST_SOURCE_DIR}/build")
      endif()
    endif()
  else()
    message(FATAL_ERROR "Polygeist submodule not found at: ${POLYGEIST_SOURCE_DIR}")
    message(FATAL_ERROR "Please initialize submodules: git submodule update --init --recursive")
  endif()
else()
  message(FATAL_ERROR "SYSTOLIC_ENABLE_POLYMER is OFF, but Polymer is REQUIRED for SystolicTransform")
endif()

# Export Polymer availability for subdirectories
set(POLYMER_AVAILABLE ${POLYMER_AVAILABLE} CACHE BOOL "Whether Polymer is available")

#-------------------------------------------------------------------------------
# Directory Setup
#-------------------------------------------------------------------------------
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(tools)

#-------------------------------------------------------------------------------
# Testing
#-------------------------------------------------------------------------------
enable_testing()
add_subdirectory(test)
