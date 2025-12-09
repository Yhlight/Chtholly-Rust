#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Token.h"
#include "Type.h"

// Forward declaration for the pretty print helper
std::string indent(int level);
class SemanticAnalyzer;
class BlockStmtAST;

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
class ExprAST : public ASTNode {
public:
    std::shared_ptr<Type> type; // The type of the expression, resolved by the semantic analyzer
};

// Expression class for numeric literals
class NumberExprAST : public ExprAST {
public:
    double value;
    NumberExprAST(double val) : value(val) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "NumberExprAST: " << value << " [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
    }
};

// Expression class for string literals
class StringExprAST : public ExprAST {
public:
    std::string value;
    StringExprAST(std::string val) : value(std::move(val)) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "StringExprAST: " << value << " [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
    }
};

// Expression class for boolean literals
class BoolExprAST : public ExprAST {
public:
    bool value;
    BoolExprAST(bool val) : value(val) {}
    void print(int level = 0) const override {}
};

// Expression class for referencing a variable
class VariableExprAST : public ExprAST {
public:
    std::string name;
    VariableExprAST(std::string name) : name(std::move(name)) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "VariableExprAST: " << name << " [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
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
        std::cout << indent(level) << "BinaryExprAST: " << tokenTypeToString(op) << " [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
        lhs->print(level + 1);
        rhs->print(level + 1);
    }
};

// Expression class for a function call
class FunctionCallExprAST : public ExprAST {
public:
    std::string callee;
    std::vector<std::unique_ptr<ExprAST>> args;
    FunctionCallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args)
        : callee(callee), args(std::move(args)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "FunctionCallExprAST: " << callee << " [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
        for (const auto& arg : args) {
            arg->print(level + 1);
        }
    }
};

// Base class for all statement nodes
class StmtAST : public ASTNode {};

// Statement class for if-else statements
class IfStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<BlockStmtAST> thenBranch;
    std::unique_ptr<StmtAST> elseBranch; // Can be another IfStmtAST for else-if

    IfStmtAST(std::unique_ptr<ExprAST> condition, std::unique_ptr<BlockStmtAST> thenBranch, std::unique_ptr<StmtAST> elseBranch)
        : condition(std::move(condition)), thenBranch(std::move(thenBranch)), elseBranch(std::move(elseBranch)) {}

    void print(int level = 0) const override {}
};

// Statement class for while loops
class WhileStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<BlockStmtAST> body;

    WhileStmtAST(std::unique_ptr<ExprAST> condition, std::unique_ptr<BlockStmtAST> body)
        : condition(std::move(condition)), body(std::move(body)) {}

    void print(int level = 0) const override {}
};

// Statement class for wrapping an expression
class ExprStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> expr;
    ExprStmtAST(std::unique_ptr<ExprAST> expr) : expr(std::move(expr)) {}
    void print(int level = 0) const override {
        expr->print(level);
    }
};

// Statement class for a return statement
class ReturnStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> returnValue;
    ReturnStmtAST(std::unique_ptr<ExprAST> returnValue) : returnValue(std::move(returnValue)) {}
    void print(int level = 0) const override {
        std::cout << indent(level) << "ReturnStmtAST:" << std::endl;
        if (returnValue) {
            returnValue->print(level + 1);
        }
    }
};


// Statement class for variable declarations
class VarDeclStmtAST : public StmtAST {
public:
    std::string varName;
    bool isMutable;
    std::unique_ptr<TypeNameAST> type; // Optional type annotation
    std::unique_ptr<ExprAST> initExpr; // Optional initializer
    VarDeclStmtAST(std::string varName, bool isMutable, std::unique_ptr<TypeNameAST> type, std::unique_ptr<ExprAST> initExpr)
        : varName(std::move(varName)), isMutable(isMutable), type(std::move(type)), initExpr(std::move(initExpr)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << (isMutable ? "VarDecl(mut) " : "VarDecl ") << varName << std::endl;
        if (type) {
            type->print(level + 1);
        }
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
