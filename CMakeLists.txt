cmake_minimum_required(VERSION 3.20)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 18.1 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Add executable
add_executable(chtholly src/main.cpp)

# Add LLVM include directories and definitions
target_include_directories(chtholly PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(chtholly PRIVATE ${LLVM_DEFINITIONS})

# Link against LLVM libraries
target_link_libraries(chtholly PRIVATE
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

# Add a simple test to ensure the executable runs and exits with a zero status
add_test(NAME ChthollyRuns COMMAND chtholly)

# Add the tests subdirectory. This is where we'll put the actual language tests.
add_subdirectory(tests)
