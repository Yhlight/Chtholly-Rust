#include <gtest/gtest.h>
#include "lexer.h"

TEST(LexerTest, NextToken) {
    std::string code = R"(
        let five = 5;
        // This is a comment
        let ten = 10;
        /*
         * This is a multi-line comment
         */
        let result = five + ten;
    )";
    Lexer lexer(code);

    std::vector<Token> expected_tokens = {
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "five"},
        {TokenType::ASSIGN, "="},
        {TokenType::INTEGER, "5"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "ten"},
        {TokenType::ASSIGN, "="},
        {TokenType::INTEGER, "10"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::LET, "let"},
        {TokenType::IDENTIFIER, "result"},
        {TokenType::ASSIGN, "="},
        {TokenType::IDENTIFIER, "five"},
        {TokenType::PLUS, "+"},
        {TokenType::IDENTIFIER, "ten"},
        {TokenType::SEMICOLON, ";"},
        {TokenType::END_OF_FILE, ""},
    };

    for (const auto& expected_token : expected_tokens) {
        Token actual_token = lexer.next_token();
        EXPECT_EQ(actual_token.type, expected_token.type);
        EXPECT_EQ(actual_token.literal, expected_token.literal);
    }
}
