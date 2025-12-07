#include <gtest/gtest.h>
#include "parser.h"
#include "lexer.h"

TEST(ExpressionParserTest, IntegerLiteralExpression) {
    std::string code = "5;";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->expression.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 5);
    EXPECT_EQ(literal->token.literal, "5");
}

TEST(ExpressionParserTest, BooleanLiteralExpression) {
    std::vector<std::tuple<std::string, bool>> tests = {
        {"true;", true},
        {"false;", false},
    };

    for (const auto& test : tests) {
        Lexer lexer(std::get<0>(test));
        Parser parser(lexer);

        auto program = parser.parse_program();
        ASSERT_NE(program, nullptr);
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
        ASSERT_NE(stmt, nullptr);

        auto literal = dynamic_cast<BooleanLiteral*>(stmt->expression.get());
        ASSERT_NE(literal, nullptr);
        EXPECT_EQ(literal->value, std::get<1>(test));
    }
}

TEST(ExpressionParserTest, IfExpression) {
    std::string code = "if (x < y) { x }";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto exp = dynamic_cast<IfExpression*>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);

    auto condition = dynamic_cast<InfixExpression*>(exp->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->left->to_string(), "x");
    EXPECT_EQ(condition->token.literal, "<");
    EXPECT_EQ(condition->right->to_string(), "y");

    ASSERT_NE(exp->consequence, nullptr);
    ASSERT_EQ(exp->consequence->statements.size(), 1);

    auto consequence_stmt = dynamic_cast<ExpressionStatement*>(exp->consequence->statements[0].get());
    ASSERT_NE(consequence_stmt, nullptr);

    EXPECT_EQ(consequence_stmt->expression->to_string(), "x");

    EXPECT_EQ(exp->alternative, nullptr);
}

TEST(ExpressionParserTest, IfElseExpression) {
    std::string code = "if (x < y) { x } else { y }";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto exp = dynamic_cast<IfExpression*>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);

    auto condition = dynamic_cast<InfixExpression*>(exp->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->left->to_string(), "x");
    EXPECT_EQ(condition->token.literal, "<");
    EXPECT_EQ(condition->right->to_string(), "y");

    ASSERT_NE(exp->consequence, nullptr);
    ASSERT_EQ(exp->consequence->statements.size(), 1);

    auto consequence_stmt = dynamic_cast<ExpressionStatement*>(exp->consequence->statements[0].get());
    ASSERT_NE(consequence_stmt, nullptr);

    EXPECT_EQ(consequence_stmt->expression->to_string(), "x");

    ASSERT_NE(exp->alternative, nullptr);
    ASSERT_EQ(exp->alternative->statements.size(), 1);

    auto alternative_stmt = dynamic_cast<ExpressionStatement*>(exp->alternative->statements[0].get());
    ASSERT_NE(alternative_stmt, nullptr);

    EXPECT_EQ(alternative_stmt->expression->to_string(), "y");
}

TEST(ExpressionParserTest, PrefixExpressions) {
    std::vector<std::tuple<std::string, std::string, long long>> tests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

    for (const auto& test : tests) {
        Lexer lexer(std::get<0>(test));
        Parser parser(lexer);

        auto program = parser.parse_program();
        ASSERT_NE(program, nullptr);
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
        ASSERT_NE(stmt, nullptr);

        auto exp = dynamic_cast<PrefixExpression*>(stmt->expression.get());
        ASSERT_NE(exp, nullptr);
        EXPECT_EQ(exp->token.literal, std::get<1>(test));

        auto literal = dynamic_cast<IntegerLiteral*>(exp->right.get());
        ASSERT_NE(literal, nullptr);
        EXPECT_EQ(literal->value, std::get<2>(test));
    }
}

TEST(ExpressionParserTest, InfixExpressions) {
    std::vector<std::tuple<std::string, std::string, std::string, std::string>> tests = {
        {"5 + 5;", "5", "+", "5"},
        {"5 - 5;", "5", "-", "5"},
        {"5 * 5;", "5", "*", "5"},
        {"5 / 5;", "5", "/", "5"},
        {"5 > 5;", "5", ">", "5"},
        {"5 < 5;", "5", "<", "5"},
        {"5 == 5;", "5", "==", "5"},
        {"5 != 5;", "5", "!=", "5"},
        {"true == true", "true", "==", "true"},
        {"true != false", "true", "!=", "false"},
        {"false == false", "false", "==", "false"},
    };

    for (const auto& test : tests) {
        Lexer lexer(std::get<0>(test));
        Parser parser(lexer);

        auto program = parser.parse_program();
        ASSERT_NE(program, nullptr);
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = dynamic_cast<ExpressionStatement*>(program->statements[0].get());
        ASSERT_NE(stmt, nullptr);

        auto exp = dynamic_cast<InfixExpression*>(stmt->expression.get());
        ASSERT_NE(exp, nullptr);

        EXPECT_EQ(exp->left->to_string(), std::get<1>(test));
        EXPECT_EQ(exp->token.literal, std::get<2>(test));
        EXPECT_EQ(exp->right->to_string(), std::get<3>(test));
    }
}
