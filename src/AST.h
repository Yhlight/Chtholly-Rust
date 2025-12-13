#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <memory>
#include <vector>
#include "Token.h"

namespace Chtholly {

class StmtAST;
class ASTVisitor;

/// ExprAST - Base class for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double Val;

public:
    NumberExprAST(double Val) : Val(Val) {}
    double getValue() const { return Val; }
    void accept(ASTVisitor& visitor) const override;
};

/// StringExprAST - Expression class for string literals.
class StringExprAST : public ExprAST {
    std::string Val;

public:
    StringExprAST(const std::string& Val) : Val(Val) {}
    const std::string& getValue() const { return Val; }
    void accept(ASTVisitor& visitor) const override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string Name;

public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
    const std::string& getName() const { return Name; }
    void accept(ASTVisitor& visitor) const override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    TokenType Op;
    std::unique_ptr<ExprAST> LHS, RHS;

public:
    BinaryExprAST(TokenType op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}

    TokenType getOp() const { return Op; }
    const ExprAST* getLHS() const { return LHS.get(); }
    const ExprAST* getRHS() const { return RHS.get(); }
    void accept(ASTVisitor& visitor) const override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<std::unique_ptr<ExprAST>> Args;

public:
    CallExprAST(const std::string &Callee,
                std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}

    const std::string& getCallee() const { return Callee; }
    const std::vector<std::unique_ptr<ExprAST>>& getArgs() const { return Args; }
    void accept(ASTVisitor& visitor) const override;
};

/// PrototypeAST - This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class PrototypeAST {
    std::string Name;
    std::vector<std::pair<std::string, std::string>> Args;
    std::string ReturnType;

public:
    PrototypeAST(const std::string &Name, std::vector<std::pair<std::string, std::string>> Args, const std::string& ReturnType)
        : Name(Name), Args(std::move(Args)), ReturnType(ReturnType) {}

    const std::string &getName() const { return Name; }
    const std::vector<std::pair<std::string, std::string>> &getArgs() const { return Args; }
    const std::string& getReturnType() const { return ReturnType; }
    void accept(ASTVisitor& visitor) const;
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::vector<std::unique_ptr<StmtAST>> Body;

public:
    virtual ~FunctionAST() = default;
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::vector<std::unique_ptr<StmtAST>> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}

    const PrototypeAST* getProto() const { return Proto.get(); }
    const std::vector<std::unique_ptr<StmtAST>>& getBody() const { return Body; }
    void accept(ASTVisitor& visitor) const;
};

/// StmtAST - Base class for all statement nodes.
class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual void accept(ASTVisitor& visitor) const = 0;
};

/// VarDeclStmtAST - Statement class for a variable declaration like "let x: i32 = 5;".
class VarDeclStmtAST : public StmtAST {
    std::string Name;
    std::string Type;
    bool IsMutable;
    std::unique_ptr<ExprAST> Init;

public:
    VarDeclStmtAST(const std::string &Name, const std::string& Type, bool IsMutable, std::unique_ptr<ExprAST> Init)
        : Name(Name), Type(Type), IsMutable(IsMutable), Init(std::move(Init)) {}

    const std::string& getName() const { return Name; }
    const std::string& getType() const { return Type; }
    bool isMutable() const { return IsMutable; }
    const ExprAST* getInit() const { return Init.get(); }
    void accept(ASTVisitor& visitor) const override;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
