#include <gtest/gtest.h>
#include "Parser.h"
#include "Lexer.h"
#include "SemanticAnalyzer.h"

using namespace Chtholly;

TEST(SemanticAnalyzerTest, ValidFunction) {
    std::string source = "fn main(): void { let x: i32 = 10; let y: i32 = x; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, UndeclaredVariable) {
    std::string source = "fn main(): void { let x: i32 = y; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, UndeclaredFunction) {
    std::string source = "fn main(): void { foo(); }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidWhileStatement) {
    std::string source = "fn main(): void { let x: bool = true; while (x) {} }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, InvalidWhileStatementCondition) {
    std::string source = "fn main(): void { let x: i32 = 1; while (x) {} }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidForStatement) {
    std::string source = "fn main(): void { for (let i: i32 = 0; i < 10; i = i + 1) {} }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, InvalidForStatementCondition) {
    std::string source = "fn main(): void { for (let i: i32 = 0; 1; i = i + 1) {} }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidDoWhileStatement) {
    std::string source = "fn main(): void { let x: bool = true; do {} while (x); }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, InvalidDoWhileStatementCondition) {
    std::string source = "fn main(): void { let x: i32 = 1; do {} while (x); }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidSwitchStatement) {
    std::string source = "fn main(): void { let x: i32 = 1; switch (x) { case 1: {} } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, InvalidSwitchStatementCondition) {
    std::string source = "fn main(): void { let x: bool = true; switch (x) { case true: {} } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, InvalidSwitchCaseType) {
    std::string source = "fn main(): void { let x: i32 = 1; switch (x) { case 1.0: {} } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, NonConstantSwitchCase) {
    std::string source = "fn main(): void { let x: i32 = 1; let y: i32 = 2; switch (x) { case y: {} } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, IncorrectArgumentCount) {
    std::string source = "fn foo(a: i32): void; fn main(): void { foo(); }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, IncorrectArgumentType) {
    std::string source = "fn foo(a: i32): void; fn main(): void { foo(1.0); }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidIfStatement) {
    std::string source = "fn main(): void { if (1 > 2) { let x: i32 = 1; } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, InvalidIfStatementCondition) {
    std::string source = "fn main(): void { if (1) { let x: i32 = 1; } }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidReturnType) {
    std::string source = "fn main(): i32 { return 10; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_NO_THROW(analyzer.analyze(functions));
}

TEST(SemanticAnalyzerTest, InvalidReturnType) {
    std::string source = "fn main(): i32 { return 10.5; }"; // Returns float, expects i32
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}

TEST(SemanticAnalyzerTest, RedeclaredVariable) {
    std::string source = "fn main(): void { let x: i32 = 10; let x: i32 = 20; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(functions), std::runtime_error);
}
