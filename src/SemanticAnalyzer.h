#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include "SymbolTable.h"
#include "TypeResolver.h"
#include <memory>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    void analyze(BlockStmtAST& ast);
    SymbolTable& getSymbolTable() { return symbolTable; }

private:
    SymbolTable symbolTable;
    TypeResolver typeResolver;
    FunctionDeclAST* currentFunction = nullptr;
    bool inSwitch = false;

    std::shared_ptr<Type> visit(ASTNode& node);
    std::shared_ptr<Type> visit(VarDeclStmtAST& node);
    std::shared_ptr<Type> visit(StructDeclAST& node);
    std::shared_ptr<Type> visit(FunctionDeclAST& node);
    std::shared_ptr<Type> visit(BlockStmtAST& node);
    std::shared_ptr<Type> visit(BinaryExprAST& node);
    std::shared_ptr<Type> visit(NumberExprAST& node);
    std::shared_ptr<Type> visit(StringExprAST& node);
    std::shared_ptr<Type> visit(VariableExprAST& node);
    std::shared_ptr<Type> visit(StructInitializerExprAST& node);
    std::shared_ptr<Type> visit(MemberAccessExprAST& node);
    std::shared_ptr<Type> visit(FunctionCallExprAST& node);
    std::shared_ptr<Type> visit(ExprStmtAST& node);
    std::shared_ptr<Type> visit(ReturnStmtAST& node);
    std::shared_ptr<Type> visit(IfStmtAST& node);
    std::shared_ptr<Type> visit(WhileStmtAST& node);
    std::shared_ptr<Type> visit(DoWhileStmtAST& node);
    std::shared_ptr<Type> visit(ForStmtAST& node);
    std::shared_ptr<Type> visit(SwitchStmtAST& node);
    std::shared_ptr<Type> visit(CaseBlockAST& node);
    std::shared_ptr<Type> visit(BreakStmtAST& node);
    std::shared_ptr<Type> visit(FallthroughStmtAST& node);
    std::shared_ptr<Type> visit(BoolExprAST& node);
    std::shared_ptr<Type> visit(TypeNameAST& node);
    std::shared_ptr<Type> visit(BorrowExprAST& node);
    std::shared_ptr<Type> visit(ReferenceTypeAST& node);
};

#endif // CHTHOLLY_SEMANTICANALYZER_H
