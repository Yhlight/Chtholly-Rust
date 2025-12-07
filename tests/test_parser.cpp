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

    auto stmt = dynamic_cast<LetStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);
    EXPECT_EQ(stmt->token.literal, "let");
    EXPECT_EQ(stmt->name->value, "x");

    auto literal = dynamic_cast<IntegerLiteral*>(stmt->value.get());
    ASSERT_NE(literal, nullptr);
    EXPECT_EQ(literal->value, 5);
    EXPECT_EQ(literal->token.literal, "5");
}
