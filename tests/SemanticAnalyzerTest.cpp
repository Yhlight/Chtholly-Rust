#include <gtest/gtest.h>
#include "../src/AST/ProgramAST.h"
#include "../src/AST/StmtAST.h"
#include "../src/Semantic/SemanticAnalyzer.h"
#include <vector>
#include <memory>

TEST(SemanticAnalyzerTest, ValidProgram) {
    std::vector<std::unique_ptr<StmtAST>> statements;
    statements.push_back(std::make_unique<LetStmtAST>("x", Type(BuiltinType::I32), std::make_unique<NumberExprAST>(5)));
    statements.push_back(std::make_unique<LetStmtAST>("y", Type(BuiltinType::F64), std::make_unique<FloatExprAST>(10.5)));
    ProgramAST program(std::move(statements));

    SemanticAnalyzer analyzer;
    EXPECT_NO_THROW(analyzer.visit(program));
}

TEST(SemanticAnalyzerTest, DuplicateVariable) {
    std::vector<std::unique_ptr<StmtAST>> statements;
    statements.push_back(std::make_unique<LetStmtAST>("x", Type(BuiltinType::I32), std::make_unique<NumberExprAST>(5)));
    statements.push_back(std::make_unique<LetStmtAST>("x", Type(BuiltinType::I32), std::make_unique<NumberExprAST>(10)));
    ProgramAST program(std::move(statements));

    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.visit(program), std::runtime_error);
}

TEST(SemanticAnalyzerTest, TypeMismatchIntToFloat) {
    std::vector<std::unique_ptr<StmtAST>> statements;
    statements.push_back(std::make_unique<LetStmtAST>("x", Type(BuiltinType::F64), std::make_unique<NumberExprAST>(5)));
    ProgramAST program(std::move(statements));

    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.visit(program), std::runtime_error);
}

TEST(SemanticAnalyzerTest, TypeMismatchFloatToInt) {
    std::vector<std::unique_ptr<StmtAST>> statements;
    statements.push_back(std::make_unique<LetStmtAST>("x", Type(BuiltinType::I32), std::make_unique<FloatExprAST>(5.5)));
    ProgramAST program(std::move(statements));

    SemanticAnalyzer analyzer;
    EXPECT_THROW(analyzer.visit(program), std::runtime_error);
}
