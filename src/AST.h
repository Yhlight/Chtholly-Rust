#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "CodeGen.h"
#include <string>
#include <vector>
#include <memory>
#include "llvm/IR/Type.h"

namespace chtholly {

class Type {
public:
    virtual ~Type() = default;
    virtual llvm::Type* getLLVMType(CodeGen& context) = 0;
};

class I32Type : public Type {
public:
    llvm::Type* getLLVMType(CodeGen& context) override;
};

class F64Type : public Type {
public:
    llvm::Type* getLLVMType(CodeGen& context) override;
};

class BoolType : public Type {
public:
    llvm::Type* getLLVMType(CodeGen& context) override;
};

class CharType : public Type {
public:
    llvm::Type* getLLVMType(CodeGen& context) override;
};

class VoidType : public Type {
public:
    llvm::Type* getLLVMType(CodeGen& context) override;
};


class ExprAST {
public:
    virtual ~ExprAST() = default;
    virtual llvm::Value* codegen(CodeGen& context) = 0;
};

class NumberExprAST : public ExprAST {
    double m_val;
public:
    NumberExprAST(double val) : m_val(val) {}
    llvm::Value* codegen(CodeGen& context) override;
};

class VariableExprAST : public ExprAST {
    std::string m_name;
public:
    VariableExprAST(const std::string& name) : m_name(name) {}
    llvm::Value* codegen(CodeGen& context) override;
};

class BinaryExprAST : public ExprAST {
    char m_op;
    std::unique_ptr<ExprAST> m_lhs, m_rhs;
public:
    BinaryExprAST(char op, std::unique_ptr<ExprAST> lhs, std::unique_ptr<ExprAST> rhs)
        : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}
    llvm::Value* codegen(CodeGen& context) override;
};

class IfExprAST : public ExprAST {
    std::unique_ptr<ExprAST> m_cond;
    std::vector<std::unique_ptr<ExprAST>> m_then;
    std::vector<std::unique_ptr<ExprAST>> m_else;
public:
    IfExprAST(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<ExprAST>> then, std::vector<std::unique_ptr<ExprAST>> elseBody)
        : m_cond(std::move(cond)), m_then(std::move(then)), m_else(std::move(elseBody)) {}
    llvm::Value* codegen(CodeGen& context) override;
};

class LetExprAST : public ExprAST {
    std::string m_varName;
    bool m_isMutable;
    std::unique_ptr<Type> m_type;
    std::unique_ptr<ExprAST> m_init;
public:
    LetExprAST(const std::string& varName, bool isMutable, std::unique_ptr<Type> type, std::unique_ptr<ExprAST> init)
        : m_varName(varName), m_isMutable(isMutable), m_type(std::move(type)), m_init(std::move(init)) {}
    llvm::Value* codegen(CodeGen& context) override;
};

// // Expression class for a function call.
// class CallExprAST : public ExprAST {
//     std::string m_callee;
//     std::vector<std::unique_ptr<ExprAST>> m_args;
// public:
//     CallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args)
//         : m_callee(callee), m_args(std::move(args)) {}
// };

class PrototypeAST {
    std::string m_name;
    std::vector<std::pair<std::string, std::unique_ptr<Type>>> m_args;
    std::unique_ptr<Type> m_returnType;
public:
    PrototypeAST(const std::string& name,
                 std::vector<std::pair<std::string, std::unique_ptr<Type>>> args,
                 std::unique_ptr<Type> returnType)
        : m_name(name), m_args(std::move(args)), m_returnType(std::move(returnType)) {}

    const std::string& getName() const { return m_name; }
    llvm::Function* codegen(CodeGen& context);
};

class FunctionAST {
    std::unique_ptr<PrototypeAST> m_proto;
    std::vector<std::unique_ptr<ExprAST>> m_body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::vector<std::unique_ptr<ExprAST>> body)
        : m_proto(std::move(proto)), m_body(std::move(body)) {}

    llvm::Function* codegen(CodeGen& context);
};

} // namespace chtholly

#endif // CHTHOLLY_AST_H
