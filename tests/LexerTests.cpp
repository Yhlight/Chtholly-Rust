#include <gtest/gtest.h>
#include "Lexer.h"
#include <vector>

using namespace Chtholly;

TEST(LexerTest, SingleCharTokens) {
    std::string source = "+-*/%(){}[],.:;";
    Lexer lexer(source);

    std::vector<TokenType> expected_tokens = {
        TokenType::Plus, TokenType::Minus, TokenType::Star, TokenType::Slash,
        TokenType::Percent, TokenType::LeftParen, TokenType::RightParen,
        TokenType::LeftBrace, TokenType::RightBrace, TokenType::LeftBracket,
        TokenType::RightBracket, TokenType::Comma, TokenType::Dot,
        TokenType::Colon, TokenType::Semicolon, TokenType::EndOfFile
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}

TEST(LexerTest, MultiCharTokens) {
    std::string source = "== != <= >= && || << >> ++ --";
    Lexer lexer(source);

    std::vector<TokenType> expected_tokens = {
        TokenType::EqualEqual, TokenType::BangEqual, TokenType::LessEqual,
        TokenType::GreaterEqual, TokenType::AmpersandAmpersand, TokenType::PipePipe,
        TokenType::LeftShift, TokenType::RightShift, TokenType::PlusPlus,
        TokenType::MinusMinus, TokenType::EndOfFile
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}

TEST(LexerTest, Keywords) {
    std::string source = "fn let mut class";
    Lexer lexer(source);

    std::vector<TokenType> expected_tokens = {
        TokenType::Fn, TokenType::Let, TokenType::Mut, TokenType::Class,
        TokenType::EndOfFile
    };

    for (const auto& expected_token : expected_tokens) {
        Token token = lexer.nextToken();
        EXPECT_EQ(token.type, expected_token);
    }
}

TEST(LexerTest, Identifier) {
    std::string source = "my_var anothervar _test";
    Lexer lexer(source);

    Token token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::Identifier);
    EXPECT_EQ(token.value, "my_var");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::Identifier);
    EXPECT_EQ(token.value, "anothervar");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::Identifier);
    EXPECT_EQ(token.value, "_test");
}

TEST(LexerTest, Numbers) {
    std::string source = "123 45.67";
    Lexer lexer(source);

    Token token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::Integer);
    EXPECT_EQ(token.value, "123");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::Float);
    EXPECT_EQ(token.value, "45.67");
}

TEST(LexerTest, StringLiteral) {
    std::string source = "\"hello world\" \"with \\\"escapes\\\"\"";
    Lexer lexer(source);

    Token token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::String);
    EXPECT_EQ(token.value, "hello world");

    token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::String);
    EXPECT_EQ(token.value, "with \"escapes\"");
}

TEST(LexerTest, CharLiteral) {
    std::string source = "'a'";
    Lexer lexer(source);

    Token token = lexer.nextToken();
    EXPECT_EQ(token.type, TokenType::Char);
    EXPECT_EQ(token.value, "a");
}

TEST(LexerTest, PositionTracking) {
    std::string source = "let x = 10;\nlet y = 20;";
    Lexer lexer(source);

    Token token = lexer.nextToken(); // let
    EXPECT_EQ(token.line, 1);
    EXPECT_EQ(token.column, 1);

    token = lexer.nextToken(); // x
    EXPECT_EQ(token.line, 1);
    EXPECT_EQ(token.column, 5);

    token = lexer.nextToken(); // =
    EXPECT_EQ(token.line, 1);
    EXPECT_EQ(token.column, 7);

    token = lexer.nextToken(); // 10
    EXPECT_EQ(token.line, 1);
    EXPECT_EQ(token.column, 9);

    token = lexer.nextToken(); // ;
    EXPECT_EQ(token.line, 1);
    EXPECT_EQ(token.column, 11);

    token = lexer.nextToken(); // let
    EXPECT_EQ(token.line, 2);
    EXPECT_EQ(token.column, 1);
}
