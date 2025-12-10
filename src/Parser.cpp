#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

std::unique_ptr<BlockStmtAST> Parser::parse() {
    auto block = std::make_unique<BlockStmtAST>();
    while (!is_at_end()) {
        block->statements.push_back(parse_statement());
    }
    return block;
}

Token& Parser::peek() {
    return tokens[current_token_idx];
}

Token& Parser::advance() {
    if (!is_at_end()) {
        current_token_idx++;
    }
    return tokens[current_token_idx - 1];
}

bool Parser::is_at_end() {
    return peek().type == TokenType::END_OF_FILE;
}

std::unique_ptr<StmtAST> Parser::parse_statement() {
    if (peek().type == TokenType::LET) {
        return std::unique_ptr<StmtAST>(parse_variable_declaration(true).release());
    }
    if (peek().type == TokenType::STRUCT) {
        return parse_struct_definition();
    }
    if (peek().type == TokenType::CLASS) {
        return parse_class_definition();
    }
    if (peek().type == TokenType::FN) {
        return parse_function_definition();
    }
    if (peek().type == TokenType::IF) {
        return parse_if_statement();
    }
    if (peek().type == TokenType::WHILE) {
        return parse_while_statement();
    }
    if (peek().type == TokenType::DO) {
        return parse_do_while_statement();
    }
    if (peek().type == TokenType::FOR) {
        return parse_for_statement();
    }
    if (peek().type == TokenType::SWITCH) {
        return parse_switch_statement();
    }
    if (peek().type == TokenType::BREAK) {
        return parse_break_statement();
    }
    if (peek().type == TokenType::FALLTHROUGH) {
        return parse_fallthrough_statement();
    }
    if (peek().type == TokenType::RETURN) {
        return parse_return_statement();
    }
    return parse_expression_statement();
}

std::unique_ptr<StmtAST> Parser::parse_break_statement() {
    advance(); // consume 'break'
    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after 'break'");
    }
    advance(); // consume ';'
    return std::make_unique<BreakStmtAST>();
}

std::unique_ptr<StmtAST> Parser::parse_fallthrough_statement() {
    advance(); // consume 'fallthrough'
    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after 'fallthrough'");
    }
    advance(); // consume ';'
    return std::make_unique<FallthroughStmtAST>();
}

std::unique_ptr<StmtAST> Parser::parse_switch_statement() {
    advance(); // consume 'switch'
    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after 'switch'");
    }
    advance(); // consume '('
    auto condition = parse_expression();
    if (peek().type != TokenType::RIGHT_PAREN) {
        throw std::runtime_error("Expected ')' after switch condition");
    }
    advance(); // consume ')'

    if (peek().type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' for switch body");
    }
    advance(); // consume '{'

    std::vector<std::unique_ptr<CaseBlockAST>> cases;
    while (peek().type != TokenType::RIGHT_BRACE) {
        if (peek().type == TokenType::CASE) {
            advance(); // consume 'case'
            auto value = parse_expression();
            if (peek().type != TokenType::COLON) {
                throw std::runtime_error("Expected ':' after case value");
            }
            advance(); // consume ':'
            auto body = parse_block();
            cases.push_back(std::make_unique<CaseBlockAST>(std::move(value), std::move(body)));
        } else if (peek().type == TokenType::DEFAULT) {
            advance(); // consume 'default'
            if (peek().type != TokenType::COLON) {
                throw std::runtime_error("Expected ':' after 'default'");
            }
            advance(); // consume ':'
            auto body = parse_block();
            cases.push_back(std::make_unique<CaseBlockAST>(nullptr, std::move(body)));
        } else {
            throw std::runtime_error("Expected 'case' or 'default' in switch body");
        }
    }

    advance(); // consume '}'

    return std::make_unique<SwitchStmtAST>(std::move(condition), std::move(cases));
}


std::unique_ptr<StmtAST> Parser::parse_return_statement() {
    advance(); // consume 'return'
    std::unique_ptr<ExprAST> value = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        value = parse_expression();
    }
    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after return value");
    }
    advance(); // consume ';'
    return std::make_unique<ReturnStmtAST>(std::move(value));
}

std::unique_ptr<StmtAST> Parser::parse_expression_statement() {
    auto expr = parse_expression();
    if (peek().type == TokenType::SEMICOLON) {
        advance(); // consume ';'
    }
    return std::make_unique<ExprStmtAST>(std::move(expr));
}

