#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <memory>
#include <optional>
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
    virtual void accept(ASTVisitor& visitor) = 0;
};

/// NumberExprAST - Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double Val;
    TokenType TokType;

public:
    NumberExprAST(double Val, TokenType TokType) : Val(Val), TokType(TokType) {}
    double getValue() const { return Val; }
    TokenType getTokType() const { return TokType; }
    void accept(ASTVisitor& visitor) override;
};

class BooleanExprAST : public ExprAST {
    bool Val;

public:
    BooleanExprAST(bool Val) : Val(Val) {}
    bool getValue() const { return Val; }
    void accept(ASTVisitor& visitor) override;
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
    void accept(ASTVisitor& visitor) override;
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
    ExprAST* getLHS() const { return LHS.get(); }
    ExprAST* getRHS() const { return RHS.get(); }
    const Type* getType() const { return Ty; }
    void setType(Type* t) { Ty = t; }
    void accept(ASTVisitor& visitor) override;
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
    std::vector<std::unique_ptr<ExprAST>>& getArgs() { return Args; }
    void accept(ASTVisitor& visitor) override;
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
    void accept(ASTVisitor& visitor);
};

/// FunctionAST - This class represents a function definition itself.
class FunctionAST {
    std::unique_ptr<PrototypeAST> Proto;
    std::optional<std::vector<std::unique_ptr<StmtAST>>> Body;

public:
    virtual ~FunctionAST() = default;
    FunctionAST(std::unique_ptr<PrototypeAST> Proto,
                std::optional<std::vector<std::unique_ptr<StmtAST>>> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}

    PrototypeAST* getProto() const { return Proto.get(); }
    const std::optional<std::vector<std::unique_ptr<StmtAST>>>& getBody() const { return Body; }
    bool isDeclaration() const { return !Body.has_value(); }
    void accept(ASTVisitor& visitor);
};

/// StmtAST - Base class for all statement nodes.
class StmtAST {
public:
    virtual ~StmtAST() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
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
    ExprAST* getInit() const { return Init.get(); }
    void accept(ASTVisitor& visitor) override;
};

/// ReturnStmtAST - Statement class for a return statement.
class ReturnStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> Value;

public:
    ReturnStmtAST(std::unique_ptr<ExprAST> Value) : Value(std::move(Value)) {}
    ExprAST* getValue() const { return Value.get(); }
    void accept(ASTVisitor& visitor) override;
};

class ExprStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> expr;

public:
    ExprStmtAST(std::unique_ptr<ExprAST> expr) : expr(std::move(expr)) {}
    ExprAST* getExpr() const { return expr.get(); }
    void accept(ASTVisitor& visitor) override;
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

    ExprAST* getCond() const { return Cond.get(); }
    std::vector<std::unique_ptr<StmtAST>>& getThen() { return Then; }
    std::vector<std::unique_ptr<StmtAST>>& getElse() { return Else; }
    void accept(ASTVisitor& visitor) override;
};

class WhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> Cond;
    std::vector<std::unique_ptr<StmtAST>> Body;

public:
    WhileStmtAST(std::unique_ptr<ExprAST> Cond,
                 std::vector<std::unique_ptr<StmtAST>> Body)
        : Cond(std::move(Cond)), Body(std::move(Body)) {}

    ExprAST* getCond() const { return Cond.get(); }
    std::vector<std::unique_ptr<StmtAST>>& getBody() { return Body; }
    void accept(ASTVisitor& visitor) override;
};

class ForStmtAST : public StmtAST {
    std::unique_ptr<StmtAST> Init;
    std::unique_ptr<ExprAST> Cond;
    std::unique_ptr<ExprAST> Incr;
    std::vector<std::unique_ptr<StmtAST>> Body;

public:
    ForStmtAST(std::unique_ptr<StmtAST> Init,
               std::unique_ptr<ExprAST> Cond,
               std::unique_ptr<ExprAST> Incr,
               std::vector<std::unique_ptr<StmtAST>> Body)
        : Init(std::move(Init)), Cond(std::move(Cond)), Incr(std::move(Incr)), Body(std::move(Body)) {}

    StmtAST* getInit() const { return Init.get(); }
    ExprAST* getCond() const { return Cond.get(); }
    ExprAST* getIncr() const { return Incr.get(); }
    std::vector<std::unique_ptr<StmtAST>>& getBody() { return Body; }
    void accept(ASTVisitor& visitor) override;
};

class DoWhileStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> Cond;
    std::vector<std::unique_ptr<StmtAST>> Body;

public:
    DoWhileStmtAST(std::unique_ptr<ExprAST> Cond,
                   std::vector<std::unique_ptr<StmtAST>> Body)
        : Cond(std::move(Cond)), Body(std::move(Body)) {}

    ExprAST* getCond() const { return Cond.get(); }
    std::vector<std::unique_ptr<StmtAST>>& getBody() { return Body; }
    void accept(ASTVisitor& visitor) override;
};

class SwitchStmtAST : public StmtAST {
    std::unique_ptr<ExprAST> Cond;
    std::vector<std::pair<std::unique_ptr<ExprAST>, std::vector<std::unique_ptr<StmtAST>>>> Cases;

public:
    SwitchStmtAST(std::unique_ptr<ExprAST> Cond,
                  std::vector<std::pair<std::unique_ptr<ExprAST>, std::vector<std::unique_ptr<StmtAST>>>> Cases)
        : Cond(std::move(Cond)), Cases(std::move(Cases)) {}

    ExprAST* getCond() const { return Cond.get(); }
    std::vector<std::pair<std::unique_ptr<ExprAST>, std::vector<std::unique_ptr<StmtAST>>>>& getCases() { return Cases; }
    void accept(ASTVisitor& visitor) override;
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_H
