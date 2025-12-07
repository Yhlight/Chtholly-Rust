cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 18.1.8 REQUIRED CONFIG)

# Add executable
add_executable(chtholly src/main.cpp)

# Link to the LLVM imported targets.
# This is the modern way to do it. It automatically handles include directories,
# definitions, and linking to the correct libraries.
target_link_libraries(chtholly PRIVATE
    LLVMCore
    LLVMSupport
)