std::unique_ptr<StmtAST> Parser::parse_if_statement() {
    advance(); // consume 'if'
    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after 'if'");
    }
    advance(); // consume '('
    auto condition = parse_expression();
    if (peek().type != TokenType::RIGHT_PAREN) {
        throw std::runtime_error("Expected ')' after if condition");
    }
    advance(); // consume ')'

    auto thenBranch = parse_block();
    std::unique_ptr<StmtAST> elseBranch = nullptr;

    if (peek().type == TokenType::ELSE) {
        advance(); // consume 'else'
        if (peek().type == TokenType::IF) {
            elseBranch = parse_if_statement();
        } else {
            elseBranch = parse_block();
        }
    }

    return std::make_unique<IfStmtAST>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<StmtAST> Parser::parse_while_statement() {
    advance(); // consume 'while'
    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after 'while'");
    }
    advance(); // consume '('
    auto condition = parse_expression();
    if (peek().type != TokenType::RIGHT_PAREN) {
        throw std::runtime_error("Expected ')' after while condition");
    }
    advance(); // consume ')'

    auto body = parse_block();

    return std::make_unique<WhileStmtAST>(std::move(condition), std::move(body));
}

std::unique_ptr<StmtAST> Parser::parse_do_while_statement() {
    advance(); // consume 'do'
    auto body = parse_block();

    if (peek().type != TokenType::WHILE) {
        throw std::runtime_error("Expected 'while' after 'do' block");
    }
    advance(); // consume 'while'

    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after 'while'");
    }
    advance(); // consume '('
    auto condition = parse_expression();
    if (peek().type != TokenType::RIGHT_PAREN) {
        throw std::runtime_error("Expected ')' after while condition");
    }
    advance(); // consume ')'

    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after 'do-while' condition");
    }
    advance(); // consume ';'

    return std::make_unique<DoWhileStmtAST>(std::move(body), std::move(condition));
}

std::unique_ptr<StmtAST> Parser::parse_for_statement() {
    advance(); // consume 'for'
    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after 'for'");
    }
    advance(); // consume '('

    std::unique_ptr<StmtAST> init;
    if (peek().type == TokenType::LET) {
        init = std::unique_ptr<StmtAST>(parse_variable_declaration(false).release());
    } else if (peek().type != TokenType::SEMICOLON) {
        init = parse_expression_statement();
    }
    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after for loop initializer");
    }
    advance(); // consume ';'

    std::unique_ptr<ExprAST> condition;
    if (peek().type != TokenType::SEMICOLON) {
        condition = parse_expression();
    }
    if (peek().type != TokenType::SEMICOLON) {
        throw std::runtime_error("Expected ';' after for loop condition");
    }
    advance(); // consume ';'

    std::unique_ptr<ExprAST> increment;
    if (peek().type != TokenType::RIGHT_PAREN) {
        increment = parse_expression();
    }
    if (peek().type != TokenType::RIGHT_PAREN) {
        throw std::runtime_error("Expected ')' after for loop increment");
    }
    advance(); // consume ')'

    auto body = parse_block();

    return std::make_unique<ForStmtAST>(std::move(init), std::move(condition), std::move(increment), std::move(body));
}


std::unique_ptr<VarDeclStmtAST> Parser::parse_variable_declaration(bool consume_semicolon) {
    advance(); // consume 'let'
    bool is_mutable = false;
    if (peek().type == TokenType::MUT) {
        is_mutable = true;
        advance(); // consume 'mut'
    }

    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected identifier after 'let' or 'let mut'");
    }
    std::string var_name = advance().value;

    std::unique_ptr<TypeNameAST> type = nullptr;
    if (peek().type == TokenType::COLON) {
        advance(); // consume ':'
        type = parse_type();
    }

    std::unique_ptr<ExprAST> initializer = nullptr;
    if (peek().type == TokenType::EQUAL) {
        advance(); // consume '='
        initializer = parse_expression();
    }

    if (consume_semicolon) {
        if (peek().type != TokenType::SEMICOLON) {
            throw std::runtime_error("Expected ';' after variable declaration");
        }
        advance(); // consume ';'
    }

    return std::make_unique<VarDeclStmtAST>(var_name, is_mutable, std::move(type), std::move(initializer));
}

