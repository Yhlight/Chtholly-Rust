#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"

TEST(ParserTest, LetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);
    auto* stmt = dynamic_cast<VarDeclStmt*>(program[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name.lexeme, "x");
    EXPECT_FALSE(stmt->isMutable);
}

TEST(ParserTest, LetMutStatement) {
    std::string source = "let mut y = \"hello\";";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Program program = parser.parse();

    ASSERT_EQ(program.size(), 1);
    auto* stmt = dynamic_cast<VarDeclStmt*>(program[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->name.lexeme, "y");
    EXPECT_TRUE(stmt->isMutable);
}

TEST(ParserTest, MissingSemicolon) {
    std::string source = "let x = 10";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    EXPECT_THROW(parser.parse(), std::runtime_error);
}

TEST(ParserTest, InvalidStatement) {
    std::string source = "let x = 10; { 123; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();
    Parser parser(tokens);
    Program program = parser.parse();
    ASSERT_EQ(program.size(), 1);
}
