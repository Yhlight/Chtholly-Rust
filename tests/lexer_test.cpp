#include <gtest/gtest.h>
#include "Lexer.h"

TEST(LexerTest, SimpleFunction) {
    std::string source = "fn main() { let x = 10; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::FN,
        TokenType::IDENTIFIER,
        TokenType::LPAREN,
        TokenType::RPAREN,
        TokenType::LBRACE,
        TokenType::LET,
        TokenType::IDENTIFIER,
        TokenType::EQUAL,
        TokenType::INTEGER,
        TokenType::SEMICOLON,
        TokenType::RBRACE,
        TokenType::END_OF_FILE,
    };

    ASSERT_EQ(tokens.size(), expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}

TEST(LexerTest, NumberAtEnd) {
    std::string source = "let x = 123.";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected_types = {
        TokenType::LET,
        TokenType::IDENTIFIER,
        TokenType::EQUAL,
        TokenType::INTEGER,
        TokenType::DOT,
        TokenType::END_OF_FILE,
    };

    ASSERT_EQ(tokens.size(), expected_types.size());

    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]);
    }
}
