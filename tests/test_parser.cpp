#include "../src/Parser.h"
#include "../src/Lexer.h"
#include <cassert>
#include <iostream>
#include <vector>

void test_parser_let_and_return() {
    std::string source = "fn main() { let x = 10; return x; }";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();

    assert(ast.size() == 1);
    auto* fn_decl = dynamic_cast<FnDecl*>(ast[0].get());
    assert(fn_decl != nullptr);
    assert(fn_decl->name.value == "main");
    assert(fn_decl->params.empty());
    assert(fn_decl->body.size() == 2);

    auto* let_stmt = dynamic_cast<LetStmt*>(fn_decl->body[0].get());
    assert(let_stmt != nullptr);
    assert(let_stmt->name.value == "x");
    auto* number_expr = dynamic_cast<NumberExpr*>(let_stmt->initializer.get());
    assert(number_expr != nullptr);
    assert(number_expr->token.value == "10");

    auto* return_stmt = dynamic_cast<ReturnStmt*>(fn_decl->body[1].get());
    assert(return_stmt != nullptr);
    auto* var_expr = dynamic_cast<VariableExpr*>(return_stmt->value.get());
    assert(var_expr != nullptr);
    assert(var_expr->token.value == "x");

    std::cout << "Parser let and return test passed!" << std::endl;
}

void test_parser_expression_statement() {
    std::string source = "10 + 20;";
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    Parser parser(tokens);
    std::vector<std::unique_ptr<Stmt>> ast = parser.parse();

    assert(ast.size() == 1);
    auto* expr_stmt = dynamic_cast<ExpressionStmt*>(ast[0].get());
    assert(expr_stmt != nullptr);
    auto* binary_expr = dynamic_cast<BinaryExpr*>(expr_stmt->expression.get());
    assert(binary_expr != nullptr);

    std::cout << "Parser expression statement test passed!" << std::endl;
}

int main() {
    test_parser_let_and_return();
    test_parser_expression_statement();
    return 0;
}
