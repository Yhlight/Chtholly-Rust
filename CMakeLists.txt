cmake_minimum_required(VERSION 3.20)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- GTest ---
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)


# Find LLVM
find_package(LLVM 18.1 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Create a static library for the compiler
add_library(chtholly_lib STATIC
    src/Lexer/Lexer.cpp
    src/Parser/Parser.cpp
    src/Semantic/SemanticAnalyzer.cpp
    src/Semantic/SymbolTable.cpp
)

# Publicly expose the include directory for the library
target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)


# Add executable
add_executable(chtholly src/main.cpp)

# Link the executable against our library and LLVM
target_link_libraries(chtholly PRIVATE chtholly_lib
  LLVMSupport
  LLVMCore
  LLVMIRReader
  LLVMBitWriter
  LLVMMC
  LLVMMCParser
  LLVMAsmParser
)


# --- Testing ---
enable_testing()

# Add the tests subdirectory. This is where we'll put the actual language tests.
add_subdirectory(tests)
