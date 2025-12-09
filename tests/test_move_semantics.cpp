#include "../src/lexer.h"
#include "../src/parser.h"
#include "../src/sema.h"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>

TEST(SemaTest, MoveSemantics)
{
    std::string source = "let a = 10; let b = a; let c = a;";
    Chtholly::Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    auto statements = parser.parse();

    // Redirect cerr to a stringstream to capture error messages
    std::stringstream buffer;
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());

    Chtholly::Sema sema;
    sema.analyze(statements);

    // Restore cerr
    std::cerr.rdbuf(old_cerr);

    std::string expected_error = "Error: Use of moved value 'a'.\n";
    ASSERT_EQ(buffer.str(), expected_error);
}
