#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <vector>
#include <string>
#include <memory>
#include "token.h"

// Base class for all nodes in the AST
class Node {
public:
    virtual ~Node() = default;
    virtual std::string to_string() const = 0;
};

// Base class for all expression nodes
class Expression : public Node {
public:
    // A dummy method to satisfy the interface for now
    std::string to_string() const override { return "Expression"; }
};

// Base class for all statement nodes
class Statement : public Node {
public:
    // A dummy method to satisfy the interface for now
    std::string to_string() const override { return "Statement"; }
};

// The root node of the AST
class Program : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    std::string to_string() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->to_string();
        }
        return out;
    }
};

class Identifier : public Expression {
public:
    Token token; // The IDENTIFIER token
    std::string value;

    std::string to_string() const override {
        return value;
    }
};

class Type : public Node {
public:
    Token token;
    std::string name;

    std::string to_string() const override {
        return name;
    }
};

class VarDeclarationStatement : public Statement {
public:
    Token token; // The LET or MUT token
    bool is_mutable;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Type> type;
    std::unique_ptr<Expression> value;

    std::string to_string() const override {
        std::string out = token.literal + " " + name->to_string();
        if (type) {
            out += ": " + type->to_string();
        }
        out += " = ";
        if (value) {
            out += value->to_string();
        }
        out += ";";
        return out;
    }
};

class IntegerLiteral : public Expression {
public:
    Token token;
    long long value;

    std::string to_string() const override {
        return token.literal;
    }
};

class PrefixExpression : public Expression {
public:
    Token token; // The operator token, e.g., !
    std::unique_ptr<Expression> right;

    std::string to_string() const override {
        return "(" + token.literal + right->to_string() + ")";
    }
};

class InfixExpression : public Expression {
public:
    Token token; // The operator token, e.g., +
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    std::string to_string() const override {
        return "(" + left->to_string() + " " + token.literal + " " + right->to_string() + ")";
    }
};

class BooleanLiteral : public Expression {
public:
    Token token;
    bool value;

    std::string to_string() const override {
        return token.literal;
    }
};

class BlockStatement : public Statement {
public:
    Token token; // The { token
    std::vector<std::unique_ptr<Statement>> statements;

    std::string to_string() const override {
        std::string out;
        for (const auto& s : statements) {
            out += s->to_string();
        }
        return out;
    }
};

class IfExpression : public Expression {
public:
    Token token; // The 'if' token
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> consequence;
    std::unique_ptr<BlockStatement> alternative;

    std::string to_string() const override {
        std::string out = "if" + condition->to_string() + " " + consequence->to_string();
        if (alternative) {
            out += "else " + alternative->to_string();
        }
        return out;
    }
};

class FunctionStatement : public Statement {
public:
    Token token; // The FN token
    std::unique_ptr<Identifier> name;
    std::vector<std::pair<std::unique_ptr<Identifier>, std::unique_ptr<Type>>> parameters;
    std::unique_ptr<Type> return_type;
    std::unique_ptr<BlockStatement> body;

    std::string to_string() const override {
        std::string out = token.literal + " " + name->to_string() + "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            out += parameters[i].first->to_string() + ": " + parameters[i].second->to_string();
            if (i < parameters.size() - 1) {
                out += ", ";
            }
        }
        out += ") ";
        if (return_type) {
            out += ": " + return_type->to_string();
        }
        out += " { " + body->to_string() + " }";
        return out;
    }
};

class CallExpression : public Expression {
public:
    Token token; // The '(' token
    std::unique_ptr<Expression> function;
    std::vector<std::unique_ptr<Expression>> arguments;

    std::string to_string() const override {
        std::string out = function->to_string() + "(";
        for (size_t i = 0; i < arguments.size(); ++i) {
            out += arguments[i]->to_string();
            if (i < arguments.size() - 1) {
                out += ", ";
            }
        }
        out += ")";
        return out;
    }
};

class ExpressionStatement : public Statement {
public:
    Token token; // The first token of the expression
    std::unique_ptr<Expression> expression;

    std::string to_string() const override {
        if (expression) {
            return expression->to_string();
        }
        return "";
    }
};

#endif //CHTHOLLY_AST_H
