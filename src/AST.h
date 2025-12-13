#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <memory>
#include <vector>
#include "Token.h"
#include "Type.h"
#include "ASTVisitor.h"

namespace Chtholly {

class StmtAST;

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

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string Name;
    Type* Ty;

public:
    VariableExprAST(const std::string &Name) : Name(Name), Ty(nullptr) {}
    const std::string& getName() const { return Name; }
    const Type* getType() const { return Ty; }
    void setType(Type* t) { Ty = t; }
    void accept(ASTVisitor& visitor) const override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
    TokenType Op;
    std::unique_ptr<ExprAST> LHS, RHS;
    Type* Ty;

public:
    BinaryExprAST(TokenType op, std::unique_ptr<ExprAST> LHS,
                  std::unique_ptr<ExprAST> RHS)
        : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)), Ty(nullptr) {}

    TokenType getOp() const { return Op; }
    const ExprAST* getLHS() const { return LHS.get(); }
    const ExprAST* getRHS() const { return RHS.get(); }
    const Type* getType() const { return Ty; }
    void setType(Type* t) { Ty = t; }
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
    std::vector<std::pair<std::string, Type*>> Args;
    Type* ReturnType;

public:
    PrototypeAST(const std::string &Name, std::vector<std::pair<std::string, Type*>> Args, Type* ReturnType)
        : Name(Name), Args(std::move(Args)), ReturnType(ReturnType) {}

    const std::string &getName() const { return Name; }
    const std::vector<std::pair<std::string, Type*>> &getArgs() const { return Args; }
    const Type* getReturnType() const { return ReturnType; }
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
    Type* Ty;
    bool IsMutable;
    std::unique_ptr<ExprAST> Init;

public:
    VarDeclStmtAST(const std::string &Name, Type* Ty, bool IsMutable, std::unique_ptr<ExprAST> Init)
        : Name(Name), Ty(Ty), IsMutable(IsMutable), Init(std::move(Init)) {}

    const std::string& getName() const { return Name; }
    const Type* getType() const { return Ty; }
    bool isMutable() const { return IsMutable; }
    const ExprAST* getInit() const { return Init.get(); }
    void accept(ASTVisitor& visitor) const override;
};

/// ReturnStmtAST - Statement class for a return statement.
class ReturnStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> Value;

public:
    ReturnStmtAST(std::unique_ptr<ExprAST> Value) : Value(std::move(Value)) {}
    const ExprAST* getValue() const { return Value.get(); }
    void accept(ASTVisitor& visitor) const override;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> expr;

public:
    ExprStmtAST(std::unique_ptr<ExprAST> expr) : expr(std::move(expr)) {}
    const ExprAST* getExpr() const { return expr.get(); }
    void accept(ASTVisitor& visitor) const override;
};

class IfStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> Cond;
    std::vector<std::unique_ptr<StmtAST>> Then;
    std::vector<std::unique_ptr<StmtAST>> Else;

public:
    IfStmtAST(std::unique_ptr<ExprAST> Cond,
              std::vector<std::unique_ptr<StmtAST>> Then,
              std::vector<std::unique_ptr<StmtAST>> Else)
        : Cond(std::move(Cond)), Then(std::move(Then)), Else(std::move(Else)) {}

    const ExprAST* getCond() const { return Cond.get(); }
    const std::vector<std::unique_ptr<StmtAST>>& getThen() const { return Then; }
    const std::vector<std::unique_ptr<StmtAST>>& getElse() const { return Else; }
    void accept(ASTVisitor& visitor) const override;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
