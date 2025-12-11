cmake_minimum_required(VERSION 3.13.4)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18 REQUIRED CONFIG)

include_directories(include)

add_executable(chtholly src/main.cpp)

llvm_map_components_to_libnames(llvm_libs core support)

target_link_libraries(chtholly ${llvm_libs})
