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

TEST(ParserTest, IfStatement) {
    std::vector<Token> tokens = {
        {TokenType::IF, "if", 1},
        {TokenType::LEFT_PAREN, "(", 1},
        {TokenType::TRUE, "true", 1},
        {TokenType::RIGHT_PAREN, ")", 1},
        {TokenType::LEFT_BRACE, "{", 1},
        {TokenType::RIGHT_BRACE, "}", 1},
        {TokenType::END_OF_FILE, "", 1}
    };

    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);

    auto ifStmt = dynamic_cast<IfStmt*>(stmts[0].get());
    ASSERT_NE(ifStmt, nullptr);

    ASTPrinter printer;
    std::string result = printer.print(*stmts[0]);
    EXPECT_EQ(result, "(if true (block))");
}

TEST(ParserTest, WhileStatement) {
    std::vector<Token> tokens = {
        {TokenType::WHILE, "while", 1},
        {TokenType::LEFT_PAREN, "(", 1},
        {TokenType::TRUE, "true", 1},
        {TokenType::RIGHT_PAREN, ")", 1},
        {TokenType::LEFT_BRACE, "{", 1},
        {TokenType::RIGHT_BRACE, "}", 1},
        {TokenType::END_OF_FILE, "", 1}
    };

    Parser parser(tokens);
    auto stmts = parser.parse();
    ASSERT_EQ(stmts.size(), 1);

    auto whileStmt = dynamic_cast<WhileStmt*>(stmts[0].get());
    ASSERT_NE(whileStmt, nullptr);

    ASTPrinter printer;
    std::string result = printer.print(*stmts[0]);
    EXPECT_EQ(result, "(while true (block))");
}
