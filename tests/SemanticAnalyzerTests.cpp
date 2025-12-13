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
    EXPECT_NO_THROW(analyzer.analyze(*functions[0]));
}

TEST(SemanticAnalyzerTest, UndeclaredVariable) {
    std::string source = "fn main(): void { let x: i32 = y; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(*functions[0]), std::runtime_error);
}

TEST(SemanticAnalyzerTest, ValidAssignment) {
    std::string source = "fn main(): void { let x: i32 = 10; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_NO_THROW(analyzer.analyze(*functions[0]));
}

TEST(SemanticAnalyzerTest, TypeMismatchInDeclaration) {
    std::string source = "fn main(): void { let x: string = 10; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(*functions[0]), std::runtime_error);
}

TEST(SemanticAnalyzerTest, TypeMismatchInBinaryExpression) {
    std::string source = "fn main(): void { let x: i32 = 10; let y: string = \"hello\"; let z: i32 = x + y; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(*functions[0]), std::runtime_error);
}

TEST(SemanticAnalyzerTest, RedeclaredVariable) {
    std::string source = "fn main(): void { let x: i32 = 10; let x: i32 = 20; }";
    Lexer lexer(source);
    Parser parser(lexer);
    auto functions = parser.parse();
    SemanticAnalyzer analyzer;

    ASSERT_EQ(functions.size(), 1);
    EXPECT_THROW(analyzer.analyze(*functions[0]), std::runtime_error);
}
