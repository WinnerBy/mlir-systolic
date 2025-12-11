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
  # Polymer is part of Polygeist, we need to find it
  if(DEFINED ENV{POLYGEIST_BUILD})
    set(POLYGEIST_BUILD_DIR $ENV{POLYGEIST_BUILD})
  elseif(DEFINED POLYGEIST_BUILD)
    set(POLYGEIST_BUILD_DIR ${POLYGEIST_BUILD})
  else()
    message(STATUS "POLYGEIST_BUILD not set, Polymer integration disabled")
    set(SYSTOLIC_ENABLE_POLYMER OFF)
  endif()
  
  if(SYSTOLIC_ENABLE_POLYMER)
    message(STATUS "Polymer integration enabled from: ${POLYGEIST_BUILD_DIR}")
    # Add Polymer include directories
    include_directories(${POLYGEIST_BUILD_DIR}/../tools/polymer/include)
    include_directories(${POLYGEIST_BUILD_DIR}/tools/polymer/include)
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
