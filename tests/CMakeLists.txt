include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(lexer_tests main.cpp lexer_test.cpp)
target_link_libraries(lexer_tests GTest::gtest_main chtholly_lib)

include(GoogleTest)
gtest_discover_tests(lexer_tests)
