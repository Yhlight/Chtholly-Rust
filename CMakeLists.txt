cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

add_executable(chtholly src/main.cpp)

llvm_map_components_to_libnames(LLVM_LIBS core support)

target_include_directories(chtholly PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(chtholly PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(chtholly PRIVATE ${LLVM_LIBS})
