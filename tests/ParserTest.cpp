#include <gtest/gtest.h>
#include "../src/Lexer/Lexer.h"
#include "../src/Parser/Parser.h"
#include "../src/AST/ProgramAST.h"
#include "../src/AST/StmtAST.h"
#include "../src/AST/ExprAST.h"

TEST(ParserTest, MultipleLetStatements) {
    std::string source = "let x = 5; let y = 10;";
    Lexer lexer(source);
    Parser parser(lexer);

    std::unique_ptr<ProgramAST> program = parser.parse();
    ASSERT_NE(program, nullptr);

    const auto& statements = program->getStatements();
    ASSERT_EQ(statements.size(), 2);

    // Check the first statement
    const LetStmtAST* letStmt1 = dynamic_cast<const LetStmtAST*>(statements[0].get());
    ASSERT_NE(letStmt1, nullptr);
    EXPECT_EQ(letStmt1->getName(), "x");
    const NumberExprAST* numValue1 = dynamic_cast<const NumberExprAST*>(letStmt1->getValue());
    ASSERT_NE(numValue1, nullptr);
    EXPECT_EQ(numValue1->getValue(), 5);

    // Check the second statement
    const LetStmtAST* letStmt2 = dynamic_cast<const LetStmtAST*>(statements[1].get());
    ASSERT_NE(letStmt2, nullptr);
    EXPECT_EQ(letStmt2->getName(), "y");
    const NumberExprAST* numValue2 = dynamic_cast<const NumberExprAST*>(letStmt2->getValue());
    ASSERT_NE(numValue2, nullptr);
    EXPECT_EQ(numValue2->getValue(), 10);
}
