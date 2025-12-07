# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Add test files
add_executable(parser_tests test_parser.cpp)
add_executable(lexer_tests test_lexer.cpp)
add_executable(expression_parser_tests test_expression_parser.cpp)

# Link against GoogleTest
target_link_libraries(parser_tests GTest::gtest_main Chtholly-lib)
target_link_libraries(lexer_tests GTest::gtest_main Chtholly-lib)
target_link_libraries(expression_parser_tests GTest::gtest_main Chtholly-lib)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(parser_tests)
gtest_discover_tests(lexer_tests)
gtest_discover_tests(expression_parser_tests)
