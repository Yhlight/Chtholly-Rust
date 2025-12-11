#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"

using namespace Chtholly;

TEST(ParserTest, ParseLetStatement)
{
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.Parse();

    ASSERT_EQ(statements.size(), 1);

    ASTPrinter printer;
    std::string result = printer.Print(*statements[0]);
    EXPECT_EQ(result, "(let a = 10)");
}
