#include <gtest/gtest.h>
#include "parser.h"

TEST(ParserTest, SingleLineComment) {
    std::string code = "// This is a single-line comment";
    Parser parser(code);
    // The parser should not throw an exception for a valid comment.
    ASSERT_NO_THROW(parser.parse());
}

TEST(ParserTest, MultiLineComment) {
    std::string code = "/* This is a multi-line comment */";
    Parser parser(code);
    ASSERT_NO_THROW(parser.parse());
}

TEST(ParserTest, UnterminatedMultiLineComment) {
    std::string code = "/* This is an unterminated multi-line comment";
    Parser parser(code);
    ASSERT_THROW(parser.parse(), std::runtime_error);
}
