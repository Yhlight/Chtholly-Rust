cmake_minimum_required(VERSION 3.13.4)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

include_directories(include)

add_executable(chtholly src/main.cpp)

llvm_map_components_to_libnames(llvm_libs core support)

target_link_libraries(chtholly ${llvm_libs})

# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_subdirectory(tests)
