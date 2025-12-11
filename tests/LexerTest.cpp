#include <gtest/gtest.h>
#include "Lexer.h"
#include "Token.h"

using namespace Chtholly;

TEST(LexerTest, TokenizeLetStatement)
{
    std::string source = "let a = 10;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.Tokenize();

    ASSERT_EQ(tokens.size(), 5);

    EXPECT_EQ(tokens[0].GetType(), TokenType::Let);
    EXPECT_EQ(tokens[0].GetValue(), "let");

    EXPECT_EQ(tokens[1].GetType(), TokenType::Identifier);
    EXPECT_EQ(tokens[1].GetValue(), "a");

    EXPECT_EQ(tokens[2].GetType(), TokenType::Equal);
    EXPECT_EQ(tokens[2].GetValue(), "=");

    EXPECT_EQ(tokens[3].GetType(), TokenType::Integer);
    EXPECT_EQ(tokens[3].GetValue(), "10");

    EXPECT_EQ(tokens[4].GetType(), TokenType::Semicolon);
    EXPECT_EQ(tokens[4].GetValue(), ";");
}
