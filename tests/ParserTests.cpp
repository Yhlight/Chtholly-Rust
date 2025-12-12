#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"

using namespace Chtholly;

TEST(ParserTest, ParseLetStatement) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->getName(), "x");
    EXPECT_FALSE(var_decl->isMutable());

    auto* num_expr = dynamic_cast<const NumberExprAST*>(var_decl->getInit());
    ASSERT_NE(num_expr, nullptr);
    EXPECT_EQ(num_expr->getValue(), 10);
}

TEST(ParserTest, ParseLetMutStatement) {
    std::string source = "let mut y = 20;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->getName(), "y");
    EXPECT_TRUE(var_decl->isMutable());

    auto* num_expr = dynamic_cast<const NumberExprAST*>(var_decl->getInit());
    ASSERT_NE(num_expr, nullptr);
    EXPECT_EQ(num_expr->getValue(), 20);
}
