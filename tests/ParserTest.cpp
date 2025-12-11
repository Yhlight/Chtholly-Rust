#include <gtest/gtest.h>
#include "../src/Lexer/Lexer.h"
#include "../src/Parser/Parser.h"
#include "../src/AST/ProgramAST.h"
#include "../src/AST/StmtAST.h"
#include "../src/AST/ExprAST.h"
#include "../src/Type/Type.h"

TEST(ParserTest, LetStatementWithType) {
    std::string source = "let x: i32 = 5;";
    Lexer lexer(source);
    Parser parser(lexer);

    std::unique_ptr<ProgramAST> program = parser.parse();
    ASSERT_NE(program, nullptr);

    const auto& statements = program->getStatements();
    ASSERT_EQ(statements.size(), 1);

    const LetStmtAST* letStmt = dynamic_cast<const LetStmtAST*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->getName(), "x");
    EXPECT_EQ(letStmt->getType().getBuiltinType(), BuiltinType::I32);

    const NumberExprAST* numValue = dynamic_cast<const NumberExprAST*>(letStmt->getValue());
    ASSERT_NE(numValue, nullptr);
    EXPECT_EQ(numValue->getValue(), 5);
}

TEST(ParserTest, LetStatementWithFloat) {
    std::string source = "let y: f64 = 123.456;";
    Lexer lexer(source);
    Parser parser(lexer);

    std::unique_ptr<ProgramAST> program = parser.parse();
    ASSERT_NE(program, nullptr);

    const auto& statements = program->getStatements();
    ASSERT_EQ(statements.size(), 1);

    const LetStmtAST* letStmt = dynamic_cast<const LetStmtAST*>(statements[0].get());
    ASSERT_NE(letStmt, nullptr);
    EXPECT_EQ(letStmt->getName(), "y");
    EXPECT_EQ(letStmt->getType().getBuiltinType(), BuiltinType::F64);

    const FloatExprAST* floatValue = dynamic_cast<const FloatExprAST*>(letStmt->getValue());
    ASSERT_NE(floatValue, nullptr);
    EXPECT_EQ(floatValue->getValue(), 123.456);
}
