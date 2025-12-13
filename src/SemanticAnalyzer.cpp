#include "SemanticAnalyzer.h"
#include "AST.h"
#include <stdexcept>

namespace Chtholly {

void SemanticAnalyzer::analyze(const FunctionAST& function) {
    function.accept(*this);
}

void SemanticAnalyzer::visit(const NumberExprAST& node) {
    if (node.getValue() == (int)node.getValue()) {
        lastType = Type::getIntegerTy();
    } else {
        lastType = Type::getFloatTy();
    }
}

void SemanticAnalyzer::visit(const VariableExprAST& node) {
    if (symbolTable.find(node.getName()) == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }
    lastType = symbolTable[node.getName()];
}

void SemanticAnalyzer::visit(const BinaryExprAST& node) {
    node.getLHS()->accept(*this);
    const Type* LHSType = lastType;
    node.getRHS()->accept(*this);
    const Type* RHSType = lastType;

    if (LHSType->getTypeID() != RHSType->getTypeID()) {
        throw std::runtime_error("Type mismatch in binary expression");
    }
    lastType = LHSType;
}

void SemanticAnalyzer::visit(const CallExprAST& node) {
    // Nothing to do for now
}

void SemanticAnalyzer::visit(const PrototypeAST& node) {
    // Nothing to do for now
}

void SemanticAnalyzer::visit(const FunctionAST& node) {
    symbolTable.clear();
    m_currentFunctionReturnType = node.getProto()->getReturnType();

    for (const auto& arg : node.getProto()->getArgs()) {
        symbolTable[arg.first] = arg.second;
    }

    for (const auto& stmt : node.getBody()) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(const VarDeclStmtAST& node) {
    if (symbolTable.find(node.getName()) != symbolTable.end()) {
        throw std::runtime_error("Variable already declared: " + node.getName());
    }
    node.getInit()->accept(*this);
    if (node.getType()) {
        if (node.getType()->getTypeID() != lastType->getTypeID()) {
            throw std::runtime_error("Type mismatch in variable declaration");
        }
        symbolTable[node.getName()] = node.getType();
    } else {
        symbolTable[node.getName()] = lastType;
    }
}

void SemanticAnalyzer::visit(const ReturnStmtAST& node) {
    const Type* returnExprType = nullptr;
    if (node.getValue()) {
        node.getValue()->accept(*this);
        returnExprType = lastType;
    } else {
        returnExprType = Type::getVoidTy();
    }

    if (m_currentFunctionReturnType->getTypeID() != returnExprType->getTypeID()) {
        throw std::runtime_error("Return type mismatch");
    }
}

void SemanticAnalyzer::visit(const ExprStmtAST& node) {
    node.getExpr()->accept(*this);
}

} // namespace Chtholly
