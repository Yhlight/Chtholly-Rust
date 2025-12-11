# Add the test executable for the lexer
add_executable(lexer_tests LexerTest.cpp)
target_link_libraries(lexer_tests PRIVATE GTest::gtest GTest::gtest_main chtholly_lib)

# Add the test executable for the parser
add_executable(parser_tests ParserTest.cpp)
target_link_libraries(parser_tests PRIVATE GTest::gtest GTest::gtest_main chtholly_lib)

# Add the test executable for the symbol table
add_executable(symbol_table_tests SymbolTableTest.cpp)
target_link_libraries(symbol_table_tests PRIVATE GTest::gtest GTest::gtest_main chtholly_lib)

# Add the test executable for the semantic analyzer
add_executable(semantic_analyzer_tests SemanticAnalyzerTest.cpp)
target_link_libraries(semantic_analyzer_tests PRIVATE GTest::gtest GTest::gtest_main chtholly_lib)

# Add the tests to CTest
include(GoogleTest)
gtest_add_tests(TARGET lexer_tests)
gtest_add_tests(TARGET parser_tests)
gtest_add_tests(TARGET symbol_table_tests)
gtest_add_tests(TARGET semantic_analyzer_tests)
