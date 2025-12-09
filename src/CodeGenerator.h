#ifndef CHTHOLLY_CODEGENERATOR_H
#define CHTHOLLY_CODEGENERATOR_H

#include "AST.h"
#include "TypeResolver.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <map>

class CodeGenerator {
public:
    CodeGenerator();
    void generate(BlockStmtAST& ast);
    void dump() const;

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::Value*> namedValues;
    TypeResolver typeResolver;

    void declarePrintf();
    llvm::Type* resolveType(const TypeNameAST& typeName);

    llvm::Value* visit(ASTNode& node);
    llvm::Value* visit(VarDeclStmtAST& node);
    llvm::Value* visit(FunctionDeclAST& node);
    llvm::Value* visit(BlockStmtAST& node);
    llvm::Value* visit(BinaryExprAST& node);
    llvm::Value* visit(NumberExprAST& node);
    llvm::Value* visit(StringExprAST& node);
    llvm::Value* visit(WhileStmtAST& node);
    llvm::Value* visit(BoolExprAST& node);
    llvm::Value* visit(VariableExprAST& node);
    llvm::Value* visit(FunctionCallExprAST& node);
    llvm::Value* visit(ExprStmtAST& node);
    llvm::Value* visit(ReturnStmtAST& node);
    llvm::Value* visit(IfStmtAST& node);
    llvm::Value* visit(BorrowExprAST& node);
    llvm::Value* visit(DereferenceExprAST& node);
};

#endif // CHTHOLLY_CODEGENERATOR_H
