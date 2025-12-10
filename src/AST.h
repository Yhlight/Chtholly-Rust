#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <utility>
#include "Token.h"
#include "Type.h"

// Forward declaration for the pretty print helper
std::string indent(int level);
class SemanticAnalyzer;
class BlockStmtAST;
class ExprAST;

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

class ReferenceTypeAST : public TypeNameAST {
public:
    std::unique_ptr<TypeNameAST> referencedType;
    bool isMutable;

    ReferenceTypeAST(std::unique_ptr<TypeNameAST> referencedType, bool isMutable)
        : TypeNameAST(""), referencedType(std::move(referencedType)), isMutable(isMutable) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << (isMutable ? "Mutable" : "Immutable") << " Reference to:" << std::endl;
        referencedType->print(level + 1);
    }
};

class ArrayTypeAST : public TypeNameAST {
public:
    std::unique_ptr<TypeNameAST> elementType;
    std::unique_ptr<ExprAST> size; // Can be nullptr for dynamic arrays/slices in the future

    ArrayTypeAST(std::unique_ptr<TypeNameAST> elementType, std::unique_ptr<ExprAST> size)
        : TypeNameAST(""), elementType(std::move(elementType)), size(std::move(size)) {}

    void print(int level = 0) const override;
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
    std::unique_ptr<ExprAST> callee;
    std::vector<std::unique_ptr<ExprAST>> args;
    FunctionCallExprAST(std::unique_ptr<ExprAST> callee, std::vector<std::unique_ptr<ExprAST>> args)
        : callee(std::move(callee)), args(std::move(args)) {}
    FunctionCallExprAST(const std::string& callee_name, std::vector<std::unique_ptr<ExprAST>> args)
        : callee(std::make_unique<VariableExprAST>(callee_name)), args(std::move(args)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "FunctionCallExprAST: [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
        callee->print(level + 1);
        for (const auto& arg : args) {
            arg->print(level + 1);
        }
    }
};

class BorrowExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> expression;
    bool isMutable;

    BorrowExprAST(std::unique_ptr<ExprAST> expression, bool isMutable)
        : expression(std::move(expression)), isMutable(isMutable) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << (isMutable ? "Mutable" : "Immutable") << " Borrow of:" << std::endl;
        expression->print(level + 1);
    }
};

// Represents a single member initializer in a struct initializer list
class MemberInitializerAST : public ASTNode {
public:
    std::string name; // Optional, for designated initializers
    std::unique_ptr<ExprAST> value;

    MemberInitializerAST(std::unique_ptr<ExprAST> value, std::string name = "")
        : name(std::move(name)), value(std::move(value)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "MemberInitializerAST: ";
        if (!name.empty()) {
            std::cout << name << " = ";
        }
        std::cout << std::endl;
        value->print(level + 1);
    }
};

// Expression class for struct initializers
class StructInitializerExprAST : public ExprAST {
public:
    std::string structName;
    std::vector<std::unique_ptr<MemberInitializerAST>> members;

    StructInitializerExprAST(std::string structName, std::vector<std::unique_ptr<MemberInitializerAST>> members)
        : structName(std::move(structName)), members(std::move(members)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "StructInitializerExprAST: " << structName << std::endl;
        for (const auto& member : members) {
            member->print(level + 1);
        }
    }
};

// Expression class for member access
class MemberAccessExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> object;
    std::string memberName;

    MemberAccessExprAST(std::unique_ptr<ExprAST> object, std::string memberName)
        : object(std::move(object)), memberName(std::move(memberName)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "MemberAccessExprAST: " << memberName << std::endl;
        object->print(level + 1);
    }
};

// Expression class for array literals
class ArrayLiteralExprAST : public ExprAST {
public:
    std::vector<std::unique_ptr<ExprAST>> elements;

    ArrayLiteralExprAST(std::vector<std::unique_ptr<ExprAST>> elements)
        : elements(std::move(elements)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "ArrayLiteralExprAST: [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
        for (const auto& element : elements) {
            element->print(level + 1);
        }
    }
};

// Expression class for array indexing
class ArrayIndexExprAST : public ExprAST {
public:
    std::unique_ptr<ExprAST> array;
    std::unique_ptr<ExprAST> index;

