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

void SemanticAnalyzer::visit(BooleanExprAST& node) {
    lastType = Type::getBoolTy();
}

void SemanticAnalyzer::visit(VariableExprAST& node) {
    if (symbolTable.find(node.getName()) == symbolTable.end()) {
        throw std::runtime_error("Undeclared variable: " + node.getName());
    }
    lastType = symbolTable.at(node.getName());
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
        case TokenType::Equal:
            if (!dynamic_cast<VariableExprAST*>(node.getLHS())) {
                throw std::runtime_error("Destination of '=' must be a variable");
            }
            break;
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
        symbolTable[arg.first] = arg.second;
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
    if (node.getType()) {
        if (node.getType()->getTypeID() != lastType->getTypeID()) {
            throw std::runtime_error("Type mismatch in variable declaration");
        }
        symbolTable[node.getName()] = node.getType();
    } else {
        symbolTable[node.getName()] = lastType;
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

void SemanticAnalyzer::visit(WhileStmtAST& node) {
    node.getCond()->accept(*this);
    if (!lastType->isBoolTy()) {
        throw std::runtime_error("While condition must be a boolean expression");
    }

    for (auto& stmt : node.getBody()) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(DoWhileStmtAST& node) {
    node.getCond()->accept(*this);
    if (!lastType->isBoolTy()) {
        throw std::runtime_error("Do-while condition must be a boolean expression");
    }

    for (auto& stmt : node.getBody()) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(ForStmtAST& node) {
    node.getInit()->accept(*this);
    node.getCond()->accept(*this);
    if (!lastType->isBoolTy()) {
        throw std::runtime_error("For condition must be a boolean expression");
    }
    node.getIncr()->accept(*this);

    for (auto& stmt : node.getBody()) {
        stmt->accept(*this);
    }
}

void SemanticAnalyzer::visit(SwitchStmtAST& node) {
    node.getCond()->accept(*this);
    const Type* condType = lastType;

    if (!condType->isIntegerTy()) {
        throw std::runtime_error("Switch condition must be an integer expression");
    }

    for (auto& Case : node.getCases()) {
        Case.first->accept(*this);
        if (lastType->getTypeID() != condType->getTypeID()) {
            throw std::runtime_error("Switch case type mismatch");
        }
        if (!dynamic_cast<NumberExprAST*>(Case.first.get())) {
            throw std::runtime_error("Case expressions must be constant integers");
        }
        for (auto& stmt : Case.second) {
            stmt->accept(*this);
        }
    }
}

} // namespace Chtholly
