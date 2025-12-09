cmake_minimum_required(VERSION 3.10)
project(Chtholly)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(chtholly src/main.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp)
target_include_directories(chtholly PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()

add_executable(test_lexer tests/test_lexer.cpp src/lexer.cpp)
target_include_directories(test_lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_lexer COMMAND test_lexer)

add_executable(test_parser tests/test_parser.cpp src/lexer.cpp src/parser.cpp)
target_include_directories(test_parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
add_test(NAME test_parser COMMAND test_parser)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_lifetime tests/test_lifetime.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp)
target_include_directories(test_lifetime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(test_lifetime GTest::GTest GTest::Main)
add_test(NAME test_lifetime COMMAND test_lifetime)

add_executable(test_functions tests/test_functions.cpp src/lexer.cpp src/parser.cpp src/semantic_analyzer.cpp)
target_include_directories(test_functions PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(test_functions GTest::GTest GTest::Main)
add_test(NAME test_functions COMMAND test_functions)
