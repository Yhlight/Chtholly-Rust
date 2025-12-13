#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"
#include "Type.h"

using namespace Chtholly;

TEST(ParserTest, ParseLetStatementWithType) {
    std::string source = "fn main(): void { let x: i32 = 10; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    ASSERT_NE(func, nullptr);

    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 1);

    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->getName(), "x");
    EXPECT_TRUE(var_decl->getType()->isIntegerTy());
    EXPECT_FALSE(var_decl->isMutable());

    auto* num_expr = dynamic_cast<const NumberExprAST*>(var_decl->getInit());
    ASSERT_NE(num_expr, nullptr);
    EXPECT_EQ(num_expr->getValue(), 10);
}

TEST(ParserTest, ReportsAndRecoversFromErrors) {
    std::string source =
        "fn main(): i32 {\n"
        "    let x: i32 = 10;\n"
        "    let y = 20 \n"      // Error 1: Missing semicolon
        "    let z: f64 = 30.0;\n"
        "    let a =; \n"         // Error 2: Missing expression after '='
        "";                      // Error 3: Missing '}'

    Lexer lexer(source);
    Parser parser(lexer);
    parser.parse();

    const auto& errors = parser.getErrors();
    ASSERT_EQ(errors.size(), 3);

    // Error 1: The parser expects a semicolon after '20', but gets 'let'
    EXPECT_TRUE(errors[0].find("Expected token Semicolon but got Let") != std::string::npos);
    // Error 2: The parser finds a semicolon where an expression is expected
    EXPECT_TRUE(errors[1].find("Unexpected token in expression: ;") != std::string::npos);
    // Error 3: The parser reaches EOF before finding the closing brace for the function
    EXPECT_TRUE(errors[2].find("Expected token RightBrace but got EndOfFile") != std::string::npos);
}

TEST(ParserTest, ParseBinaryExpression) {
    std::string source = "fn main(): void { let x: i32 = 1 + 2 * 3; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 1);

    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), TokenType::Plus);

    auto* lhs = dynamic_cast<const NumberExprAST*>(bin_expr->getLHS());
    ASSERT_NE(lhs, nullptr);
    EXPECT_EQ(lhs->getValue(), 1);

    auto* rhs = dynamic_cast<const BinaryExprAST*>(bin_expr->getRHS());
    ASSERT_NE(rhs, nullptr);
    EXPECT_EQ(rhs->getOp(), TokenType::Star);
}

TEST(ParserTest, ParseCallExpression) {
    std::string source = "fn main(): void { foo(1, 2); }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 1);

    auto* call_stmt = dynamic_cast<ExprStmtAST*>(body[0].get());
    ASSERT_NE(call_stmt, nullptr);

    auto* call_expr = dynamic_cast<CallExprAST*>(call_stmt->getExpr());
    ASSERT_NE(call_expr, nullptr);
    EXPECT_EQ(call_expr->getCallee(), "foo");

    auto& args = call_expr->getArgs();
    ASSERT_EQ(args.size(), 2);
}

TEST(ParserTest, ParseIfStatement) {
    std::string source = "fn main(): void { if (1 > 2) { let x: i32 = 1; } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 1);

    auto* if_stmt = dynamic_cast<IfStmtAST*>(body[0].get());
    ASSERT_NE(if_stmt, nullptr);

    auto* cond = dynamic_cast<const BinaryExprAST*>(if_stmt->getCond());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->getOp(), TokenType::Greater);

    const auto& then_block = if_stmt->getThen();
    ASSERT_EQ(then_block.size(), 1);
    auto* then_stmt = dynamic_cast<VarDeclStmtAST*>(then_block[0].get());
    ASSERT_NE(then_stmt, nullptr);
    EXPECT_EQ(then_stmt->getName(), "x");

    const auto& else_block = if_stmt->getElse();
    ASSERT_EQ(else_block.size(), 0);
}
