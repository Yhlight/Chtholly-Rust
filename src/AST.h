#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Token.h"

// Forward declaration for the pretty print helper
std::string indent(int level);

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int level = 0) const = 0;
};

// Base class for all expression nodes
class ExprAST : public ASTNode {};

// Expression class for numeric literals
class NumberExprAST : public ExprAST {
    double value;
public:
    NumberExprAST(double val) : value(val) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "NumberExprAST: " << value << std::endl;
    }
};

// Expression class for referencing a variable
class VariableExprAST : public ExprAST {
    std::string name;
public:
    VariableExprAST(std::string name) : name(std::move(name)) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "VariableExprAST: " << name << std::endl;
    }
};

// Expression class for a binary operator
class BinaryExprAST : public ExprAST {
    TokenType op;
    std::unique_ptr<ExprAST> lhs, rhs;
public:
    BinaryExprAST(TokenType op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "BinaryExprAST: " << tokenTypeToString(op) << std::endl;
        lhs->print(level + 1);
        rhs->print(level + 1);
    }
};

// Base class for all statement nodes
class StmtAST : public ASTNode {};

// Statement class for variable declarations
class VarDeclStmtAST : public StmtAST {
    std::string varName;
    bool isMutable;
    std::unique_ptr<ExprAST> initExpr; // Optional initializer
public:
    VarDeclStmtAST(std::string varName, bool isMutable, std::unique_ptr<ExprAST> initExpr)
        : varName(std::move(varName)), isMutable(isMutable), initExpr(std::move(initExpr)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << (isMutable ? "VarDecl(mut) " : "VarDecl ") << varName << std::endl;
        if (initExpr) {
            initExpr->print(level + 1);
        }
    }
};

// Represents a block of statements
class BlockStmtAST : public StmtAST {
public:
    std::vector<std::unique_ptr<StmtAST>> statements;
    void print(int level = 0) const override {
        std::cout << indent(level) << "BlockStmtAST:" << std::endl;
        for(const auto& stmt : statements) {
            stmt->print(level + 1);
        }
    }
};


// Helper to produce indentation for pretty printing
inline std::string indent(int level) {
    std::string res = "";
    for (int i = 0; i < level; ++i) {
        res += "  ";
    }
    return res;
}

#endif // CHTHOLLY_AST_H