std::unique_ptr<StmtAST> Parser::parse_function_definition() {
    advance(); // consume 'fn'
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected function name after 'fn'");
    }
    std::string func_name = advance().value;

    if (peek().type != TokenType::LEFT_PAREN) {
        throw std::runtime_error("Expected '(' after function name");
    }
    advance(); // consume '('

    std::vector<FunctionArg> args;
    while (peek().type != TokenType::RIGHT_PAREN) {
        if (peek().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected argument name");
        }
        std::string arg_name = advance().value;

        if (peek().type != TokenType::COLON) {
            throw std::runtime_error("Expected ':' after argument name");
        }
        advance(); // consume ':'

        args.push_back({arg_name, parse_type(), nullptr});

        if (peek().type == TokenType::COMMA) {
            advance(); // consume ','
        } else if (peek().type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ',' or ')' after argument");
        }
    }
    advance(); // consume ')'

    if (peek().type != TokenType::COLON) {
        throw std::runtime_error("Expected ':' for return type");
    }
    advance(); // consume ':'
    auto return_type = parse_type();

    auto body = parse_block();

    return std::make_unique<FunctionDeclAST>(func_name, std::move(args), std::move(return_type), std::move(body));
}

std::unique_ptr<StmtAST> Parser::parse_struct_definition() {
    advance(); // consume 'struct'
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected struct name after 'struct'");
    }
    std::string struct_name = advance().value;

    if (peek().type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' after struct name");
    }
    advance(); // consume '{'

    std::vector<std::unique_ptr<MemberVarDeclAST>> members;
    while (peek().type != TokenType::RIGHT_BRACE) {
        if (peek().type != TokenType::LET) {
            throw std::runtime_error("Expected 'let' for member variable declaration");
        }
        advance(); // consume 'let'

        bool is_mutable = false;
        if (peek().type == TokenType::MUT) {
            is_mutable = true;
            advance(); // consume 'mut'
        }

        if (peek().type != TokenType::IDENTIFIER) {
            throw std::runtime_error("Expected identifier for member variable name");
        }
        std::string member_name = advance().value;

        if (peek().type != TokenType::COLON) {
            throw std::runtime_error("Expected ':' after member variable name");
        }
        advance(); // consume ':'

        auto type = parse_type();

        std::unique_ptr<ExprAST> default_value = nullptr;
        if (peek().type == TokenType::EQUAL) {
            advance(); // consume '='
            default_value = parse_expression();
        }

        if (peek().type != TokenType::SEMICOLON) {
            throw std::runtime_error("Expected ';' after member variable declaration");
        }
        advance(); // consume ';'

        members.push_back(std::make_unique<MemberVarDeclAST>(member_name, std::move(type), is_mutable, std::move(default_value)));
    }

    advance(); // consume '}'

    return std::make_unique<StructDeclAST>(struct_name, std::move(members));
}

std::unique_ptr<StmtAST> Parser::parse_class_definition() {
    advance(); // consume 'class'
    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected class name after 'class'");
    }
    std::string class_name = advance().value;

    if (peek().type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' after class name");
    }
    advance(); // consume '{'

    std::vector<std::unique_ptr<MemberVarDeclAST>> members;
    std::vector<std::unique_ptr<FunctionDeclAST>> methods;

    while (peek().type != TokenType::RIGHT_BRACE) {
        if (peek().type == TokenType::LET) {
            advance(); // consume 'let'
            bool is_mutable = false;
            if (peek().type == TokenType::MUT) {
                is_mutable = true;
                advance(); // consume 'mut'
            }

            if (peek().type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected identifier for member variable name");
            }
            std::string member_name = advance().value;

            if (peek().type != TokenType::COLON) {
                throw std::runtime_error("Expected ':' after member variable name");
            }
            advance(); // consume ':'

            auto type = parse_type();

            std::unique_ptr<ExprAST> default_value = nullptr;
            if (peek().type == TokenType::EQUAL) {
                advance(); // consume '='
                default_value = parse_expression();
            }

            if (peek().type != TokenType::SEMICOLON) {
                throw std::runtime_error("Expected ';' after member variable declaration");
            }
            advance(); // consume ';'
            members.push_back(std::make_unique<MemberVarDeclAST>(member_name, std::move(type), is_mutable, std::move(default_value)));
        } else if (peek().type == TokenType::FN) {
            methods.push_back(std::unique_ptr<FunctionDeclAST>(static_cast<FunctionDeclAST*>(parse_function_definition().release())));
        } else {
            throw std::runtime_error("Expected 'let' or 'fn' in class body");
        }
    }

    advance(); // consume '}'

    return std::make_unique<ClassDeclAST>(class_name, std::move(members), std::move(methods));
}

