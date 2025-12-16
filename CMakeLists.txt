cmake_minimum_required(VERSION 3.20)
project(mlir-systolic LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------
option(SYSTOLIC_ENABLE_POLYMER "Enable Polymer integration for polyhedral analysis" ON)

#-------------------------------------------------------------------------------
# MLIR/LLVM Configuration
#-------------------------------------------------------------------------------
find_package(MLIR REQUIRED CONFIG)
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Using MLIRConfig.cmake in: ${MLIR_DIR}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

set(LLVM_RUNTIME_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/bin)
set(LLVM_LIBRARY_OUTPUT_INTDIR ${CMAKE_BINARY_DIR}/lib)
set(MLIR_BINARY_DIR ${CMAKE_BINARY_DIR})

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

#-------------------------------------------------------------------------------
# Polymer Integration (Optional)
#-------------------------------------------------------------------------------
set(POLYMER_AVAILABLE FALSE)

if(SYSTOLIC_ENABLE_POLYMER)
  set(POLYGEIST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/Polygeist)
  
  # Check if Polygeist submodule exists
  if(EXISTS ${POLYGEIST_SOURCE_DIR}/tools/polymer/include)
    message(STATUS "Found Polygeist submodule at: ${POLYGEIST_SOURCE_DIR}")
    
    # Add Polymer include directories from source
    include_directories(${POLYGEIST_SOURCE_DIR}/tools/polymer/include)
    message(STATUS "Added Polymer include directory: ${POLYGEIST_SOURCE_DIR}/tools/polymer/include")
    
    # Try to find Polygeist build directory
    # Priority: 1. Environment variable, 2. CMake variable, 3. Relative paths
    set(POLYGEIST_BUILD_DIR "")
    
    if(DEFINED ENV{POLYGEIST_BUILD})
      set(POLYGEIST_BUILD_DIR $ENV{POLYGEIST_BUILD})
      message(STATUS "Using POLYGEIST_BUILD from environment: ${POLYGEIST_BUILD_DIR}")
    elseif(DEFINED POLYGEIST_BUILD)
      set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD})
      message(STATUS "Using POLYGEIST_BUILD from CMake variable: ${POLYGEIST_BUILD_DIR}")
    else()
      # Try common relative paths
      if(EXISTS ${POLYGEIST_SOURCE_DIR}/build)
        set(POLYGEIST_BUILD_DIR ${POLYGEIST_SOURCE_DIR}/build)
        message(STATUS "Found Polygeist build directory at: ${POLYGEIST_BUILD_DIR}")
      elseif(EXISTS ${POLYGEIST_SOURCE_DIR}/../Polygeist-build)
        set(POLYGEIST_BUILD_DIR ${POLYGEIST_SOURCE_DIR}/../Polygeist-build)
        message(STATUS "Found Polygeist build directory at: ${POLYGEIST_BUILD_DIR}")
      endif()
    endif()
    
    # Verify build directory and check for Polymer libraries
    if(POLYGEIST_BUILD_DIR AND EXISTS ${POLYGEIST_BUILD_DIR})
      set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD_DIR} CACHE PATH "Polygeist build directory")
      
      # Check for Polymer libraries
      set(POLYMER_SUPPORT_LIB "${POLYGEIST_BUILD_DIR}/lib/libPolymerSupport.a")
      set(POLYMER_ISL_LIB "${POLYGEIST_BUILD_DIR}/lib/libPolymerTargetISL.a")
      
      if(EXISTS ${POLYMER_SUPPORT_LIB} AND EXISTS ${POLYMER_ISL_LIB})
        message(STATUS "Found Polymer libraries:")
        message(STATUS "  - ${POLYMER_SUPPORT_LIB}")
        message(STATUS "  - ${POLYMER_ISL_LIB}")
        
        # Add library directory
        link_directories(${POLYGEIST_BUILD_DIR}/lib)
        
        # Find GMP (required by Polymer)
        find_package(GMP)
        if(GMP_FOUND)
          message(STATUS "Found GMP: ${GMP_LIBRARIES}")
        else()
          message(WARNING "GMP not found. Polymer may not work correctly.")
          message(WARNING "Install GMP (e.g., sudo apt-get install libgmp-dev)")
        endif()
        
        # Find ISL headers (from Polly in LLVM build)
        # Polymer expects ISL from Polly's External directory
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
          message(WARNING "ISL headers not found. Tried:")
          foreach(candidate ${ISL_INCLUDE_CANDIDATES})
            message(WARNING "  - ${candidate}")
          endforeach()
          message(WARNING "Polymer may not compile correctly without ISL headers.")
        endif()
        
        # Add compile definition (both global and export to subdirectories)
        add_compile_definitions(SYSTOLIC_ENABLE_POLYMER=1)
        set(SYSTOLIC_ENABLE_POLYMER ON CACHE BOOL "Enable Polymer" FORCE)
        set(POLYMER_AVAILABLE TRUE)
        message(STATUS "Polymer integration enabled successfully")
      else()
        message(WARNING "Polygeist build directory found but Polymer libraries not found:")
        message(WARNING "  Expected: ${POLYMER_SUPPORT_LIB}")
        message(WARNING "  Expected: ${POLYMER_ISL_LIB}")
        message(WARNING "Please build Polygeist with Polymer enabled:")
        message(WARNING "  cd third_party/Polygeist && mkdir build && cd build")
        message(WARNING "  cmake .. -DMLIR_DIR=... -DLLVM_DIR=... -DPOLYGEIST_POLYMER_ENABLE_ISL=ON -GNinja")
        message(WARNING "  ninja PolymerSupport PolymerTargetISL")
        set(SYSTOLIC_ENABLE_POLYMER OFF)
      endif()
    else()
      message(WARNING "Polygeist build directory not found.")
      message(WARNING "Please build Polygeist first:")
      message(WARNING "  cd third_party/Polygeist && mkdir build && cd build")
      message(WARNING "  cmake .. -DMLIR_DIR=${MLIR_DIR} -DLLVM_DIR=${LLVM_DIR} \\")
      message(WARNING "           -DPOLYGEIST_POLYMER_ENABLE_ISL=ON \\")
      message(WARNING "           -DPOLYGEIST_ENABLE_POLYMER=ON -GNinja")
      message(WARNING "  ninja PolymerSupport PolymerTargetISL")
      message(WARNING "Then set POLYGEIST_BUILD environment variable or CMake variable.")
      set(SYSTOLIC_ENABLE_POLYMER OFF)
    endif()
  else()
    message(STATUS "Polygeist submodule not found at: ${POLYGEIST_SOURCE_DIR}")
    message(STATUS "Polymer integration disabled")
    set(SYSTOLIC_ENABLE_POLYMER OFF)
  endif()
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
