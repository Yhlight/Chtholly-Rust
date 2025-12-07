#include <gtest/gtest.h>
#include "Parser.h"
#include "ASTPrinter.h"
#include "Token.h"

using namespace Chtholly;

TEST(ParserTest, SimpleExpression) {
    std::vector<Token> tokens = {
        {TokenType::NUMBER, "1", 1},
        {TokenType::PLUS, "+", 1},
        {TokenType::NUMBER, "2", 1},
        {TokenType::SEMICOLON, ";", 1},
        {TokenType::END_OF_FILE, "", 1}
    };

    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);

    auto exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);

    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    EXPECT_EQ(result, "(+ 1.000000 2.000000)");
}

TEST(ParserTest, ParenthesizedExpression) {
    std::vector<Token> tokens = {
        {TokenType::LEFT_PAREN, "(", 1},
        {TokenType::NUMBER, "1", 1},
        {TokenType::PLUS, "+", 1},
        {TokenType::NUMBER, "2", 1},
        {TokenType::RIGHT_PAREN, ")", 1},
        {TokenType::STAR, "*", 1},
        {TokenType::NUMBER, "3", 1},
        {TokenType::SEMICOLON, ";", 1},
        {TokenType::END_OF_FILE, "", 1}
    };

    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);

    auto exprStmt = dynamic_cast<ExpressionStmt*>(stmts[0].get());
    ASSERT_NE(exprStmt, nullptr);

    ASTPrinter printer;
    std::string result = printer.print(*exprStmt->expression);
    EXPECT_EQ(result, "(* (group (+ 1.000000 2.000000)) 3.000000)");
}

TEST(ParserTest, VariableDeclaration) {
    std::vector<Token> tokens = {
        {TokenType::LET, "let", 1},
        {TokenType::IDENTIFIER, "a", 1},
        {TokenType::EQUAL, "=", 1},
        {TokenType::NUMBER, "10", 1},
        {TokenType::SEMICOLON, ";", 1},
        {TokenType::END_OF_FILE, "", 1}
    };

    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);

    auto varDeclStmt = dynamic_cast<VarDeclStmt*>(stmts[0].get());
    ASSERT_NE(varDeclStmt, nullptr);
    EXPECT_EQ(varDeclStmt->name.lexeme, "a");

    ASTPrinter printer;
    std::string result = printer.print(*varDeclStmt->initializer);
    EXPECT_EQ(result, "10.000000");
}
