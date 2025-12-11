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
    std::unique_ptr<ExprAST> m_body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body)
        : m_proto(std::move(proto)), m_body(std::move(body)) {}

    llvm::Function* codegen(CodeGen& context);
};

} // namespace chtholly

#endif // CHTHOLLY_AST_H
