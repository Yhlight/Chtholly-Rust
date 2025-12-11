#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include "CodeGen.h"
#include <string>
#include <vector>
#include <memory>

namespace chtholly {

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

class LetExprAST : public ExprAST {
    std::string m_varName;
    std::unique_ptr<ExprAST> m_init;
public:
    LetExprAST(const std::string& varName, std::unique_ptr<ExprAST> init)
        : m_varName(varName), m_init(std::move(init)) {}
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
    std::vector<std::string> m_args;
public:
    PrototypeAST(const std::string& name, std::vector<std::string> args)
        : m_name(name), m_args(std::move(args)) {}

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