std::unique_ptr<BlockStmtAST> Parser::parse_block() {
    if (peek().type != TokenType::LEFT_BRACE) {
        throw std::runtime_error("Expected '{' to start a block");
    }
    advance(); // consume '{'

    auto block = std::make_unique<BlockStmtAST>();
    while (peek().type != TokenType::RIGHT_BRACE && !is_at_end()) {
        block->statements.push_back(parse_statement());
    }

    if (peek().type != TokenType::RIGHT_BRACE) {
        throw std::runtime_error("Expected '}' to end a block");
    }
    advance(); // consume '}'
    return block;
}

std::unique_ptr<TypeNameAST> Parser::parse_type() {
    if (peek().type == TokenType::AMPERSAND) {
        advance(); // consume '&'
        bool is_mutable = false;
        if (peek().type == TokenType::MUT) {
            is_mutable = true;
            advance(); // consume 'mut'
        }
        return std::make_unique<ReferenceTypeAST>(parse_type(), is_mutable);
    }

    if (peek().type != TokenType::IDENTIFIER) {
        throw std::runtime_error("Expected a type name");
    }
    // This will need to be extended to handle complex types like arrays, etc.
    auto base_type = std::make_unique<TypeNameAST>(advance().value);
    if (peek().type == TokenType::LEFT_BRACKET) {
        advance(); // consume '['
        auto size = parse_expression();
        if (peek().type != TokenType::RIGHT_BRACKET) {
            throw std::runtime_error("Expected ']' after array size");
        }
        advance(); // consume ']'
        return std::make_unique<ArrayTypeAST>(std::move(base_type), std::move(size));
    }
    return base_type;
}


std::unique_ptr<ExprAST> Parser::parse_expression() {
    auto lhs = parse_primary();
    if (!lhs) {
        return nullptr;
    }
    return parse_binary_expression(0, std::move(lhs));
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
    auto expr = [this]() -> std::unique_ptr<ExprAST> {
        if (peek().type == TokenType::AMPERSAND) {
            advance(); // consume '&'
            bool is_mutable = false;
            if (peek().type == TokenType::MUT) {
                is_mutable = true;
                advance(); // consume 'mut'
            }
            return std::make_unique<BorrowExprAST>(parse_primary(), is_mutable);
        }

        if (peek().type == TokenType::INTEGER_LITERAL || peek().type == TokenType::FLOAT_LITERAL) {
            return std::make_unique<NumberExprAST>(std::stod(advance().value));
        }
        if (peek().type == TokenType::STRING_LITERAL) {
            return std::make_unique<StringExprAST>(advance().value);
        }
        if (peek().type == TokenType::TRUE) {
            advance();
            return std::make_unique<BoolExprAST>(true);
        }
        if (peek().type == TokenType::FALSE) {
            advance();
            return std::make_unique<BoolExprAST>(false);
        }
        if (peek().type == TokenType::IDENTIFIER) {
            auto var = std::make_unique<VariableExprAST>(advance().value);
            if (peek().type == TokenType::LEFT_BRACE) {
                advance(); // consume '{'
                std::vector<std::unique_ptr<MemberInitializerAST>> members;
                while (peek().type != TokenType::RIGHT_BRACE) {
                    if (peek().type == TokenType::IDENTIFIER && tokens[current_token_idx + 1].type == TokenType::COLON) {
                        std::string member_name = advance().value;
                        advance(); // consume ':'
                        members.push_back(std::make_unique<MemberInitializerAST>(parse_expression(), member_name));
                    } else {
                        members.push_back(std::make_unique<MemberInitializerAST>(parse_expression()));
                    }
                    if (peek().type == TokenType::COMMA) {
                        advance();
                    } else if (peek().type != TokenType::RIGHT_BRACE) {
                        throw std::runtime_error("Expected ',' or '}' in struct initializer");
                    }
                }
                advance(); // consume '}'
                return std::make_unique<StructInitializerExprAST>(var->name, std::move(members));
            }
            return var;
        }
        if (peek().type == TokenType::LEFT_PAREN) {
            advance(); // consume '('
            auto expr = parse_expression();
            if (peek().type != TokenType::RIGHT_PAREN) {
                throw std::runtime_error("Expected ')' after expression");
            }
            advance(); // consume ')'
            return expr;
        }
        if (peek().type == TokenType::LEFT_BRACKET) {
            advance(); // consume '['
            std::vector<std::unique_ptr<ExprAST>> elements;
            while (peek().type != TokenType::RIGHT_BRACKET) {
                elements.push_back(parse_expression());
                if (peek().type == TokenType::COMMA) {
                    advance();
                } else if (peek().type != TokenType::RIGHT_BRACKET) {
                    throw std::runtime_error("Expected ',' or ']' in array literal");
                }
            }
            advance(); // consume ']'
            return std::make_unique<ArrayLiteralExprAST>(std::move(elements));
        }
        return nullptr;
    }();

    if (expr && peek().type == TokenType::LEFT_PAREN) {
        return parse_call_expression(std::move(expr));
    }
    return expr;
}

