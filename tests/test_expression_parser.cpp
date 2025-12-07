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
    std::vector<std::tuple<std::string, long long, std::string, long long>> tests = {
        {"5 + 5;", 5, "+", 5},
        {"5 - 5;", 5, "-", 5},
        {"5 * 5;", 5, "*", 5},
        {"5 / 5;", 5, "/", 5},
        {"5 > 5;", 5, ">", 5},
        {"5 < 5;", 5, "<", 5},
        {"5 == 5;", 5, "==", 5},
        {"5 != 5;", 5, "!=", 5},
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

        auto left_literal = dynamic_cast<IntegerLiteral*>(exp->left.get());
        ASSERT_NE(left_literal, nullptr);
        EXPECT_EQ(left_literal->value, std::get<1>(test));

        EXPECT_EQ(exp->token.literal, std::get<2>(test));

        auto right_literal = dynamic_cast<IntegerLiteral*>(exp->right.get());
        ASSERT_NE(right_literal, nullptr);
        EXPECT_EQ(right_literal->value, std::get<3>(test));
    }
}
