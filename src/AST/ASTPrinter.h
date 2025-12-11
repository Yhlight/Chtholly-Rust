#ifndef CHTHOLLY_AST_PRINTER_H
#define CHTHOLLY_AST_PRINTER_H

#include "Expr.h"
#include <string>
#include <vector>
#include <sstream>
#include <any>

namespace Chtholly
{

class ASTPrinter : public ExprVisitor
{
public:
    std::string print(const Expr& expr)
    {
        return std::any_cast<std::string>(expr.accept(*this));
    }

    std::any visit(const Binary& expr) override
    {
        return parenthesize(std::string(expr.op.lexeme), { &expr.left, &expr.right });
    }

    std::any visit(const Grouping& expr) override
    {
        return parenthesize("group", { &expr.expression });
    }

    std::any visit(const Literal& expr) override
    {
        if (expr.value.has_value())
        {
            if (expr.value.type() == typeid(double))
            {
                std::string text = std::to_string(std::any_cast<double>(expr.value));
                // Remove trailing zeros for clean output
                text.erase(text.find_last_not_of('0') + 1, std::string::npos);
                if (text.back() == '.')
                {
                    text.pop_back();
                }
                return text;
            }
            if (expr.value.type() == typeid(std::string))
            {
                return std::any_cast<std::string>(expr.value);
            }
             if (expr.value.type() == typeid(bool))
            {
                return std::string(std::any_cast<bool>(expr.value) ? "true" : "false");
            }
        }
        return std::string("nil");
    }

    std::any visit(const Unary& expr) override
    {
        return parenthesize(std::string(expr.op.lexeme), { &expr.right });
    }

private:
    template<typename... Args>
    std::string parenthesize(const std::string& name, const std::vector<const std::unique_ptr<Expr>*>& exprs)
    {
        std::stringstream ss;
        ss << "(" << name;
        for (const auto* expr_ptr : exprs)
        {
            ss << " ";
            ss << std::any_cast<std::string>((*expr_ptr)->accept(*this));
        }
        ss << ")";
        return ss.str();
    }
};

} // namespace Chtholly

#endif // CHTHOLLY_AST_PRINTER_H
