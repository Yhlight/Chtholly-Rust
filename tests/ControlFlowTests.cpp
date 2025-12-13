#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "AST.h"
#include "Type.h"
#include "CodeGenerator.h"
#include <fstream>
#include <string>
#include <vector>

using namespace Chtholly;

std::string readFile(const std::string& filepath) {
    std::ifstream ifs(filepath);
    return std::string((std::istreambuf_iterator<char>(ifs)),
                       (std::istreambuf_iterator<char>()));
}

TEST(ControlFlowTest, WhileStatement) {
    std::string source = readFile("tests/while_test.cns");
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 2);

    auto* while_stmt = dynamic_cast<WhileStmtAST*>(body[1].get());
    ASSERT_NE(while_stmt, nullptr);

    auto* cond = dynamic_cast<const BinaryExprAST*>(while_stmt->getCond());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->getOp(), TokenType::Less);

    const auto& while_body = while_stmt->getBody();
    ASSERT_EQ(while_body.size(), 1);
}

TEST(ControlFlowTest, ForStatement) {
    std::string source = readFile("tests/for_test.cns");
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 2);

    auto* for_stmt = dynamic_cast<ForStmtAST*>(body[1].get());
    ASSERT_NE(for_stmt, nullptr);

    auto* init = dynamic_cast<VarDeclStmtAST*>(for_stmt->getInit());
    ASSERT_NE(init, nullptr);

    auto* cond = dynamic_cast<const BinaryExprAST*>(for_stmt->getCond());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->getOp(), TokenType::Less);

    auto* incr = dynamic_cast<const AssignExprAST*>(for_stmt->getIncr());
    ASSERT_NE(incr, nullptr);

    const auto& for_body = for_stmt->getBody();
    ASSERT_EQ(for_body.size(), 1);
}

TEST(ControlFlowTest, DoWhileStatement) {
    std::string source = readFile("tests/do_while_test.cns");
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();

    ASSERT_EQ(functions.size(), 1);
    auto* func = functions[0].get();
    const auto& body = func->getBody().value();
    ASSERT_EQ(body.size(), 2);

    auto* do_while_stmt = dynamic_cast<DoWhileStmtAST*>(body[1].get());
    ASSERT_NE(do_while_stmt, nullptr);

    auto* cond = dynamic_cast<const BinaryExprAST*>(do_while_stmt->getCond());
    ASSERT_NE(cond, nullptr);
    EXPECT_EQ(cond->getOp(), TokenType::Less);

    const auto& do_while_body = do_while_stmt->getBody();
    ASSERT_EQ(do_while_body.size(), 1);
}

TEST(ControlFlowTest, EndToEndWhile) {
    std::string source = readFile("tests/while_test.cns");
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    CodeGenerator generator;
    for (auto& func : functions) {
        generator.generate(*func);
    }
    // TODO: Actually execute the generated code and check the result
}

TEST(ControlFlowTest, EndToEndFor) {
    std::string source = readFile("tests/for_test.cns");
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    CodeGenerator generator;
    for (auto& func : functions) {
        generator.generate(*func);
    }
    // TODO: Actually execute the generated code and check the result
}

TEST(ControlFlowTest, EndToEndDoWhile) {
    std::string source = readFile("tests/do_while_test.cns");
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    CodeGenerator generator;
    for (auto& func : functions) {
        generator.generate(*func);
    }
    // TODO: Actually execute the generated code and check the result
}
