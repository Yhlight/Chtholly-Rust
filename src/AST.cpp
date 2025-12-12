#include "AST.h"
#include "ASTVisitor.h"

namespace Chtholly {

void NumberExprAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

void VariableExprAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

void BinaryExprAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

void CallExprAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

void PrototypeAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

void FunctionAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

void VarDeclStmtAST::accept(ASTVisitor& visitor) const {
    visitor.visit(*this);
}

} // namespace Chtholly
