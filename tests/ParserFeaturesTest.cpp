#include <gtest/gtest.h>
#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"

using namespace Chtholly;

TEST(ParserFeaturesTest, ParseLetMutStatement)
{
    std::string source = "let mut a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.Parse();

    ASSERT_EQ(statements.size(), 1);

    ASTPrinter printer;
    std::string result = printer.Print(*statements[0]);
    EXPECT_EQ(result, "(let mut a = 10)");
}

TEST(ParserFeaturesTest, ParseBinaryExpression)
{
    std::string source = "5 + 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.Parse();

    ASSERT_EQ(statements.size(), 1);

    ASTPrinter printer;
    std::string result = printer.Print(*statements[0]);
    EXPECT_EQ(result, "(expr (+ 5 10))");
}

TEST(ParserFeaturesTest, ParseBinaryExpressionWithPrecedence)
{
    std::string source = "5 + 2 * 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.Parse();

    ASSERT_EQ(statements.size(), 1);

    ASTPrinter printer;
    std::string result = printer.Print(*statements[0]);
    EXPECT_EQ(result, "(expr (+ 5 (* 2 10)))");
}

TEST(ParserFeaturesTest, ParseUninitializedLet)
{
    std::string source = "let x;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    ASSERT_THROW(parser.Parse(), std::runtime_error);
}

TEST(ParserFeaturesTest, ParseUnaryExpression)
{
    std::string source = "-10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.Parse();

    ASSERT_EQ(statements.size(), 1);

    ASTPrinter printer;
    std::string result = printer.Print(*statements[0]);
    EXPECT_EQ(result, "(expr (- 10))");
}

TEST(ParserFeaturesTest, ParseParenthesizedExpression)
{
    std::string source = "(5 + 10);";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> statements = parser.Parse();

    ASSERT_EQ(statements.size(), 1);

    ASTPrinter printer;
    std::string result = printer.Print(*statements[0]);
    EXPECT_EQ(result, "(expr (group (+ 5 10)))");
}

TEST(ParserFeaturesTest, ParseInvalidExpression)
{
    std::string source = ";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();
    Parser parser(tokens);
    ASSERT_THROW(parser.Parse(), std::runtime_error);
}
