cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

target_link_libraries(chtholly PRIVATE
    LLVMCore
    LLVMSupport
    LLVMAnalysis
)
