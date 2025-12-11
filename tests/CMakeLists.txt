# Add the test executable
add_executable(lexer_tests LexerTest.cpp)

# Link the test executable against GTest and the main project source
target_link_libraries(lexer_tests PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(lexer_tests PRIVATE ../src)

# Add the lexer source files to the test target
target_sources(lexer_tests PRIVATE ../src/Lexer/Lexer.cpp)

# Add the test to CTest
include(GoogleTest)
gtest_add_tests(TARGET lexer_tests)
