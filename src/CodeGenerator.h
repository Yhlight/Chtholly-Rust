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

    void generate(const FunctionAST& function);
    void dump() const;

    void visit(const NumberExprAST& node) override;
    void visit(const VariableExprAST& node) override;
    void visit(const BinaryExprAST& node) override;
    void visit(const CallExprAST& node) override;
    void visit(const PrototypeAST& node) override;
    void visit(const FunctionAST& node) override;
    void visit(const VarDeclStmtAST& node) override;
    void visit(const ReturnStmtAST& node) override;
    void visit(const ExprStmtAST& node) override;
    void visit(const IfStmtAST& node) override;

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
