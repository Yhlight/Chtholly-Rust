#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST/ASTPrinter.h"
#include <vector>
#include <string>

using namespace Chtholly;

std::string parseAndPrint(const std::string& source) {
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    std::unique_ptr<Expr> expr = parser.parse();
    if (expr) {
        ASTPrinter printer;
        return printer.print(*expr);
    }
    return "Parse Error";
}

TEST(ParserTest, SimpleAddition) {
    std::string source = "1 + 2";
    std::string expected = "(+ 1 2)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, OperatorPrecedence) {
    std::string source = "1 + 2 * 3";
    std::string expected = "(+ 1 (* 2 3))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, Grouping) {
    std::string source = "(1 + 2) * 3";
    std::string expected = "(* (group (+ 1 2)) 3)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, UnaryOperator) {
    std::string source = "-1";
    std::string expected = "(- 1)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ComplexExpression) {
    std::string source = "(-1 + 2) * 3 / 4";
    std::string expected = "(/ (* (group (+ (- 1) 2)) 3) 4)";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ComparisonOperators) {
    std::string source = "1 < 2 == 3 > 4";
    std::string expected = "(== (< 1 2) (> 3 4))";
    EXPECT_EQ(parseAndPrint(source), expected);
}

TEST(ParserTest, ErrorHandling) {
    std::string source = "1 +";
    EXPECT_EQ(parseAndPrint(source), "Parse Error");
}
