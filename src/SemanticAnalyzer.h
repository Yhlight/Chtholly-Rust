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
    void analyze(std::vector<std::unique_ptr<FunctionAST>>& functions);

    void visit(NumberExprAST& node) override;
    void visit(StringExprAST& node) override;
    void visit(VariableExprAST& node) override;
    void visit(BinaryExprAST& node) override;
    void visit(AssignExprAST& node) override;
    void visit(BorrowExprAST& node) override;
    void visit(DereferenceExprAST& node) override;
    void visit(CallExprAST& node) override;
    void visit(PrototypeAST& node) override;
    void visit(FunctionAST& node) override;
    void visit(VarDeclStmtAST& node) override;
    void visit(ReturnStmtAST& node) override;
    void visit(ExprStmtAST& node) override;
    void visit(IfStmtAST& node) override;

private:
    enum class OwnershipState {
        Owned,
        Moved
    };

    struct SymbolInfo {
        const Type* type;
        OwnershipState state;
        int immutableBorrows = 0;
        bool mutableBorrowed = false;
    };

    mutable std::map<std::string, SymbolInfo> symbolTable;
    std::map<std::string, const PrototypeAST*> functionTable;
    const Type* m_currentFunctionReturnType = nullptr;
    const Type* lastType;
};

} // namespace Chtholly

#endif // CHTHOLLY_SEMANTICANALYZER_H
