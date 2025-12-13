#ifndef CHTHOLLY_SEMANTICANALYZER_H
#define CHTHOLLY_SEMANTICANALYZER_H

#include "ASTVisitor.h"
#include <map>
#include <string>

namespace Chtholly {

class SemanticAnalyzer : public ASTVisitor {
public:
    void analyze(const FunctionAST& function);

    void visit(const NumberExprAST& node) override;
    void visit(const StringExprAST& node) override;
    void visit(const VariableExprAST& node) override;
    void visit(const BinaryExprAST& node) override;
    void visit(const CallExprAST& node) override;
    void visit(const PrototypeAST& node) override;
    void visit(const FunctionAST& node) override;
    void visit(const VarDeclStmtAST& node) override;

private:
    mutable std::map<std::string, std::string> symbolTable;
    mutable std::string currentExprType;
};

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTICANALYZER_H
