#include <gtest/gtest.h>
#include "Interpreter.h"
#include "Parser.h"
#include "Lexer.h"

TEST(InterpreterTest, VarDeclaration) {
    std::string source = "let x = 10;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(program);

    Object value = interpreter.environment->get("x");
    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 10);
}

TEST(InterpreterTest, SimpleAddition) {
    std::string source = "let x = 1 + 2;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(program);

    Object value = interpreter.environment->get("x");
    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 3);
}

TEST(InterpreterTest, OperatorPrecedence) {
    std::string source = "let x = 1 + 2 * 3;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(program);

    Object value = interpreter.environment->get("x");
    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 7);
}

TEST(InterpreterTest, Grouping) {
    std::string source = "let x = (1 + 2) * 3;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(program);

    Object value = interpreter.environment->get("x");
    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), 9);
}

TEST(InterpreterTest, UnaryOperator) {
    std::string source = "let x = -1;";
    Lexer lexer(source);
    auto tokens = lexer.scanTokens();
    Parser parser(tokens);
    auto program = parser.parse();
    Interpreter interpreter;
    interpreter.interpret(program);

    Object value = interpreter.environment->get("x");
    ASSERT_TRUE(std::holds_alternative<int>(value));
    EXPECT_EQ(std::get<int>(value), -1);
}
