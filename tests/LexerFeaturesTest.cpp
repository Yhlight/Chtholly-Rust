#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"

using namespace Chtholly;

TEST(LexerFeaturesTest, TokenizeComments)
{
    std::string source = R"(
// This is a single-line comment.
let a = 10; // Another comment.
/* This is a multi-line
   comment. */
let b = 20;
)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 10);

    EXPECT_EQ(tokens[0].GetType(), TokenType::Let);
    EXPECT_EQ(tokens[1].GetType(), TokenType::Identifier);
    EXPECT_EQ(tokens[2].GetType(), TokenType::Equal);
    EXPECT_EQ(tokens[3].GetType(), TokenType::Integer);
    EXPECT_EQ(tokens[4].GetType(), TokenType::Semicolon);

    EXPECT_EQ(tokens[5].GetType(), TokenType::Let);
    EXPECT_EQ(tokens[6].GetType(), TokenType::Identifier);
    EXPECT_EQ(tokens[7].GetType(), TokenType::Equal);
    EXPECT_EQ(tokens[8].GetType(), TokenType::Integer);
    EXPECT_EQ(tokens[9].GetType(), TokenType::Semicolon);
}

TEST(LexerFeaturesTest, TokenizeOperators)
{
    std::string source = "== != <= >= += -= *= /= && ||";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].GetType(), TokenType::DoubleEqual);
    EXPECT_EQ(tokens[1].GetType(), TokenType::BangEqual);
    EXPECT_EQ(tokens[2].GetType(), TokenType::LessEqual);
    EXPECT_EQ(tokens[3].GetType(), TokenType::GreaterEqual);
    EXPECT_EQ(tokens[4].GetType(), TokenType::PlusEqual);
    EXPECT_EQ(tokens[5].GetType(), TokenType::MinusEqual);
    EXPECT_EQ(tokens[6].GetType(), TokenType::AsteriskEqual);
    EXPECT_EQ(tokens[7].GetType(), TokenType::SlashEqual);
    EXPECT_EQ(tokens[8].GetType(), TokenType::DoubleAmpersand);
    EXPECT_EQ(tokens[9].GetType(), TokenType::DoublePipe);
}

TEST(LexerFeaturesTest, TokenizeLiterals)
{
    std::string source = R"("hello" 123.45)";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 2);

    EXPECT_EQ(tokens[0].GetType(), TokenType::String);
    EXPECT_EQ(tokens[0].GetValue(), "hello");

    EXPECT_EQ(tokens[1].GetType(), TokenType::Float);
    EXPECT_EQ(tokens[1].GetValue(), "123.45");
}

TEST(LexerFeaturesTest, TokenizeValidCharLiteral)
{
    std::string source = "'c'";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].GetType(), TokenType::Char);
    EXPECT_EQ(tokens[0].GetValue(), "c");
}

TEST(LexerFeaturesTest, TokenizeEmptyCharLiteral)
{
    std::string source = "''";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].GetType(), TokenType::Unknown);
}

TEST(LexerFeaturesTest, TokenizeMultiCharLiteral)
{
    std::string source = "'ab'";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].GetType(), TokenType::Unknown);
}

TEST(LexerFeaturesTest, TokenizeUnterminatedCharLiteral)
{
    std::string source = "'a";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].GetType(), TokenType::Unknown);
}
