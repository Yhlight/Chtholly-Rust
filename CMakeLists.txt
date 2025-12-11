cmake_minimum_required(VERSION 3.16)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(llvm_libs Core Support Analysis)

# Main executable
add_executable(chtholly src/main.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp src/SemanticAnalyzer.cpp src/CodeGenerator.cpp)
target_include_directories(chtholly PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(chtholly PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(chtholly PRIVATE ${llvm_libs})

# Tests
add_executable(test_lexer tests/test_lexer.cpp src/Lexer.cpp)

add_executable(test_parser tests/test_parser.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp)

add_executable(test_semantic_analyzer tests/test_semantic_analyzer.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp src/SemanticAnalyzer.cpp)

add_executable(test_code_generator tests/test_code_generator.cpp src/Lexer.cpp src/Parser.cpp src/SymbolTable.cpp src/CodeGenerator.cpp)
target_include_directories(test_code_generator PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(test_code_generator PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(test_code_generator PRIVATE ${llvm_libs})
