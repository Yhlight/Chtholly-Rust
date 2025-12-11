#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"
#include <vector>

using namespace Chtholly;

// Helper to assert that a stream of tokens has the expected types in order.
void assertTokenTypes(const std::vector<Token>& tokens, const std::vector<TokenType>& expected_types) {
    ASSERT_EQ(tokens.size(), expected_types.size()) << "Mismatched number of tokens.";
    for (size_t i = 0; i < tokens.size(); ++i) {
        EXPECT_EQ(tokens[i].type, expected_types[i]) << "Token #" << i << " has wrong type.";
    }
}

TEST(LexerTest, SingleCharacterTokens) {
    std::string_view source = "(){}[];:,.";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::LeftParen, TokenType::RightParen,
        TokenType::LeftBrace, TokenType::RightBrace,
        TokenType::LeftBracket, TokenType::RightBracket,
        TokenType::Semicolon, TokenType::Colon,
        TokenType::Comma, TokenType::Dot,
        TokenType::Eof
    };
    assertTokenTypes(tokens, expected);
}

TEST(LexerTest, OperatorTokens) {
    std::string_view source = "+ += ++ - -= -- * *= / /= % %= ! != = == > >= < <= & && | || ^ ~ << >>";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::Plus, TokenType::PlusEqual, TokenType::PlusPlus,
        TokenType::Minus, TokenType::MinusEqual, TokenType::MinusMinus,
        TokenType::Star, TokenType::StarEqual,
        TokenType::Slash, TokenType::SlashEqual,
        TokenType::Percent, TokenType::PercentEqual,
        TokenType::Bang, TokenType::BangEqual,
        TokenType::Equal, TokenType::EqualEqual,
        TokenType::Greater, TokenType::GreaterEqual,
        TokenType::Less, TokenType::LessEqual,
        TokenType::Amp, TokenType::AmpAmp,
        TokenType::Pipe, TokenType::PipePipe,
        TokenType::Caret, TokenType::Tilde,
        TokenType::LessLess, TokenType::GreaterGreater,
        TokenType::Eof
    };
    assertTokenTypes(tokens, expected);
}

TEST(LexerTest, NumberLiterals) {
    std::string_view source = "123 45.67 8";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = { TokenType::Number, TokenType::Number, TokenType::Number, TokenType::Eof };
    assertTokenTypes(tokens, expected);

    EXPECT_EQ(tokens[0].lexeme, "123");
    EXPECT_EQ(tokens[1].lexeme, "45.67");
    EXPECT_EQ(tokens[2].lexeme, "8");
}

TEST(LexerTest, StringLiteral) {
    std::string_view source = "\"hello world\"";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = { TokenType::String, TokenType::Eof };
    assertTokenTypes(tokens, expected);
    EXPECT_EQ(tokens[0].lexeme, "\"hello world\"");
}

TEST(LexerTest, UnterminatedString) {
    std::string_view source = "\"hello";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    // Expect an Error token followed by EOF.
    std::vector<TokenType> expected = { TokenType::Error, TokenType::Eof };
    assertTokenTypes(tokens, expected);
    EXPECT_EQ(tokens[0].lexeme, "Unterminated string.");
}

TEST(LexerTest, IdentifiersAndKeywords) {
    std::string_view source = "let mut var = 10; fn if else";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::Let, TokenType::Mut, TokenType::Identifier,
        TokenType::Equal, TokenType::Number, TokenType::Semicolon,
        TokenType::Fn, TokenType::If, TokenType::Else,
        TokenType::Eof
    };
    assertTokenTypes(tokens, expected);
    EXPECT_EQ(tokens[2].lexeme, "var");
}

TEST(LexerTest, Comments) {
    std::string_view source = R"(
        // This is a comment.
        let x = 10; // Another comment
        /* This is a
         * block comment.
         */
        let y = 20;
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.scanTokens();

    std::vector<TokenType> expected = {
        TokenType::Let, TokenType::Identifier, TokenType::Equal, TokenType::Number, TokenType::Semicolon,
        TokenType::Let, TokenType::Identifier, TokenType::Equal, TokenType::Number, TokenType::Semicolon,
        TokenType::Eof
    };
    assertTokenTypes(tokens, expected);
    EXPECT_EQ(tokens[1].lexeme, "x");
    EXPECT_EQ(tokens[6].lexeme, "y");
}
