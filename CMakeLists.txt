cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
message(STATUS "LLVM library dirs: ${LLVM_LIBRARY_DIRS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(chtholly src/main.cpp)

llvm_map_components_to_libnames(llvm_libs
    support
    core
    irreader
    codegen
    target
    ipo
    mc
    bitreader
    bitwriter
    analysis
    instcombine
    transformutils
    executionengine
    runtimedyld
    targetparser
    linker
)

target_link_libraries(chtholly PRIVATE ${llvm_libs})