    ArrayIndexExprAST(std::unique_ptr<ExprAST> array, std::unique_ptr<ExprAST> index)
        : array(std::move(array)), index(std::move(index)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "ArrayIndexExprAST: [Type: " << (type ? type->toString() : "unresolved") << "]" << std::endl;
        std::cout << indent(level + 1) << "Array:" << std::endl;
        array->print(level + 2);
        std::cout << indent(level + 1) << "Index:" << std::endl;
        index->print(level + 2);
    }
};

class EnumVariantExprAST : public ExprAST {
public:
    std::string enumName;
    std::string variantName;
    std::vector<std::unique_ptr<ExprAST>> args;

    EnumVariantExprAST(std::string enumName, std::string variantName, std::vector<std::unique_ptr<ExprAST>> args)
        : enumName(std::move(enumName)), variantName(std::move(variantName)), args(std::move(args)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "EnumVariantExpr: " << enumName << "::" << variantName << std::endl;
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

    void print(int level = 0) const override;
};

// Statement class for do-while loops
class DoWhileStmtAST : public StmtAST {
public:
    std::unique_ptr<BlockStmtAST> body;
    std::unique_ptr<ExprAST> condition;

    DoWhileStmtAST(std::unique_ptr<BlockStmtAST> body, std::unique_ptr<ExprAST> condition)
        : body(std::move(body)), condition(std::move(condition)) {}

    void print(int level = 0) const override;
};

// Statement class for for loops
class ForStmtAST : public StmtAST {
public:
    std::unique_ptr<StmtAST> init;
    std::unique_ptr<ExprAST> condition;
    std::unique_ptr<ExprAST> increment;
    std::unique_ptr<BlockStmtAST> body;

    ForStmtAST(std::unique_ptr<StmtAST> init, std::unique_ptr<ExprAST> condition, std::unique_ptr<ExprAST> increment, std::unique_ptr<BlockStmtAST> body)
        : init(std::move(init)), condition(std::move(condition)), increment(std::move(increment)), body(std::move(body)) {}

    void print(int level = 0) const override;
};

// Statement class for break statements
class BreakStmtAST : public StmtAST {
public:
    BreakStmtAST() = default;
    void print(int level = 0) const override {
        std::cout << indent(level) << "BreakStmtAST" << std::endl;
    }
};

// Statement class for fallthrough statements
class FallthroughStmtAST : public StmtAST {
public:
    FallthroughStmtAST() = default;
    void print(int level = 0) const override {
        std::cout << indent(level) << "FallthroughStmtAST" << std::endl;
    }
};

// Represents a single case in a switch statement
class CaseBlockAST : public ASTNode {
public:
    std::unique_ptr<ExprAST> value; // The value for the case, can be nullptr for default
    std::unique_ptr<BlockStmtAST> body;

    CaseBlockAST(std::unique_ptr<ExprAST> value, std::unique_ptr<BlockStmtAST> body)
        : value(std::move(value)), body(std::move(body)) {}

    void print(int level = 0) const override;
};

// Statement class for switch statements
class SwitchStmtAST : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<CaseBlockAST>> cases;

    SwitchStmtAST(std::unique_ptr<ExprAST> condition, std::vector<std::unique_ptr<CaseBlockAST>> cases)
        : condition(std::move(condition)), cases(std::move(cases)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "SwitchStmtAST:" << std::endl;
        std::cout << indent(level + 1) << "Condition:" << std::endl;
        condition->print(level + 2);
        std::cout << indent(level + 1) << "Cases:" << std::endl;
        for (const auto& caseBlock : cases) {
            caseBlock->print(level + 2);
        }
    }
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
    std::shared_ptr<Type> resolvedType;
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

// Represents a member variable declaration in a struct or class
class MemberVarDeclAST : public ASTNode {
public:
    std::string name;
    std::unique_ptr<TypeNameAST> type;
    bool isMutable;
    std::unique_ptr<ExprAST> defaultValue; // For default values

