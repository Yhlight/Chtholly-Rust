#include <gtest/gtest.h>
#include "parser.h"
#include "lexer.h"

TEST(ParserTest, LetStatements) {
    std::string code = "let x = 5;";
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

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 5);
    EXPECT_EQ(literal->token.literal, "5");
}

TEST(ParserTest, MutStatements) {
    std::string code = "mut y = 10;";
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

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 10);
    EXPECT_EQ(literal->token.literal, "10");
}

TEST(ParserTest, MultipleVarDeclarationStatements) {
    std::string code = R"(
        let x = 5;
        mut y = 10;
        let foobar = 838383;
    )";
    Lexer lexer(code);
    Parser parser(lexer);

    auto program = parser.parse_program();
    ASSERT_NE(program, nullptr);
    ASSERT_EQ(program->statements.size(), 3);

    const std::vector<std::tuple<std::string, std::string, bool, long long>> expected = {
        {"let", "x", false, 5},
        {"mut", "y", true, 10},
        {"let", "foobar", false, 838383},
    };

    for (size_t i = 0; i < expected.size(); ++i) {
        auto stmt = dynamic_cast<VarDeclarationStatement*>(program->statements[i].get());
        ASSERT_NE(stmt, nullptr);
        EXPECT_EQ(stmt->token.literal, std::get<0>(expected[i]));
        EXPECT_EQ(stmt->name->value, std::get<1>(expected[i]));
        EXPECT_EQ(stmt->is_mutable, std::get<2>(expected[i]));

        auto literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());
        ASSERT_NE(literal, nullptr);
        EXPECT_EQ(literal->value, std::get<3>(expected[i]));
    }
}
