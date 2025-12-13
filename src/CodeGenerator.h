#ifndef CHTHOLLY_CODEGENERATOR_H
#define CHTHOLLY_CODEGENERATOR_H

#include "ASTVisitor.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <map>
#include <string>

namespace Chtholly {

class Type;

class CodeGenerator : public ASTVisitor {
public:
    CodeGenerator();

    void generate(FunctionAST& function);
    void dump();

    void visit(NumberExprAST& node) override;
    void visit(VariableExprAST& node) override;
    void visit(BinaryExprAST& node) override;
    void visit(CallExprAST& node) override;
    void visit(PrototypeAST& node) override;
    void visit(FunctionAST& node) override;
    void visit(VarDeclStmtAST& node) override;
    void visit(ReturnStmtAST& node) override;
    void visit(ExprStmtAST& node) override;
    void visit(IfStmtAST& node) override;

private:
    llvm::Type* llvmTypeFromChthollyType(const Type* type);

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    std::map<std::string, llvm::Value*> namedValues;
    llvm::Value* lastValue;
};

} // namespace Chtholly

#endif // CHTHOLLY_CODEGENERATOR_H
