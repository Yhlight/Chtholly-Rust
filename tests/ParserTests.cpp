#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"

using namespace Chtholly;

TEST(ParserTest, ParseLetStatementWithType) {
    std::string source = "fn main(): void { let x: i32 = 10; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    ASSERT_NE(func, nullptr);

    const auto& body = func->getBody();
    ASSERT_EQ(body.size(), 1);

    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->getName(), "x");
    EXPECT_EQ(var_decl->getType(), "i32");
    EXPECT_FALSE(var_decl->isMutable());

    auto* num_expr = dynamic_cast<const NumberExprAST*>(var_decl->getInit());
    ASSERT_NE(num_expr, nullptr);
    EXPECT_EQ(num_expr->getValue(), 10);
}

TEST(ParserTest, ParseSimpleBinaryExpression) {
    std::string source = "fn main(): void { let x: i32 = 5 + 2; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody();
    ASSERT_EQ(body.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), TokenType::Plus);

    auto* lhs = dynamic_cast<const NumberExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getValue(), 5);

    auto* rhs = dynamic_cast<const NumberExprAST*>(bin_expr->getRHS());
    ASSERT_NE(rhs, nullptr);
    EXPECT_EQ(rhs->getValue(), 2);
}

TEST(ParserTest, ParsePrecedence) {
    std::string source = "fn main(): void { let x: i32 = 5 + 2 * 3; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody();
    ASSERT_EQ(body.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), TokenType::Plus);

    auto* lhs = dynamic_cast<const NumberExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getValue(), 5);

    auto* rhs = dynamic_cast<const BinaryExprAST*>(bin_expr->getRHS());
    ASSERT_NE(rhs, nullptr);
    EXPECT_EQ(rhs->getOp(), TokenType::Star);

    auto* rhs_lhs = dynamic_cast<const NumberExprAST*>(rhs->getLHS());
    ASSERT_NE(rhs_lhs, nullptr);
    EXPECT_EQ(rhs_lhs->getValue(), 2);

    auto* rhs_rhs = dynamic_cast<const NumberExprAST*>(rhs->getRHS());
    ASSERT_NE(rhs_rhs, nullptr);
    EXPECT_EQ(rhs_rhs->getValue(), 3);
}

TEST(ParserTest, ParseParentheses) {
    std::string source = "fn main(): void { let x: i32 = (5 + 2) * 3; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody();
    ASSERT_EQ(body.size(), 1);
    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), TokenType::Star);

    auto* lhs = dynamic_cast<const BinaryExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getOp(), TokenType::Plus);

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
