#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "AST.h"
#include "Type.h"
#include "ASTVisitor.h"
#include <map>
#include <string>

namespace Chtholly {

class SemanticAnalyzer : public ASTVisitor {
public:
    void analyze(const FunctionAST& function);

    void visit(const NumberExprAST& node) override;
    void visit(const VariableExprAST& node) override;
    void visit(const BinaryExprAST& node) override;
    void visit(const CallExprAST& node) override;
    void visit(const PrototypeAST& node) override;
    void visit(const FunctionAST& node) override;
    void visit(const VarDeclStmtAST& node) override;
    void visit(const ReturnStmtAST& node) override;
    void visit(const ExprStmtAST& node) override;

private:
    mutable std::map<std::string, const Type*> symbolTable;
    const Type* m_currentFunctionReturnType = nullptr;
    const Type* lastType;
};

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTICANALYZER_H
