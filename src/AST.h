#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Token.h"

// Forward declaration for the pretty print helper
std::string indent(int level);
class SemanticAnalyzer;

// Base class for all AST nodes
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int level = 0) const = 0;
};

// Represents a type name
class TypeNameAST : public ASTNode {
public:
    std::string name;
    TypeNameAST(std::string name) : name(std::move(name)) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "TypeName: " << name << std::endl;
    }
};

// Base class for all expression nodes
class ExprAST : public ASTNode {};

// Expression class for numeric literals
class NumberExprAST : public ExprAST {
public:
    double value;
    NumberExprAST(double val) : value(val) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "NumberExprAST: " << value << std::endl;
    }
};

// Expression class for referencing a variable
class VariableExprAST : public ExprAST {
public:
    std::string name;
    VariableExprAST(std::string name) : name(std::move(name)) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "VariableExprAST: " << name << std::endl;
    }
};

// Expression class for a binary operator
class BinaryExprAST : public ExprAST {
public:
    TokenType op;
    std::unique_ptr<ExprAST> lhs, rhs;
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
public:
    std::string varName;
    bool isMutable;
    std::unique_ptr<ExprAST> initExpr; // Optional initializer
    VarDeclStmtAST(std::string varName, bool isMutable, std::unique_ptr<ExprAST> initExpr)
        : varName(std::move(varName)), isMutable(isMutable), initExpr(std::move(initExpr)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << (isMutable ? "VarDecl(mut) " : "VarDecl ") << varName << std::endl;
        if (initExpr) {
            initExpr->print(level + 1);
        }
    }
};

class BlockStmtAST; // Forward declare for FunctionDeclAST

// Represents a function argument
struct FunctionArg {
    std::string name;
    std::unique_ptr<TypeNameAST> type;
};

// Statement class for function declarations
class FunctionDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<FunctionArg> args;
    std::unique_ptr<TypeNameAST> returnType;
    std::unique_ptr<BlockStmtAST> body;
    FunctionDeclAST(std::string name, std::vector<FunctionArg> args, std::unique_ptr<TypeNameAST> returnType, std::unique_ptr<BlockStmtAST> body)
        : name(std::move(name)), args(std::move(args)), returnType(std::move(returnType)), body(std::move(body)) {}

    void print(int level = 0) const override;
};


// Represents a block of statements
class BlockStmtAST : public StmtAST {
public:
    std::vector<std::unique_ptr<StmtAST>> statements;
    void print(int level = 0) const override {
        std::cout << indent(level) << "BlockStmtAST:" << std::endl;
        for(const auto& stmt : statements) {
            if (stmt) stmt->print(level + 1);
        }
    }
};

inline void FunctionDeclAST::print(int level) const {
    std::cout << indent(level) << "FunctionDecl: " << name << std::endl;
    std::cout << indent(level + 1) << "Args:" << std::endl;
    for (const auto& arg : args) {
        std::cout << indent(level + 2) << arg.name << ":" << std::endl;
        arg.type->print(level + 3);
    }
    std::cout << indent(level + 1) << "Return Type:" << std::endl;
    returnType->print(level + 2);
    std::cout << indent(level + 1) << "Body:" << std::endl;
    body->print(level + 2);
}


// Helper to produce indentation for pretty printing
inline std::string indent(int level) {
    std::string res = "";
    for (int i = 0; i < level; ++i) {
        res += "  ";
    }
    return res;
}

#endif // CHTHOLLY_AST_H
