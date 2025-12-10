cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtholly src/main.cpp src/lexer.cpp src/parser.cpp src/symbol_table.cpp src/semantic_analyzer.cpp)
target_include_directories(chtholly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()

add_executable(test_lexer tests/test_lexer.cpp src/lexer.cpp)
target_include_directories(test_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_lexer COMMAND test_lexer)

add_executable(test_parser tests/test_parser.cpp src/lexer.cpp src/parser.cpp)
target_include_directories(test_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_parser COMMAND test_parser)

add_executable(test_semantic_analyzer tests/test_semantic_analyzer.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_semantic_analyzer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_semantic_analyzer COMMAND test_semantic_analyzer)

add_executable(test_functions tests/test_functions.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_functions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_functions COMMAND test_functions)

add_executable(test_control_flow tests/test_control_flow.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_control_flow PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_control_flow COMMAND test_control_flow)

add_executable(test_switch_statement tests/test_switch_statement.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_switch_statement PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_switch_statement COMMAND test_switch_statement)

add_executable(test_structs tests/test_structs.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_structs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_structs COMMAND test_structs)

add_executable(test_classes tests/test_classes.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_classes PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_classes COMMAND test_classes)

add_executable(test_enums tests/test_enums.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp src/symbol_table.cpp)
target_include_directories(test_enums PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_enums COMMAND test_enums)
