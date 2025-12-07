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

class VarDeclarationStatement : public Statement {
public:
    Token token; // The LET or MUT token
    bool is_mutable;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    std::string to_string() const override {
        std::string out = token.literal + " " + name->to_string() + " = ";
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

#endif //CHTHOLLY_AST_H
