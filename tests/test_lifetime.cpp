#include "gtest/gtest.h"
#include "lexer.h"
#include "parser.h"
#include "semantic_analyzer.h"
#include <vector>
#include <string>

void expect_semantic_error(const std::string& source) {
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();
    ASSERT_FALSE(parser.hadError());

    Chtholly::SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.analyze(statements), std::runtime_error);
}

void expect_no_semantic_error(const std::string& source) {
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();
    ASSERT_FALSE(parser.hadError());

    Chtholly::SemanticAnalyzer analyzer;
    EXPECT_NO_THROW(analyzer.analyze(statements));
}

TEST(LifetimeTest, ValidMutableBorrow) {
    expect_no_semantic_error("let mut x: i32 = 5; let y = &mut x;");
}

TEST(LifetimeTest, InvalidMutableBorrow) {
    expect_semantic_error("let x: i32 = 5; let y = &mut x;");
}

TEST(LifetimeTest, MultipleMutableBorrows) {
    expect_semantic_error("let mut x: i32 = 5; let y = &mut x; let z = &mut x;");
}

TEST(LifetimeTest, MutableAndImmutableBorrows) {
    expect_semantic_error("let mut x: i32 = 5; let y = &x; let z = &mut x;");
}

TEST(LifetimeTest, ImmutableAndMutableBorrows) {
    expect_semantic_error("let mut x: i32 = 5; let y = &mut x; let z = &x;");
}

TEST(LifetimeTest, ValidMultipleImmutableBorrows) {
    expect_no_semantic_error("let x: i32 = 5; let y = &x; let z = &x;");
}

TEST(LifetimeTest, NestedScopeValidBorrow) {
    expect_no_semantic_error("let mut x: i32 = 5; { let y = &mut x; } let z = &mut x;");
}

TEST(LifetimeTest, NestedScopeInvalidBorrow) {
    expect_semantic_error("let mut x: i32 = 5; let y = &mut x; { let z = &mut x; }");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
