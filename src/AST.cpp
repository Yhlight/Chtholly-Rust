#include "AST.h"
#include "ASTVisitor.h"

namespace Chtholly {

void NumberExprAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VariableExprAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void BinaryExprAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void CallExprAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void PrototypeAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void FunctionAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void VarDeclStmtAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ReturnStmtAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void ExprStmtAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

void IfStmtAST::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

} // namespace Chtholly
