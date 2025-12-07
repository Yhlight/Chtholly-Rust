#include <gtest/gtest.h>
#include "lexer.h"

TEST(LexerTest, NextToken) {
    std::string code = "let five = 5;";
    Lexer lexer(code);

    std::vector<Token> expected_tokens = {
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "five"},
        {TokenType::ASSIGN, "="},
        {TokenType::INTEGER, "5"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::END_OF_FILE, ""},
    };

    for (const auto& expected_token : expected_tokens) {
        Token actual_token = lexer.next_token();
        EXPECT_EQ(actual_token.type, expected_token.type);
        EXPECT_EQ(actual_token.literal, expected_token.literal);
    }
}