    MemberVarDeclAST(std::string name, std::unique_ptr<TypeNameAST> type, bool isMutable, std::unique_ptr<ExprAST> defaultValue)
        : name(std::move(name)), type(std::move(type)), isMutable(isMutable), defaultValue(std::move(defaultValue)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << (isMutable ? "Member(mut) " : "Member ") << name << ":" << std::endl;
        type->print(level + 1);
        if (defaultValue) {
            std::cout << indent(level + 1) << "Default:" << std::endl;
            defaultValue->print(level + 2);
        }
    }
};

// Statement class for struct declarations
class StructDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::unique_ptr<MemberVarDeclAST>> members;

    StructDeclAST(std::string name, std::vector<std::unique_ptr<MemberVarDeclAST>> members)
        : name(std::move(name)), members(std::move(members)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "StructDecl: " << name << std::endl;
        for (const auto& member : members) {
            member->print(level + 1);
        }
    }
};

// Statement class for class declarations
class ClassDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::unique_ptr<MemberVarDeclAST>> members;
    std::vector<std::unique_ptr<FunctionDeclAST>> methods;

    ClassDeclAST(std::string name, std::vector<std::unique_ptr<MemberVarDeclAST>> members, std::vector<std::unique_ptr<FunctionDeclAST>> methods)
        : name(std::move(name)), members(std::move(members)), methods(std::move(methods)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "ClassDecl: " << name << std::endl;
        std::cout << indent(level + 1) << "Members:" << std::endl;
        for (const auto& member : members) {
            member->print(level + 2);
        }
        std::cout << indent(level + 1) << "Methods:" << std::endl;
        for (const auto& method : methods) {
            method->print(level + 2);
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
            if (stmt) stmt->print(level + 1);
        }
    }
};

class EnumVariantAST : public ASTNode {
public:
    std::string name;
    std::vector<std::unique_ptr<TypeNameAST>> types;

    EnumVariantAST(std::string name, std::vector<std::unique_ptr<TypeNameAST>> types)
        : name(std::move(name)), types(std::move(types)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "EnumVariant: " << name << std::endl;
        for (const auto& type : types) {
            type->print(level + 1);
        }
    }
};

class EnumDeclAST : public StmtAST {
public:
    std::string name;
    std::vector<std::unique_ptr<EnumVariantAST>> variants;

    EnumDeclAST(std::string name, std::vector<std::unique_ptr<EnumVariantAST>> variants)
        : name(std::move(name)), variants(std::move(variants)) {}

    void print(int level = 0) const override {
        std::cout << indent(level) << "EnumDecl: " << name << std::endl;
        for (const auto& variant : variants) {
            variant->print(level + 1);
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

inline void CaseBlockAST::print(int level) const {
    std::cout << indent(level) << "CaseBlockAST:" << std::endl;
    if (value) {
        value->print(level + 1);
    } else {
        std::cout << indent(level + 1) << "Default case" << std::endl;
    }
    body->print(level + 1);
}

inline void WhileStmtAST::print(int level) const {
    std::cout << indent(level) << "WhileStmtAST:" << std::endl;
    std::cout << indent(level + 1) << "Condition:" << std::endl;
    condition->print(level + 2);
    std::cout << indent(level + 1) << "Body:" << std::endl;
    body->print(level + 2);
}

inline void DoWhileStmtAST::print(int level) const {
    std::cout << indent(level) << "DoWhileStmtAST:" << std::endl;
    std::cout << indent(level + 1) << "Body:" << std::endl;
    body->print(level + 2);
    std::cout << indent(level + 1) << "Condition:" << std::endl;
    condition->print(level + 2);
}

inline void ForStmtAST::print(int level) const {
    std::cout << indent(level) << "ForStmtAST:" << std::endl;
    std::cout << indent(level + 1) << "Init:" << std::endl;
    if (init) init->print(level + 2);
    std::cout << indent(level + 1) << "Condition:" << std::endl;
    if (condition) condition->print(level + 2);
    std::cout << indent(level + 1) << "Increment:" << std::endl;
    if (increment) increment->print(level + 2);
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

inline void ArrayTypeAST::print(int level) const {
    std::cout << indent(level) << "ArrayType:" << std::endl;
    elementType->print(level + 1);
    if (size) {
        std::cout << indent(level + 1) << "Size:" << std::endl;
        size->print(level + 2);
    }
}


#endif // CHTHOLLY_AST_H
