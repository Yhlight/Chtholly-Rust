#include "SemanticAnalyzer.h"
#include "AST.h"
#include <stdexcept>

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

void SemanticAnalyzer::visit(VariableExprAST& node) {
    auto it = symbolTable.find(node.getName());
    if (it == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }
    if (it->second.state == OwnershipState::Moved) {
        throw std::runtime_error("Variable " + node.getName() + " has been moved");
    }
    lastType = it->second.type;
    node.setType(const_cast<Type*>(lastType));
}

void SemanticAnalyzer::visit(BinaryExprAST& node) {
    node.getLHS()->accept(*this);
    const Type* LHSType = lastType;
    node.getRHS()->accept(*this);
    const Type* RHSType = lastType;

    if (node.getOp() == TokenType::Equal) {
        if (auto* var = dynamic_cast<VariableExprAST*>(node.getLHS())) {
            auto it = symbolTable.find(var->getName());
            if (it == symbolTable.end()) {
                throw std::runtime_error("Undeclared variable: " + var->getName());
            }
            if (!it->second.isMutable) {
                throw std::runtime_error("Cannot assign to immutable variable: " + var->getName());
            }
            if (auto* rhsVar = dynamic_cast<VariableExprAST*>(node.getRHS())) {
                auto rhsIt = symbolTable.find(rhsVar->getName());
                if (rhsIt != symbolTable.end()) {
                    rhsIt->second.state = OwnershipState::Moved;
                    rhsVar->setMove(true);
                }
            }
            it->second.state = OwnershipState::Owned;
        }
    }


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
        if (auto* var = dynamic_cast<VariableExprAST*>(node.getArgs()[i].get())) {
            auto it = symbolTable.find(var->getName());
            if (it != symbolTable.end()) {
                it->second.state = OwnershipState::Moved;
                var->setMove(true);
            }
        }
    }

    lastType = proto->getReturnType();
}

void SemanticAnalyzer::visit(PrototypeAST& node) {
    // Nothing to do for now
}

void SemanticAnalyzer::visit(FunctionAST& node) {
    symbolTable.clear();
    m_currentFunctionReturnType = node.getProto()->getReturnType();

    for (const auto& arg : node.getProto()->getArgs()) {
        symbolTable[arg.first] = {arg.second, OwnershipState::Owned, false};
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
    if (auto* var = dynamic_cast<VariableExprAST*>(node.getInit())) {
        auto it = symbolTable.find(var->getName());
        if (it != symbolTable.end()) {
            it->second.state = OwnershipState::Moved;
            var->setMove(true);
        }
    }
    const Type* initType = lastType;
    if (node.getType()) {
        if (node.getType()->getTypeID() != initType->getTypeID()) {
            throw std::runtime_error("Type mismatch in variable declaration");
        }
        symbolTable[node.getName()] = {node.getType(), OwnershipState::Owned, node.isMutable()};
    } else {
        symbolTable[node.getName()] = {initType, OwnershipState::Owned, node.isMutable()};
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
