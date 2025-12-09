include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

add_executable(parser_tests main.cpp parser_test.cpp)
target_link_libraries(parser_tests GTest::gtest_main chtholly_lib)
gtest_discover_tests(parser_tests)

add_executable(lexer_tests main.cpp lexer_test.cpp)
target_link_libraries(lexer_tests GTest::gtest_main chtholly_lib)
gtest_discover_tests(lexer_tests)
