cmake_minimum_required(VERSION 3.20)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_CMAKE_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(chtholly src/main.cpp)

llvm_map_components_to_libnames(CHTHOLLY_LLVM_LIBS
    core
    irreader
    support
)

target_link_libraries(chtholly PRIVATE ${CHTHOLLY_LLVM_LIBS})
