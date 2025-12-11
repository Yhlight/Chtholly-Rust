add_executable(lexer_tests
    test_lexer.cpp
    ../src/Lexer.cpp
)

target_include_directories(lexer_tests PUBLIC ../include)

target_link_libraries(lexer_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(lexer_tests)

add_executable(parser_tests
    test_parser.cpp
    ../src/Lexer.cpp
    ../src/Parser.cpp
)

target_include_directories(parser_tests PUBLIC ../include)

target_link_libraries(parser_tests
    GTest::gtest_main
)

gtest_discover_tests(parser_tests)
