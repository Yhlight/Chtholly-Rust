cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Common library
add_library(common INTERFACE)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src/common)

# AST library
add_library(ast)
target_sources(ast PRIVATE src/ast/ASTPrinter.cpp)
target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ast)
target_link_libraries(ast PUBLIC common)

# Lexer library
add_library(lexer)
target_sources(lexer PRIVATE src/lexer/Lexer.cpp)
target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer)
target_link_libraries(lexer PUBLIC common)

# Parser library
add_library(parser)
target_sources(parser PRIVATE src/parser/Parser.cpp)
target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/parser)
target_link_libraries(parser PUBLIC lexer ast)

# Main executable
add_executable(chtholly src/driver/main.cpp)
target_link_libraries(chtholly PRIVATE parser)

llvm_map_components_to_libnames(LLVM_LIBS core support)

target_include_directories(chtholly PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(chtholly PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(chtholly PRIVATE ${LLVM_LIBS})


# Testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests tests/LexerTest.cpp tests/LexerFeaturesTest.cpp tests/ParserTest.cpp tests/ParserFeaturesTest.cpp)
target_link_libraries(chtholly_tests PRIVATE parser gtest_main)

add_test(NAME ChthollyTests COMMAND chtholly_tests)
