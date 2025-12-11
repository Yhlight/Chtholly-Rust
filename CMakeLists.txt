cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_executable(chtholly src/main.cpp)

target_link_libraries(chtholly PRIVATE
  LLVM::Core
  LLVM::Support
  LLVM::IRReader
  )
