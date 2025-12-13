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
class IfStmtAST;

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    virtual void visit(NumberExprAST& node) = 0;
    virtual void visit(VariableExprAST& node) = 0;
    virtual void visit(BinaryExprAST& node) = 0;
    virtual void visit(CallExprAST& node) = 0;
    virtual void visit(PrototypeAST& node) = 0;
    virtual void visit(FunctionAST& node) = 0;
    virtual void visit(VarDeclStmtAST& node) = 0;
    virtual void visit(ReturnStmtAST& node) = 0;
    virtual void visit(ExprStmtAST& node) = 0;
    virtual void visit(IfStmtAST& node) = 0;
};

} // namespace Chtholly

#endif // CHTHOLLY_ASTVISITOR_H
