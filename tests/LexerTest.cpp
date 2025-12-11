#include <gtest/gtest.h>
#include "../src/Lexer/Lexer.h"
#include <vector>

TEST(LexerTest, SimpleTokenization) {
    std::string source = "let x = 5;";
    Lexer lexer(source);

    std::vector<Token> expected_tokens = {
        {TokenType::Let, "let"},
        {TokenType::Identifier, "x"},
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
