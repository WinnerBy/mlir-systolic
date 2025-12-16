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
if(SYSTOLIC_ENABLE_POLYMER)
  # Polymer is part of Polygeist
  # First, try to use Polygeist as a submodule (preferred)
  set(POLYGEIST_SOURCE_DIR ${PROJECT_SOURCE_DIR}/third_party/Polygeist)
  if(EXISTS ${POLYGEIST_SOURCE_DIR}/tools/polymer/include)
    message(STATUS "Using Polygeist from submodule: ${POLYGEIST_SOURCE_DIR}")
    # Add Polymer include directories from source
    include_directories(${POLYGEIST_SOURCE_DIR}/tools/polymer/include)
    
    # Try to find Polygeist build directory
    # Check common build directory locations
    if(DEFINED ENV{POLYGEIST_BUILD})
      set(POLYGEIST_BUILD_DIR $ENV{POLYGEIST_BUILD})
    elseif(DEFINED POLYGEIST_BUILD)
      set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD})
    else()
      # Try to find build directory relative to source
      if(EXISTS ${POLYGEIST_SOURCE_DIR}/../Polygeist-build)
        set(POLYGEIST_BUILD_DIR ${POLYGEIST_SOURCE_DIR}/../Polygeist-build)
      elseif(EXISTS ${POLYGEIST_SOURCE_DIR}/build)
        set(POLYGEIST_BUILD_DIR ${POLYGEIST_SOURCE_DIR}/build)
      endif()
    endif()
    
    # If build directory is found, add library paths
    if(DEFINED POLYGEIST_BUILD_DIR AND EXISTS ${POLYGEIST_BUILD_DIR})
      message(STATUS "Polygeist build directory: ${POLYGEIST_BUILD_DIR}")
      link_directories(${POLYGEIST_BUILD_DIR}/lib)
    else()
      message(WARNING "Polygeist build directory not found. "
                      "Set POLYGEIST_BUILD environment variable or "
                      "POLYGEIST_BUILD CMake variable to enable Polymer libraries.")
    endif()
  else()
    # Fallback: try to use external Polygeist (via POLYGEIST_BUILD)
    if(DEFINED ENV{POLYGEIST_BUILD})
      set(POLYGEIST_BUILD_DIR $ENV{POLYGEIST_BUILD})
    elseif(DEFINED POLYGEIST_BUILD)
      set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD})
    else()
      message(STATUS "Polygeist submodule not found and POLYGEIST_BUILD not set, "
                     "Polymer integration disabled")
      set(SYSTOLIC_ENABLE_POLYMER OFF)
    endif()
    
    if(SYSTOLIC_ENABLE_POLYMER)
      message(STATUS "Using external Polygeist from: ${POLYGEIST_BUILD_DIR}")
      # Add Polymer include directories
      include_directories(${POLYGEIST_BUILD_DIR}/../tools/polymer/include)
      include_directories(${POLYGEIST_BUILD_DIR}/tools/polymer/include)
      link_directories(${POLYGEIST_BUILD_DIR}/lib)
    endif()
  endif()
endif()

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
