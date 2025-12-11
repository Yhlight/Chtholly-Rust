#include <gtest/gtest.h>
#include "../src/Lexer/Lexer.h"
#include <vector>

TEST(LexerTest, SimpleTokenization) {
    std::string source = "let x: i32 = 5;";
    Lexer lexer(source);

    std::vector<Token> expected_tokens = {
        {TokenType::Let, "let"},
        {TokenType::Identifier, "x"},
        {TokenType::Colon, ":"},
        {TokenType::Identifier, "i32"},
        {TokenType::Assign, "="},
        {TokenType::Integer, "5"},
        {TokenType::Semicolon, ";"},
        {TokenType::Eof, ""}
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token.type);
        EXPECT_EQ(token.value, expected_token.value);
    }
}

TEST(LexerTest, FloatTokenization) {
    std::string source = "let y: f64 = 123.456;";
    Lexer lexer(source);

    std::vector<Token> expected_tokens = {
        {TokenType::Let, "let"},
        {TokenType::Identifier, "y"},
        {TokenType::Colon, ":"},
        {TokenType::Identifier, "f64"},
        {TokenType::Assign, "="},
        {TokenType::Float, "123.456"},
        {TokenType::Semicolon, ";"},
        {TokenType::Eof, ""}
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token.type);
        EXPECT_EQ(token.value, expected_token.value);
    }
}
