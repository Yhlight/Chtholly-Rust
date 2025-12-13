#ifndef CHTHOLLY_ASTVISITOR_H
#define CHTHOLLY_ASTVISITOR_H

#include "AST.h"

namespace Chtholly {

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    virtual void visit(const NumberExprAST& node) = 0;
    virtual void visit(const StringExprAST& node) = 0;
    virtual void visit(const VariableExprAST& node) = 0;
    virtual void visit(const BinaryExprAST& node) = 0;
    virtual void visit(const CallExprAST& node) = 0;
    virtual void visit(const PrototypeAST& node) = 0;
    virtual void visit(const FunctionAST& node) = 0;
    virtual void visit(const VarDeclStmtAST& node) = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_ASTVISITOR_H
