#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <variant>

// A simple AST visitor to check the structure of the parsed tree
class AstPrinter : public Chtholly::ExprVisitor, public Chtholly::StmtVisitor
{
public:
    std::string print(const std::shared_ptr<Chtholly::Stmt>& stmt)
    {
        stmt->accept(*this);
        return result;
    }

    void visit(const Chtholly::BinaryExpr& expr) override {}
    void visit(const Chtholly::UnaryExpr& expr) override {}
    void visit(const Chtholly::CallExpr& expr) override {
        result += "call";
    }
    void visit(const Chtholly::FunctionStmt& stmt) override {
        result += "fn " + stmt.name.lexeme + "(";
        for (size_t i = 0; i < stmt.parameters.size(); ++i)
        {
            result += stmt.parameters[i].lexeme + ": " + stmt.parameterTypes[i].lexeme;
            if (i < stmt.parameters.size() - 1)
            {
                result += ", ";
            }
        }
        result += "): " + stmt.returnType.lexeme + " { ... }";
    }
    void visit(const Chtholly::ReturnStmt& stmt) override {
        result += "return";
        if (stmt.value)
        {
            result += " ";
            stmt.value->accept(*this);
        }
        result += ";";
    }

    void visit(const Chtholly::LiteralExpr& expr) override
    {
        std::visit([this](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) result += std::to_string(arg);
            else if constexpr (std::is_same_v<T, double>) result += std::to_string(arg);
            else if constexpr (std::is_same_v<T, std::string>) result += arg;
            else if constexpr (std::is_same_v<T, char>) result += arg;
            else if constexpr (std::is_same_v<T, bool>) result += (arg ? "true" : "false");
        }, expr.value);
    }

    void visit(const Chtholly::VariableExpr& expr) override {}
    void visit(const Chtholly::AssignExpr& expr) override {}

    void visit(const Chtholly::ExpressionStmt& stmt) override
    {
        stmt.expression->accept(*this);
        result += ";";
    }

    void visit(const Chtholly::LetStmt& stmt) override
    {
        result += "let " + stmt.name.lexeme;
        if (stmt.isMutable)
        {
            result += " mut";
        }
        if (stmt.initializer)
        {
            result += " = ";
            stmt.initializer->accept(*this);
        }
        result += ";";
    }

    void visit(const Chtholly::BlockStmt& stmt) override
    {
        result += "{ ";
        for (const auto& statement : stmt.statements)
        {
            statement->accept(*this);
        }
        result += " }";
    }

    void visit(const Chtholly::IfStmt& stmt) override
    {
        result += "if (...) ";
        stmt.thenBranch->accept(*this);
        if (stmt.elseBranch)
        {
            result += " else ";
            stmt.elseBranch->accept(*this);
        }
    }

    void visit(const Chtholly::WhileStmt& stmt) override
    {
        result += "while (...) ";
        stmt.body->accept(*this);
    }

    void visit(const Chtholly::ForStmt& stmt) override
    {
        result += "for (...; ...; ...) ";
        stmt.body->accept(*this);
    }

    void visit(const Chtholly::SwitchStmt& stmt) override
    {
        result += "switch (...) { ... }";
    }

    void visit(const Chtholly::CaseStmt& stmt) override
    {
        result += "case ...: ...";
    }

    void visit(const Chtholly::BreakStmt& stmt) override
    {
        result += "break;";
    }

    void visit(const Chtholly::ContinueStmt& stmt) override
    {
        result += "continue;";
    }

    void visit(const Chtholly::FallthroughStmt& stmt) override
    {
        result += "fallthrough;";
    }

    void visit(const Chtholly::StructStmt& stmt) override
    {
        result += "struct " + stmt.name.lexeme + " { ... }";
    }

    void visit(const Chtholly::GetExpr& expr) override
    {
        expr.object->accept(*this);
        result += "." + expr.name.lexeme;
    }

    void visit(const Chtholly::SetExpr& expr) override
    {
        expr.object->accept(*this);
        result += "." + expr.name.lexeme + " = ";
        expr.value->accept(*this);
    }

    void visit(const Chtholly::StructInitializerExpr& expr) override
    {
        result += expr.name.lexeme + "{ ... }";
    }

    void visit(const Chtholly::ClassStmt& stmt) override
    {
        result += "class " + stmt.name.lexeme + " { ... }";
    }

    void visit(const Chtholly::ThisExpr& expr) override
    {
        result += "self";
    }

    void visit(const Chtholly::EnumStmt& stmt) override
    {
        result += "enum " + stmt.name.lexeme + " { ... }";
    }

private:
    std::string result;
};


void test_simple_let_statement()
{
    std::string source = "let x = 10;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    assert(statements.size() == 1);

    AstPrinter printer;
    std::string output = printer.print(statements[0]);

    assert(output == "let x = 10;");

    std::cout << "test_simple_let_statement passed." << std::endl;
}

void test_invalid_assignment()
{
    std::string source = "5 = 10;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);

    // This should throw an exception, but for now, we just want to make sure it doesn't crash.
    // The parser should report an error and return a non-null vector.
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();
    assert(parser.hadError());

    std::cout << "test_invalid_assignment passed." << std::endl;
}


void test_if_statement()
{
    std::string source = "if (x > 0) { let a = 1; }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    assert(statements.size() == 1);

    AstPrinter printer;
    std::string output = printer.print(statements[0]);

    assert(output == "if (...) { let a = 1; }");
    std::cout << "test_if_statement passed." << std::endl;
}

void test_if_else_statement()
{
    std::string source = "if (x > 0) { let a = 1; } else { let b = 2; }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    assert(statements.size() == 1);

    AstPrinter printer;
    std::string output = printer.print(statements[0]);

    assert(output == "if (...) { let a = 1; } else { let b = 2; }");
    std::cout << "test_if_else_statement passed." << std::endl;
}

void test_while_statement()
{
    std::string source = "while (x > 0) { let a = 1; }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    assert(statements.size() == 1);

    AstPrinter printer;
    std::string output = printer.print(statements[0]);

    assert(output == "while (...) { let a = 1; }");
    std::cout << "test_while_statement passed." << std::endl;
}

void test_for_statement()
{
    std::string source = "for (let i = 0; i < 10; i = i + 1) { let a = 1; }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    assert(statements.size() == 1);

    AstPrinter printer;
    std::string output = printer.print(statements[0]);

    assert(output == "for (...; ...; ...) { let a = 1; }");
    std::cout << "test_for_statement passed." << std::endl;
}

void test_function_declaration()
{
    std::string source = "fn add(a: i32, b: i32): i32 { return a + b; }";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();

    assert(statements.size() == 1);

    AstPrinter printer;
    std::string output = printer.print(statements[0]);

    assert(output == "fn add(a: i32, b: i32): i32 { ... }");
    std::cout << "test_function_declaration passed." << std::endl;
}

void test_uninitialized_untyped_let()
{
    std::string source = "let x;";
    Chtholly::Lexer lexer(source);
    std::vector<Chtholly::Token> tokens = lexer.scanTokens();
    Chtholly::Parser parser(tokens);
    std::vector<std::shared_ptr<Chtholly::Stmt>> statements = parser.parse();
    assert(parser.hadError());
    std::cout << "test_uninitialized_untyped_let passed." << std::endl;
}

int main()
{
    test_simple_let_statement();
    test_invalid_assignment();
    test_if_statement();
    test_if_else_statement();
    test_while_statement();
    test_for_statement();
    test_function_declaration();
    test_uninitialized_untyped_let();
    return 0;
}
