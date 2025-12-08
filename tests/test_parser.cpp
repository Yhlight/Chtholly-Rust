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

TEST(ParserTest, FunctionDefinition) {
    std::string code = "fn add(a: i32, b: i32): i32 { let result = a + b; }";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<FunctionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->token.literal, "fn");
    EXPECT_EQ(stmt->name->value, "add");

    ASSERT_EQ(stmt->parameters.size(), 2);
    EXPECT_EQ(stmt->parameters[0].first->value, "a");
    EXPECT_EQ(stmt->parameters[0].second->name, "i32");
    EXPECT_EQ(stmt->parameters[1].first->value, "b");
    EXPECT_EQ(stmt->parameters[1].second->name, "i32");

    ASSERT_NE(stmt->return_type, nullptr);
    EXPECT_EQ(stmt->return_type->name, "i32");

    ASSERT_NE(stmt->body, nullptr);
    ASSERT_EQ(stmt->body->statements.size(), 1);
}

TEST(ParserTest, WhileStatement) {
    std::string code = "while (x < y) { x }";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<WhileStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto condition = dynamic_cast<InfixExpression*>(stmt->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->left->to_string(), "x");
    EXPECT_EQ(condition->token.literal, "<");
    EXPECT_EQ(condition->right->to_string(), "y");

    ASSERT_NE(stmt->body, nullptr);
    ASSERT_EQ(stmt->body->statements.size(), 1);

    auto body_stmt = dynamic_cast<ExpressionStatement*>(stmt->body->statements[0].get());
    ASSERT_NE(body_stmt, nullptr);

    EXPECT_EQ(body_stmt->expression->to_string(), "x");
}

TEST(ParserTest, ForStatement) {
    std::string code = "for (let i = 0; i < 10; i++) { x }";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<ForStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto init = dynamic_cast<VarDeclarationStatement*>(stmt->init.get());
    ASSERT_NE(init, nullptr);
    EXPECT_EQ(init->name->value, "i");
    EXPECT_EQ(init->value->to_string(), "0");

    auto cond = dynamic_cast<InfixExpression*>(stmt->condition.get());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->left->to_string(), "i");
    EXPECT_EQ(cond->token.literal, "<");
    EXPECT_EQ(cond->right->to_string(), "10");

    auto inc = dynamic_cast<PostfixExpression*>(stmt->increment.get());
    ASSERT_NE(inc, nullptr);
    EXPECT_EQ(inc->left->to_string(), "i");
    EXPECT_EQ(inc->token.literal, "++");

    ASSERT_NE(stmt->body, nullptr);
    ASSERT_EQ(stmt->body->statements.size(), 1);

    auto body_stmt = dynamic_cast<ExpressionStatement*>(stmt->body->statements[0].get());
    ASSERT_NE(body_stmt, nullptr);

    EXPECT_EQ(body_stmt->expression->to_string(), "x");
}
