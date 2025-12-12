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

TEST(ParserTest, ParseSimpleBinaryExpression) {
    std::string source = "let x = 5 + 2;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), '+');

    auto* lhs = dynamic_cast<const NumberExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getValue(), 5);

    auto* rhs = dynamic_cast<const NumberExprAST*>(bin_expr->getRHS());
    ASSERT_NE(rhs, nullptr);
    EXPECT_EQ(rhs->getValue(), 2);
}

TEST(ParserTest, ParsePrecedence) {
    std::string source = "let x = 5 + 2 * 3;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), '+');

    auto* lhs = dynamic_cast<const NumberExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getValue(), 5);

    auto* rhs = dynamic_cast<const BinaryExprAST*>(bin_expr->getRHS());
    ASSERT_NE(rhs, nullptr);
    EXPECT_EQ(rhs->getOp(), '*');

    auto* rhs_lhs = dynamic_cast<const NumberExprAST*>(rhs->getLHS());
    ASSERT_NE(rhs_lhs, nullptr);
    EXPECT_EQ(rhs_lhs->getValue(), 2);

    auto* rhs_rhs = dynamic_cast<const NumberExprAST*>(rhs->getRHS());
    ASSERT_NE(rhs_rhs, nullptr);
    EXPECT_EQ(rhs_rhs->getValue(), 3);
}

TEST(ParserTest, ParseParentheses) {
    std::string source = "let x = (5 + 2) * 3;";
    Lexer lexer(source);
    Parser parser(lexer);

    auto statements = parser.parse();

    ASSERT_EQ(statements.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(statements[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), '*');

    auto* lhs = dynamic_cast<const BinaryExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getOp(), '+');

    auto* lhs_lhs = dynamic_cast<const NumberExprAST*>(lhs->getLHS());
    ASSERT_NE(lhs_lhs, nullptr);
    EXPECT_EQ(lhs_lhs->getValue(), 5);

    auto* lhs_rhs = dynamic_cast<const NumberExprAST*>(lhs->getRHS());
    ASSERT_NE(lhs_rhs, nullptr);
    EXPECT_EQ(lhs_rhs->getValue(), 2);

    auto* rhs = dynamic_cast<const NumberExprAST*>(bin_expr->getRHS());
    ASSERT_NE(rhs, nullptr);
    EXPECT_EQ(rhs->getValue(), 3);
}
