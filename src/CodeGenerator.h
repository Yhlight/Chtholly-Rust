#ifndef CHTHOLLY_CODEGENERATOR_H
#define CHTHOLLY_CODEGENERATOR_H

#include "AST.h"
#include "SymbolTable.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <memory>
#include <vector>
#include <map>

class CodeGenerator : public Visitor {
public:
    CodeGenerator();
    std::string generate(const std::vector<std::unique_ptr<Stmt>>& statements);

    void visit(NumberExpr& expr) override;
    void visit(VariableExpr& expr) override;
    void visit(BinaryExpr& expr) override;
    void visit(LetStmt& stmt) override;
    void visit(ReturnStmt& stmt) override;
    void visit(FnDecl& stmt) override;
    void visit(ExpressionStmt& stmt) override;
    void visit(IfStmt& stmt) override;
    void visit(BlockStmt& stmt) override;

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::Value*> namedValues;
    llvm::Value* lastValue = nullptr;
};

#endif //CHTHOLLY_CODEGENERATOR_H
