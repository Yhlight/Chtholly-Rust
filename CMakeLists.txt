cmake_minimum_required(VERSION 3.14)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find LLVM
find_package(LLVM 18.1.8 REQUIRED CONFIG)

# Chtholly Library
add_library(chtholly_lib src/Lexer.cpp src/ASTPrinter.cpp src/Parser.cpp)

target_link_libraries(chtholly_lib PRIVATE
    LLVMCore
    LLVMSupport
)
target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Main Executable
add_executable(chtholly src/main.cpp)
target_link_libraries(chtholly PRIVATE chtholly_lib)

# Copy examples directory to build directory
file(COPY examples DESTINATION ${CMAKE_BINARY_DIR})

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(chtholly_tests tests/LexerTest.cpp tests/ParserTest.cpp)

target_link_libraries(chtholly_tests PRIVATE gtest_main chtholly_lib)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
