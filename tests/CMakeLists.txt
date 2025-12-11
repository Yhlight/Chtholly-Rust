# Add the test executable for the lexer
add_executable(lexer_tests LexerTest.cpp)
target_link_libraries(lexer_tests PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(lexer_tests PRIVATE ../src)
target_sources(lexer_tests PRIVATE ../src/Lexer/Lexer.cpp)

# Add the test executable for the parser
add_executable(parser_tests ParserTest.cpp)
target_link_libraries(parser_tests PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(parser_tests PRIVATE ../src)
target_sources(parser_tests PRIVATE ../src/Lexer/Lexer.cpp ../src/Parser/Parser.cpp)

# Add the test executable for the symbol table
add_executable(symbol_table_tests SymbolTableTest.cpp)
target_link_libraries(symbol_table_tests PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(symbol_table_tests PRIVATE ../src)
target_sources(symbol_table_tests PRIVATE ../src/Semantic/SymbolTable.cpp)

# Add the tests to CTest
include(GoogleTest)
gtest_add_tests(TARGET lexer_tests)
gtest_add_tests(TARGET parser_tests)
gtest_add_tests(TARGET symbol_table_tests)
