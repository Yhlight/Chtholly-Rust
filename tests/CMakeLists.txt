cmake_minimum_required(VERSION 3.20)

project(ChthollyTests)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests test_lexer.cpp ../src/Lexer.cpp)
target_include_directories(chtholly_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

target_link_libraries(chtholly_tests
    PRIVATE GTest::gtest GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(chtholly_tests)
