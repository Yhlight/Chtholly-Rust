#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"

// A helper function to get the expression from a VarDeclStmt
Expr* getInitializer(Stmt* stmt) {
    if (auto* varDecl = dynamic_cast<VarDeclStmt*>(stmt)) {
        return varDecl->initializer.get();
    }
    return nullptr;
}

TEST(ExpressionTest, SimpleAddition) {
    std::string source = "let x = 1 + 2;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program.size(), 1);
    auto* expr = getInitializer(program[0].get());
    ASSERT_NE(expr, nullptr);
    auto* binaryExpr = dynamic_cast<BinaryExpr*>(expr);
    ASSERT_NE(binaryExpr, nullptr);
    EXPECT_EQ(binaryExpr->op.type, TokenType::PLUS);
}

TEST(ExpressionTest, OperatorPrecedence) {
    std::string source = "let x = 1 + 2 * 3;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program.size(), 1);
    auto* expr = getInitializer(program[0].get());
    ASSERT_NE(expr, nullptr);

    auto* topExpr = dynamic_cast<BinaryExpr*>(expr);
    ASSERT_NE(topExpr, nullptr);
    EXPECT_EQ(topExpr->op.type, TokenType::PLUS);

    auto* rightExpr = dynamic_cast<BinaryExpr*>(topExpr->right.get());
    ASSERT_NE(rightExpr, nullptr);
    EXPECT_EQ(rightExpr->op.type, TokenType::STAR);
}

TEST(ExpressionTest, Grouping) {
    std::string source = "let x = (1 + 2) * 3;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program.size(), 1);
    auto* expr = getInitializer(program[0].get());
    ASSERT_NE(expr, nullptr);

    auto* topExpr = dynamic_cast<BinaryExpr*>(expr);
    ASSERT_NE(topExpr, nullptr);
    EXPECT_EQ(topExpr->op.type, TokenType::STAR);

    auto* leftExpr = dynamic_cast<GroupingExpr*>(topExpr->left.get());
    ASSERT_NE(leftExpr, nullptr);

    auto* groupedExpr = dynamic_cast<BinaryExpr*>(leftExpr->expression.get());
    ASSERT_NE(groupedExpr, nullptr);
    EXPECT_EQ(groupedExpr->op.type, TokenType::PLUS);
}

TEST(ExpressionTest, UnaryOperator) {
    std::string source = "let x = -1;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();

    ASSERT_EQ(program.size(), 1);
    auto* expr = getInitializer(program[0].get());
    ASSERT_NE(expr, nullptr);

    auto* unaryExpr = dynamic_cast<UnaryExpr*>(expr);
    ASSERT_NE(unaryExpr, nullptr);
    EXPECT_EQ(unaryExpr->op.type, TokenType::MINUS);
}