std::unique_ptr<ExprAST> Parser::parse_call_expression(std::unique_ptr<ExprAST> callee) {
    std::vector<std::unique_ptr<ExprAST>> args;
    advance(); // consume '('
    while (peek().type != TokenType::RIGHT_PAREN) {
        args.push_back(parse_expression());
        if (peek().type == TokenType::COMMA) {
            advance(); // consume ','
        } else if (peek().type != TokenType::RIGHT_PAREN) {
            throw std::runtime_error("Expected ',' or ')' in argument list");
        }
    }
    advance(); // consume ')'

    if (auto* var = dynamic_cast<VariableExprAST*>(callee.get())) {
        return std::make_unique<FunctionCallExprAST>(var->name, std::move(args));
    }
    if (auto* memberAccess = dynamic_cast<MemberAccessExprAST*>(callee.get())) {
        return std::make_unique<FunctionCallExprAST>(std::move(callee), std::move(args));
    }

    throw std::runtime_error("Expected a variable name or member access for a function call");
}


int Parser::get_token_precedence() {
    switch (peek().type) {
        case TokenType::EQUAL:
            return 2;
        case TokenType::PLUS:
        case TokenType::MINUS:
            return 10;
        case TokenType::STAR:
        case TokenType::SLASH:
            return 20;
        case TokenType::LESS:
        case TokenType::LESS_EQUAL:
        case TokenType::GREATER:
        case TokenType::GREATER_EQUAL:
        case TokenType::DOUBLE_EQUAL:
        case TokenType::NOT_EQUAL:
            return 5;
        default:
            return -1;
    }
}

std::unique_ptr<ExprAST> Parser::parse_binary_expression(int expr_prec, std::unique_ptr<ExprAST> lhs) {
    while (true) {
        if (peek().type == TokenType::DOT) {
            advance(); // consume '.'
            if (peek().type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected identifier after '.'");
            }
            std::string memberName = advance().value;
            lhs = std::make_unique<MemberAccessExprAST>(std::move(lhs), memberName);
            continue;
        }

        if (peek().type == TokenType::LEFT_BRACKET) {
            advance(); // consume '['
            auto index = parse_expression();
            if (peek().type != TokenType::RIGHT_BRACKET) {
                throw std::runtime_error("Expected ']' after array index");
            }
            advance(); // consume ']'
            lhs = std::make_unique<ArrayIndexExprAST>(std::move(lhs), std::move(index));
            continue;
        }

        int tok_prec = get_token_precedence();
        if (tok_prec < expr_prec) {
            return lhs;
        }

        TokenType op = advance().type;
        auto rhs = parse_primary();
        if (!rhs) {
            return nullptr;
        }

        int next_prec = get_token_precedence();
        if (tok_prec < next_prec) {
            if (op == TokenType::EQUAL) { // Right-associative
                rhs = parse_binary_expression(tok_prec, std::move(rhs));
            } else { // Left-associative
                rhs = parse_binary_expression(tok_prec + 1, std::move(rhs));
            }
            if (!rhs) {
                return nullptr;
            }
        }
        lhs = std::make_unique<BinaryExprAST>(op, std::move(lhs), std::move(rhs));
    }
}
