#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"

using namespace Chtholly;

TEST(LexerTest, SingleCharacterTokens) {
    std::string source = "(){}[],.-+;*/%";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 15);
    EXPECT_EQ(tokens[0].type, TokenType::LEFT_PAREN);
    EXPECT_EQ(tokens[1].type, TokenType::RIGHT_PAREN);
    EXPECT_EQ(tokens[2].type, TokenType::LEFT_BRACE);
    EXPECT_EQ(tokens[3].type, TokenType::RIGHT_BRACE);
    EXPECT_EQ(tokens[4].type, TokenType::LEFT_BRACKET);
    EXPECT_EQ(tokens[5].type, TokenType::RIGHT_BRACKET);
    EXPECT_EQ(tokens[6].type, TokenType::COMMA);
    EXPECT_EQ(tokens[7].type, TokenType::DOT);
    EXPECT_EQ(tokens[8].type, TokenType::MINUS);
    EXPECT_EQ(tokens[9].type, TokenType::PLUS);
    EXPECT_EQ(tokens[10].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[11].type, TokenType::STAR);
    EXPECT_EQ(tokens[12].type, TokenType::SLASH);
    EXPECT_EQ(tokens[13].type, TokenType::MODULO);
    EXPECT_EQ(tokens[14].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, DoubleCharacterTokens) {
    std::string source = "! != = == > >= < <= += -= *= /= %= ++ --";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 16);
    EXPECT_EQ(tokens[0].type, TokenType::BANG);
    EXPECT_EQ(tokens[1].type, TokenType::BANG_EQUAL);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::EQUAL_EQUAL);
    EXPECT_EQ(tokens[4].type, TokenType::GREATER);
    EXPECT_EQ(tokens[5].type, TokenType::GREATER_EQUAL);
    EXPECT_EQ(tokens[6].type, TokenType::LESS);
    EXPECT_EQ(tokens[7].type, TokenType::LESS_EQUAL);
    EXPECT_EQ(tokens[8].type, TokenType::PLUS_EQUAL);
    EXPECT_EQ(tokens[9].type, TokenType::MINUS_EQUAL);
    EXPECT_EQ(tokens[10].type, TokenType::STAR_EQUAL);
    EXPECT_EQ(tokens[11].type, TokenType::SLASH_EQUAL);
    EXPECT_EQ(tokens[12].type, TokenType::MODULO_EQUAL);
    EXPECT_EQ(tokens[13].type, TokenType::PLUS_PLUS);
    EXPECT_EQ(tokens[14].type, TokenType::MINUS_MINUS);
    EXPECT_EQ(tokens[15].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, StringLiteral) {
    std::string source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "\"hello world\"");
}

TEST(LexerTest, NumberLiteral) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[1].lexeme, "45.67");
}

TEST(LexerTest, IdentifierAndKeywords) {
    std::string source = "let mut var = fn";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::MUT);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].lexeme, "var");
    EXPECT_EQ(tokens[3].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[4].type, TokenType::FN);
    EXPECT_EQ(tokens[5].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, Comments) {
    std::string source = "// this is a comment\nlet x = 10; /* block comment */";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::LET);
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[2].type, TokenType::EQUAL);
    EXPECT_EQ(tokens[3].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[4].type, TokenType::SEMICOLON);
    EXPECT_EQ(tokens[5].type, TokenType::END_OF_FILE);
}
