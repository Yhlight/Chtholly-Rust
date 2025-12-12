#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <memory>
#include <vector>

namespace Chtholly {

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double Val;

public:
    NumberExprAST(double Val) : Val(Val) {}
    double getValue() const { return Val; }
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
    const std::string& getName() const { return Name; }
};

/// StmtAST - Base class for all statement nodes.
class StmtAST {
public:
    virtual ~StmtAST() = default;
};

/// VarDeclStmtAST - Statement class for a variable declaration like "let x = 5;".
class VarDeclStmtAST : public StmtAST {
    std::string Name;
    bool IsMutable;
    std::unique_ptr<ExprAST> Init;

public:
    VarDeclStmtAST(const std::string &Name, bool IsMutable, std::unique_ptr<ExprAST> Init)
        : Name(Name), IsMutable(IsMutable), Init(std::move(Init)) {}

    const std::string& getName() const { return Name; }
    bool isMutable() const { return IsMutable; }
    const ExprAST* getInit() const { return Init.get(); }
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
