#include <gtest/gtest.h>
#include "parser.h"
#include "lexer.h"

TEST(ParserTest, LetStatements) {
    std::string code = "let x: i32 = 5;";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<VarDeclarationStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->token.literal, "let");
    EXPECT_EQ(stmt->name->value, "x");
    EXPECT_FALSE(stmt->is_mutable);
    ASSERT_NE(stmt->type, nullptr);
    EXPECT_EQ(stmt->type->name, "i32");

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 5);
    EXPECT_EQ(literal->token.literal, "5");
}

TEST(ParserTest, MutStatements) {
    std::string code = "mut y: string = 10;";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<VarDeclarationStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->token.literal, "mut");
    EXPECT_EQ(stmt->name->value, "y");
    EXPECT_TRUE(stmt->is_mutable);
    ASSERT_NE(stmt->type, nullptr);
    EXPECT_EQ(stmt->type->name, "string");

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 10);
    EXPECT_EQ(literal->token.literal, "10");
}

TEST(ParserTest, MainFunction) {
    std::string code = "fn main(): i32 {}";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<FunctionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->token.literal, "fn");
    EXPECT_EQ(stmt->name->value, "main");
    ASSERT_NE(stmt->return_type, nullptr);
    EXPECT_EQ(stmt->return_type->name, "i32");
    ASSERT_NE(stmt->body, nullptr);
    EXPECT_EQ(stmt->body->statements.size(), 0);
}
