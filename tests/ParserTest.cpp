#include <gtest/gtest.h>
#include "../src/Lexer/Lexer.h"
#include "../src/Parser/Parser.h"
#include "../src/AST/StmtAST.h"
#include "../src/AST/ExprAST.h"

TEST(ParserTest, SimpleLetStatement) {
    std::string source = "let x = 5;";
    Lexer lexer(source);
    Parser parser(lexer);

    std::unique_ptr<StmtAST> stmt = parser.parseLetStatement();

    // Check if the statement is a LetStmtAST
    LetStmtAST* letStmt = dynamic_cast<LetStmtAST*>(stmt.get());
    ASSERT_NE(letStmt, nullptr);

    // Check the variable name
    EXPECT_EQ(letStmt->getName(), "x");

    // Check the value
    const ExprAST* value = letStmt->getValue();
    const NumberExprAST* numValue = dynamic_cast<const NumberExprAST*>(value);
    ASSERT_NE(numValue, nullptr);
    EXPECT_EQ(numValue->getValue(), 5);
}
