#include <gtest/gtest.h>
#include "Lexer.h"

using namespace Chtholly;

TEST(LexerTest, TokenizesKeywords) {
    std::string source = "fn let mut class struct";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::Fn);
    EXPECT_EQ(tokens[1].type, TokenType::Let);
    EXPECT_EQ(tokens[2].type, TokenType::Mut);
    EXPECT_EQ(tokens[3].type, TokenType::Class);
    EXPECT_EQ(tokens[4].type, TokenType::Struct);
    EXPECT_EQ(tokens[5].type, TokenType::Eof);
}

TEST(LexerTest, TokenizesIdentifiers) {
    std::string source = "my_var anotherVar _test";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::Identifier);
    EXPECT_EQ(tokens[0].value, "my_var");
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);
    EXPECT_EQ(tokens[1].value, "anotherVar");
    EXPECT_EQ(tokens[2].type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].value, "_test");
    EXPECT_EQ(tokens[3].type, TokenType::Eof);
}

TEST(LexerTest, TokenizesNumbers) {
    std::string source = "123 45.67";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::Integer);
    EXPECT_EQ(tokens[0].value, "123");
    EXPECT_EQ(tokens[1].type, TokenType::Float);
    EXPECT_EQ(tokens[1].value, "45.67");
    EXPECT_EQ(tokens[2].type, TokenType::Eof);
}

TEST(LexerTest, TokenizesStringsAndChars) {
    std::string source = "\"hello\" 'c'";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, TokenType::StringLiteral);
    EXPECT_EQ(tokens[0].value, "hello");
    EXPECT_EQ(tokens[1].type, TokenType::Character);
    EXPECT_EQ(tokens[1].value, "c");
    EXPECT_EQ(tokens[2].type, TokenType::Eof);
}

TEST(LexerTest, TokenizesOperators) {
    std::string source = "+ - * / % = == != < > <= >= && || ! ++ --";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 18);
    EXPECT_EQ(tokens[0].type, TokenType::Plus);
    EXPECT_EQ(tokens[1].type, TokenType::Minus);
    EXPECT_EQ(tokens[2].type, TokenType::Star);
    EXPECT_EQ(tokens[3].type, TokenType::Slash);
    EXPECT_EQ(tokens[4].type, TokenType::Percent);
    EXPECT_EQ(tokens[5].type, TokenType::Equal);
    EXPECT_EQ(tokens[6].type, TokenType::EqualEqual);
    EXPECT_EQ(tokens[7].type, TokenType::NotEqual);
    EXPECT_EQ(tokens[8].type, TokenType::Less);
    EXPECT_EQ(tokens[9].type, TokenType::Greater);
    EXPECT_EQ(tokens[10].type, TokenType::LessEqual);
    EXPECT_EQ(tokens[11].type, TokenType::GreaterEqual);
    EXPECT_EQ(tokens[12].type, TokenType::AmpAmp);
    EXPECT_EQ(tokens[13].type, TokenType::PipePipe);
    EXPECT_EQ(tokens[14].type, TokenType::Not);
    EXPECT_EQ(tokens[15].type, TokenType::PlusPlus);
    EXPECT_EQ(tokens[16].type, TokenType::MinusMinus);
}

TEST(LexerTest, SkipsComments) {
    std::string source = R"(
        // This is a single line comment
        let x = 10; /* This is a
                       multi-line comment */
        let y = 20;
    )";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    ASSERT_EQ(tokens.size(), 11);
    EXPECT_EQ(tokens[0].type, TokenType::Let);
    EXPECT_EQ(tokens[1].type, TokenType::Identifier);
    EXPECT_EQ(tokens[2].type, TokenType::Equal);
    EXPECT_EQ(tokens[3].type, TokenType::Integer);
    EXPECT_EQ(tokens[4].type, TokenType::Semicolon);
    EXPECT_EQ(tokens[5].type, TokenType::Let);
    EXPECT_EQ(tokens[6].type, TokenType::Identifier);
    EXPECT_EQ(tokens[7].type, TokenType::Equal);
    EXPECT_EQ(tokens[8].type, TokenType::Integer);
    EXPECT_EQ(tokens[9].type, TokenType::Semicolon);
    EXPECT_EQ(tokens[10].type, TokenType::Eof);
}
