#ifndef CHTHOLLY_CODEGENERATOR_H
#define CHTHOLLY_CODEGENERATOR_H

#include "AST.h"
#include "TypeResolver.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <map>

class SymbolTable;

class CodeGenerator {
public:
    CodeGenerator(SymbolTable& symbolTable);
    void generate(BlockStmtAST& ast);
    void dump() const;

private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::map<std::string, llvm::AllocaInst*> namedValues;
    std::vector<llvm::AllocaInst*> ownedValues; // Track owned values
    llvm::BasicBlock* currentSwitchExit = nullptr;
    TypeResolver typeResolver;
    bool isMemberAccess = false;

    void createStringSwitch(SwitchStmtAST& node);
    void declarePrintf();
    void declareMalloc();
    void declareStrcpy();
    void declareFree();
    llvm::Type* resolveType(const TypeNameAST& typeName);
    llvm::Type* resolveType(const Type& type);

    llvm::Value* visit(ASTNode& node);
    llvm::Value* visit(VarDeclStmtAST& node);
    llvm::Value* visit(StructDeclAST& node);
    llvm::Value* visit(ClassDeclAST& node);
    llvm::Value* visit(FunctionDeclAST& node);
    llvm::Value* visit(BlockStmtAST& node);
    llvm::Value* visit(BinaryExprAST& node);
    llvm::Value* visit(NumberExprAST& node);
    llvm::Value* visit(StringExprAST& node);
    llvm::Value* visit(BoolExprAST& node);
    llvm::Value* visit(VariableExprAST& node);
    llvm::Value* visit(StructInitializerExprAST& node);
    llvm::Value* visit(MemberAccessExprAST& node);
    llvm::Value* visit(FunctionCallExprAST& node);
    llvm::Value* visit(ExprStmtAST& node);
    llvm::Value* visit(ReturnStmtAST& node);
    llvm::Value* visit(IfStmtAST& node);
    llvm::Value* visit(WhileStmtAST& node);
    llvm::Value* visit(DoWhileStmtAST& node);
    llvm::Value* visit(ForStmtAST& node);
    llvm::Value* visit(SwitchStmtAST& node);
    llvm::Value* visit(BreakStmtAST& node);
    llvm::Value* visit(FallthroughStmtAST& node);
    llvm::Value* visit(BorrowExprAST& node);
    llvm::Value* visit(ArrayLiteralExprAST& node);
    llvm::Value* visit(ArrayIndexExprAST& node);
    llvm::Value* visit(EnumDeclAST& node);
    llvm::Value* visit(EnumVariantExprAST& node);
};

#endif // CHTHOLLY_CODEGENERATOR_H
