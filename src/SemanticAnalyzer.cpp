#include "SemanticAnalyzer.h"
#include "AST.h"
#include <stdexcept>
#include <iostream>

namespace Chtholly {

void SemanticAnalyzer::analyze(std::vector<std::unique_ptr<FunctionAST>>& functions) {
    for (const auto& func : functions) {
        functionTable[func->getProto()->getName()] = func->getProto();
    }

    for (auto& func : functions) {
        func->accept(*this);
    }
}

void SemanticAnalyzer::visit(NumberExprAST& node) {
    if (node.getTokType() == TokenType::Integer) {
        lastType = Type::getIntegerTy();
    } else {
        lastType = Type::getFloatTy();
    }
}

void SemanticAnalyzer::visit(StringExprAST& node) {
    lastType = Type::getStringTy();
}

void SemanticAnalyzer::visit(VariableExprAST& node) {
    if (symbolTable.find(node.getName()) == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }
    SymbolInfo& info = symbolTable.at(node.getName());
    if (info.state == OwnershipState::Moved) {
        throw std::runtime_error("Variable has been moved: " + node.getName());
    }
    lastType = info.type;
    node.setType(const_cast<Type*>(lastType));
}

void SemanticAnalyzer::visit(BinaryExprAST& node) {
    node.getLHS()->accept(*this);
    const Type* LHSType = lastType;
    node.getRHS()->accept(*this);
    const Type* RHSType = lastType;

    if (LHSType->getTypeID() != RHSType->getTypeID()) {
        throw std::runtime_error("Type mismatch in binary expression");
    }

    switch (node.getOp()) {
        case TokenType::Less:
        case TokenType::LessEqual:
        case TokenType::Greater:
        case TokenType::GreaterEqual:
        case TokenType::EqualEqual:
        case TokenType::BangEqual:
            lastType = Type::getBoolTy();
            break;
        default:
            lastType = LHSType;
    }

    node.setType(const_cast<Type*>(lastType));
}

void SemanticAnalyzer::visit(CallExprAST& node) {
    if (functionTable.find(node.getCallee()) == functionTable.end()) {
        if (node.getCallee() == "println") {
            if (node.getArgs().size() != 1) {
                throw std::runtime_error("Incorrect number of arguments in call to println");
            }
            node.getArgs()[0]->accept(*this);
            if (!lastType->isStringTy()) {
                throw std::runtime_error("Argument to println must be a string");
            }
            lastType = Type::getVoidTy();
            return;
        }
        throw std::runtime_error("Function not declared: " + node.getCallee());
    }

    const PrototypeAST* proto = functionTable.at(node.getCallee());
    if (proto->getArgs().size() != node.getArgs().size()) {
        throw std::runtime_error("Incorrect number of arguments in call to " + node.getCallee());
    }

    for (size_t i = 0; i < node.getArgs().size(); ++i) {
        node.getArgs()[i]->accept(*this);
        if (lastType->getTypeID() != proto->getArgs()[i].second->getTypeID()) {
            throw std::runtime_error("Type mismatch in argument " + std::to_string(i) + " of call to " + node.getCallee());
        }
        if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.getArgs()[i].get())) {
            SymbolInfo& sourceInfo = symbolTable.at(varExpr->getName());
            sourceInfo.state = OwnershipState::Moved;
        }
    }

    lastType = proto->getReturnType();
}

void SemanticAnalyzer::visit(BorrowExprAST& node) {
    if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.getExpr())) {
        SymbolInfo& info = symbolTable.at(varExpr->getName());

        if (node.isMutable()) {
            if (info.immutableBorrows > 0 || info.mutableBorrowed) {
                throw std::runtime_error("Cannot borrow as mutable because it is already borrowed");
            }
            info.mutableBorrowed = true;
        } else {
            if (info.mutableBorrowed) {
                throw std::runtime_error("Cannot borrow as immutable because it is already borrowed as mutable");
            }
            info.immutableBorrows++;
        }
    }
    node.getExpr()->accept(*this);
    lastType = Type::getReferenceTy(const_cast<Type*>(lastType), node.isMutable());
}

void SemanticAnalyzer::visit(DereferenceExprAST& node) {
    node.getExpr()->accept(*this);
    if (!lastType->isReferenceTy()) {
        throw std::runtime_error("Cannot dereference a non-reference type");
    }
    lastType = static_cast<ReferenceType*>(const_cast<Type*>(lastType))->getBaseType();
}

void SemanticAnalyzer::visit(AssignExprAST& node) {
    if (symbolTable.find(node.getName()) == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }

    SymbolInfo& targetInfo = symbolTable.at(node.getName());

    node.getValue()->accept(*this);
    const Type* valueType = lastType;

    if (targetInfo.type->getTypeID() != valueType->getTypeID()) {
        throw std::runtime_error("Type mismatch in assignment");
    }

    if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.getValue())) {
        SymbolInfo& sourceInfo = symbolTable.at(varExpr->getName());
        sourceInfo.state = OwnershipState::Moved;
    }

    targetInfo.state = OwnershipState::Owned;
    lastType = targetInfo.type;
}

void SemanticAnalyzer::visit(PrototypeAST& node) {
    // Nothing to do for now
}

void SemanticAnalyzer::visit(FunctionAST& node) {
    symbolTable.clear();
    m_currentFunctionReturnType = node.getProto()->getReturnType();

    for (const auto& arg : node.getProto()->getArgs()) {
        symbolTable[arg.first] = {arg.second, OwnershipState::Owned};
    }

    if (node.getBody()) {
        for (auto& stmt : *node.getBody()) {
            stmt->accept(*this);
        }
    }
}

void SemanticAnalyzer::visit(VarDeclStmtAST& node) {
    if (symbolTable.find(node.getName()) != symbolTable.end()) {
        throw std::runtime_error("Variable already declared: " + node.getName());
    }
    node.getInit()->accept(*this);
    if (auto* varExpr = dynamic_cast<VariableExprAST*>(node.getInit())) {
        SymbolInfo& sourceInfo = symbolTable.at(varExpr->getName());
        sourceInfo.state = OwnershipState::Moved;
    }
    if (node.getType()) {
        if (node.getType()->isReferenceTy() && lastType->isReferenceTy()) {
            auto* declRef = static_cast<const ReferenceType*>(node.getType());
            auto* initRef = static_cast<const ReferenceType*>(lastType);
            if (declRef->getBaseType()->getTypeID() != initRef->getBaseType()->getTypeID() ||
                declRef->isMutable() != initRef->isMutable()) {
                throw std::runtime_error("Type mismatch in reference variable declaration");
            }
        } else if (node.getType()->getTypeID() != lastType->getTypeID()) {
            throw std::runtime_error("Type mismatch in variable declaration");
        }
        symbolTable[node.getName()] = {node.getType(), OwnershipState::Owned};
    } else {
        symbolTable[node.getName()] = {lastType, OwnershipState::Owned};
    }
}

void SemanticAnalyzer::visit(ReturnStmtAST& node) {
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

void SemanticAnalyzer::visit(ExprStmtAST& node) {
    node.getExpr()->accept(*this);
}

void SemanticAnalyzer::visit(IfStmtAST& node) {
    node.getCond()->accept(*this);
    if (!lastType->isBoolTy()) {
        throw std::runtime_error("If condition must be a boolean expression");
    }

    for (auto& stmt : node.getThen()) {
        stmt->accept(*this);
    }

    for (auto& stmt : node.getElse()) {
        stmt->accept(*this);
    }
}

} // namespace Chtholly
