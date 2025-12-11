#ifndef CHTHOLLY_AST_H
#define CHTHOLLY_AST_H

#include <string>
#include <vector>
#include <memory>

namespace chtholly {

// Base class for all expression nodes.
class ExprAST {
public:
    virtual ~ExprAST() = default;
};

// Expression class for numeric literals like "1.0".
class NumberExprAST : public ExprAST {
    double m_val;
public:
    NumberExprAST(double val) : m_val(val) {}
};

// Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
    std::string m_name;
public:
    VariableExprAST(const std::string& name) : m_name(name) {}
};

// // Expression class for a function call.
// class CallExprAST : public ExprAST {
//     std::string m_callee;
//     std::vector<std::unique_ptr<ExprAST>> m_args;
// public:
//     CallExprAST(const std::string& callee, std::vector<std::unique_ptr<ExprAST>> args)
//         : m_callee(callee), m_args(std::move(args)) {}
// };

// This class represents the "prototype" for a function,
// which captures its name, and its argument names (thus implicitly the number
// of arguments the function takes).
class PrototypeAST {
    std::string m_name;
    std::vector<std::string> m_args;
public:
    PrototypeAST(const std::string& name, std::vector<std::string> args)
        : m_name(name), m_args(std::move(args)) {}

    const std::string& getName() const { return m_name; }
};

// This class represents a function definition itself.
class FunctionAST {
    std::unique_ptr<PrototypeAST> m_proto;
    std::unique_ptr<ExprAST> m_body;
public:
    FunctionAST(std::unique_ptr<PrototypeAST> proto, std::unique_ptr<ExprAST> body)
        : m_proto(std::move(proto)), m_body(std::move(body)) {}
};

} // namespace chtholly

#endif // CHTHOLLY_AST_H
