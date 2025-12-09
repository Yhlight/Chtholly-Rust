#include "Interpreter.h"
#include <stdexcept>

Interpreter::Interpreter() {
    environment = std::make_shared<Environment>();
}

void Interpreter::interpret(const Program& program) {
    for (const auto& stmt : program) {
        if (stmt) {
            execute(*stmt);
        }
    }
}

void Interpreter::execute(Stmt& stmt) {
    stmt.accept(*this);
}

Object Interpreter::evaluate(Expr& expr) {
    expr.accept(*this);
    return lastEvaluated;
}

void Interpreter::visitVarDeclStmt(VarDeclStmt& stmt) {
    Object value = std::monostate{};
    if (stmt.initializer) {
        value = evaluate(*stmt.initializer);
    }
    environment->define(stmt.name.lexeme, value);
}

void Interpreter::visitBinaryExpr(BinaryExpr& expr) {
    Object left = evaluate(*expr.left);
    Object right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::PLUS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) + std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) + std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::MINUS:
             if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) - std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) - std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::STAR:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) * std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) * std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::SLASH:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) / std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) / std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::GREATER:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) > std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) > std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::GREATER_EQUAL:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) >= std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) >= std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::LESS:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) < std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) < std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::LESS_EQUAL:
            if (std::holds_alternative<int>(left) && std::holds_alternative<int>(right)) {
                lastEvaluated = std::get<int>(left) <= std::get<int>(right);
            } else if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right)) {
                lastEvaluated = std::get<double>(left) <= std::get<double>(right);
            } else {
                throw std::runtime_error("Operands must be two numbers.");
            }
            break;
        case TokenType::EQUAL_EQUAL:
            lastEvaluated = left == right;
            break;
        case TokenType::BANG_EQUAL:
            lastEvaluated = left != right;
            break;
        default:
            throw std::runtime_error("Invalid binary operator.");
    }
}

void Interpreter::visitUnaryExpr(UnaryExpr& expr) {
    Object right = evaluate(*expr.right);

    switch (expr.op.type) {
        case TokenType::MINUS:
            if (std::holds_alternative<int>(right)) {
                lastEvaluated = -std::get<int>(right);
            } else if (std::holds_alternative<double>(right)) {
                lastEvaluated = -std::get<double>(right);
            } else {
                throw std::runtime_error("Operand must be a number.");
            }
            break;
        case TokenType::BANG:
            lastEvaluated = !std::get<bool>(right);
            break;
        default:
            throw std::runtime_error("Invalid unary operator.");
    }
}

void Interpreter::visitGroupingExpr(GroupingExpr& expr) {
    lastEvaluated = evaluate(*expr.expression);
}

void Interpreter::visitLiteralExpr(LiteralExpr& expr) {
    switch (expr.token.type) {
        case TokenType::INTEGER:
            lastEvaluated = std::get<int>(expr.token.literal);
            break;
        case TokenType::FLOAT:
            lastEvaluated = std::get<double>(expr.token.literal);
            break;
        case TokenType::STRING:
            lastEvaluated = std::get<std::string>(expr.token.literal);
            break;
        default:
            lastEvaluated = std::monostate{};
    }
}

void Interpreter::visitVariableExpr(VariableExpr& expr) {
    lastEvaluated = environment->get(expr.name.lexeme);
}
