#ifndef CHTHOLLY_ASTVISITOR_H
#define CHTHOLLY_ASTVISITOR_H

namespace Chtholly {

class NumberExprAST;
class VariableExprAST;
class BinaryExprAST;
class CallExprAST;
class PrototypeAST;
class FunctionAST;
class VarDeclStmtAST;
class ReturnStmtAST;
class ExprStmtAST;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(const NumberExprAST& node) = 0;
    virtual void visit(const VariableExprAST& node) = 0;
    virtual void visit(const BinaryExprAST& node) = 0;
    virtual void visit(const CallExprAST& node) = 0;
    virtual void visit(const PrototypeAST& node) = 0;
    virtual void visit(const FunctionAST& node) = 0;
    virtual void visit(const VarDeclStmtAST& node) = 0;
    virtual void visit(const ReturnStmtAST& node) = 0;
    virtual void visit(const ExprStmtAST& node) = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_ASTVISITOR_H
