#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"

using namespace Chtholly;

TEST(ParserTest, ParseLetStatement) {
    std::string source = "fn main(): void { let x = 10; }";
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
    EXPECT_FALSE(var_decl->isMutable());

    auto* num_expr = dynamic_cast<const NumberExprAST*>(var_decl->getInit());
    ASSERT_NE(num_expr, nullptr);
    EXPECT_EQ(num_expr->getValue(), 10);
}

TEST(ParserTest, ParseFunction) {
    std::string source = "fn add(a: i32, b: i32): i32 { let x = a + b; }";
    Lexer lexer(source);
    Parser parser(lexer);

    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    ASSERT_NE(func, nullptr);

    auto* proto = func->getProto();
    ASSERT_NE(proto, nullptr);
    EXPECT_EQ(proto->getName(), "add");
    EXPECT_EQ(proto->getReturnType(), "i32");

    auto args = proto->getArgs();
    ASSERT_EQ(args.size(), 2);
    EXPECT_EQ(args[0].first, "a");
    EXPECT_EQ(args[0].second, "i32");
    EXPECT_EQ(args[1].first, "b");
    EXPECT_EQ(args[1].second, "i32");

    const auto& body = func->getBody();
    ASSERT_EQ(body.size(), 1);

    auto* var_decl = dynamic_cast<VarDeclStmtAST*>(body[0].get());
    ASSERT_NE(var_decl, nullptr);
    EXPECT_EQ(var_decl->getName(), "x");

    auto* bin_expr = dynamic_cast<const BinaryExprAST*>(var_decl->getInit());
    ASSERT_NE(bin_expr, nullptr);
    EXPECT_EQ(bin_expr->getOp(), '+');
}
