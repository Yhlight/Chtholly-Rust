#include "parser.h"

Parser::Parser(Lexer& lexer) : lexer_(lexer) {
    // Read two tokens, so cur_token_ and peek_token_ are both set
    next_token();
    next_token();

    precedences = {
        {TokenType::EQ, EQUALS},
        {TokenType::NOT_EQ, EQUALS},
        {TokenType::LT, LESSGREATER},
        {TokenType::GT, LESSGREATER},
        {TokenType::PLUS, SUM},
        {TokenType::MINUS, SUM},
        {TokenType::SLASH, PRODUCT},
        {TokenType::ASTERISK, PRODUCT},
        {TokenType::LPAREN, CALL},
        {TokenType::INC, POSTFIX},
        {TokenType::DEC, POSTFIX},
    };
}

void Parser::next_token() {
    cur_token_ = peek_token_;
    peek_token_ = lexer_.next_token();
}

Precedence Parser::peek_precedence() {
    if (precedences.find(peek_token_.type) != precedences.end()) {
        return precedences[peek_token_.type];
    }
    return LOWEST;
}

Precedence Parser::cur_precedence() {
    if (precedences.find(cur_token_.type) != precedences.end()) {
        return precedences[cur_token_.type];
    }
    return LOWEST;
}


std::unique_ptr<Program> Parser::parse_program() {
    auto program = std::make_unique<Program>();
    while (cur_token_.type != TokenType::END_OF_FILE) {
        auto stmt = parse_statement();
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        next_token();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parse_statement() {
    if (cur_token_.type == TokenType::LET || cur_token_.type == TokenType::MUT) {
        return parse_var_declaration_statement();
    }
    if (cur_token_.type == TokenType::FN) {
        return parse_function_statement();
    }
    if (cur_token_.type == TokenType::WHILE) {
        return parse_while_statement();
    }
    if (cur_token_.type == TokenType::FOR) {
        return parse_for_statement();
    }

    auto stmt = std::make_unique<ExpressionStatement>();
    stmt->token = cur_token_;
    stmt->expression = parse_expression(LOWEST);

    if (peek_token_.type == TokenType::SEMICOLON) {
        next_token();
    }

    return stmt;
}

std::unique_ptr<VarDeclarationStatement> Parser::parse_var_declaration_statement() {
    auto stmt = std::make_unique<VarDeclarationStatement>();
    stmt->token = cur_token_;
    stmt->is_mutable = cur_token_.type == TokenType::MUT;

    if (peek_token_.type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    next_token();

    auto name = std::make_unique<Identifier>();
    name->token = cur_token_;
    name->value = cur_token_.literal;
    stmt->name = std::move(name);

    if (peek_token_.type == TokenType::COLON) {
        next_token();
        next_token();
        stmt->type = parse_type();
    }

    if (peek_token_.type != TokenType::ASSIGN) {
        return nullptr;
    }
    next_token();
    next_token();

    stmt->value = parse_expression(LOWEST);

    if (peek_token_.type != TokenType::SEMICOLON) {
        return nullptr;
    }
    next_token();

    return stmt;
}

std::unique_ptr<FunctionStatement> Parser::parse_function_statement() {
    auto func = std::make_unique<FunctionStatement>();
    func->token = cur_token_;

    if (peek_token_.type != TokenType::IDENTIFIER) {
        return nullptr;
    }
    next_token();

    auto name = std::make_unique<Identifier>();
    name->token = cur_token_;
    name->value = cur_token_.literal;
    func->name = std::move(name);

    if (peek_token_.type != TokenType::LPAREN) {
        return nullptr;
    }
    next_token();

    func->parameters = parse_function_parameters();

    if (peek_token_.type == TokenType::COLON) {
        next_token();
        next_token();
        func->return_type = parse_type();
    }

    if (peek_token_.type != TokenType::LBRACE) {
        return nullptr;
    }
    next_token();

    func->body = parse_block_statement();

    return func;
}

std::vector<std::pair<std::unique_ptr<Identifier>, std::unique_ptr<Type>>> Parser::parse_function_parameters() {
    std::vector<std::pair<std::unique_ptr<Identifier>, std::unique_ptr<Type>>> params;

    if (peek_token_.type == TokenType::RPAREN) {
        next_token();
        return params;
    }

    next_token();

    auto ident = std::make_unique<Identifier>();
    ident->token = cur_token_;
    ident->value = cur_token_.literal;

    if (peek_token_.type != TokenType::COLON) {
        return {};
    }
    next_token();
    next_token();

    auto type = parse_type();
    params.emplace_back(std::move(ident), std::move(type));

    while (peek_token_.type == TokenType::COMMA) {
        next_token();
        next_token();

        ident = std::make_unique<Identifier>();
        ident->token = cur_token_;
        ident->value = cur_token_.literal;

        if (peek_token_.type != TokenType::COLON) {
            return {};
        }
        next_token();
        next_token();

        type = parse_type();
        params.emplace_back(std::move(ident), std::move(type));
    }

    if (peek_token_.type != TokenType::RPAREN) {
        return {};
    }
    next_token();

    return params;
}

std::unique_ptr<WhileStatement> Parser::parse_while_statement() {
    auto stmt = std::make_unique<WhileStatement>();
    stmt->token = cur_token_;

    if (peek_token_.type != TokenType::LPAREN) {
        return nullptr;
    }
    next_token();
    next_token();

    stmt->condition = parse_expression(LOWEST);

    if (peek_token_.type != TokenType::RPAREN) {
        return nullptr;
    }
    next_token();

    if (peek_token_.type != TokenType::LBRACE) {
        return nullptr;
    }
    next_token();

    stmt->body = parse_block_statement();

    return stmt;
}

std::unique_ptr<ForStatement> Parser::parse_for_statement() {
    auto stmt = std::make_unique<ForStatement>();
    stmt->token = cur_token_;

    if (peek_token_.type != TokenType::LPAREN) {
        return nullptr;
    }
    next_token();
    next_token();

    stmt->init = parse_statement();
    next_token();

    stmt->condition = parse_expression(LOWEST);

    if (peek_token_.type != TokenType::SEMICOLON) {
        return nullptr;
    }
    next_token();
    next_token();

    stmt->increment = parse_expression(LOWEST);

    if (peek_token_.type != TokenType::RPAREN) {
        return nullptr;
    }
    next_token();

    if (peek_token_.type != TokenType::LBRACE) {
        return nullptr;
    }
    next_token();

    stmt->body = parse_block_statement();

    return stmt;
}

std::unique_ptr<BlockStatement> Parser::parse_block_statement() {
    auto block = std::make_unique<BlockStatement>();
    block->token = cur_token_;

    next_token();

    while (cur_token_.type != TokenType::RBRACE && cur_token_.type != TokenType::END_OF_FILE) {
        auto stmt = parse_statement();
        if (stmt) {
            block->statements.push_back(std::move(stmt));
        }
        next_token();
    }

    return block;
}

std::unique_ptr<Type> Parser::parse_type() {
    auto type = std::make_unique<Type>();
    type->token = cur_token_;
    type->name = cur_token_.literal;
    // Handle array types
    if (peek_token_.type == TokenType::LBRACKET) {
        next_token();
        if (peek_token_.type != TokenType::RBRACKET) {
            // TODO: handle static arrays
            return nullptr;
        }
        next_token();
        type->name += "[]";
    }
    return type;
}

std::unique_ptr<Expression> Parser::parse_expression(Precedence precedence) {
    std::unique_ptr<Expression> left_exp;
    if (cur_token_.type == TokenType::IDENTIFIER) {
        auto ident = std::make_unique<Identifier>();
        ident->token = cur_token_;
        ident->value = cur_token_.literal;
        left_exp = std::move(ident);
    }
    else if (cur_token_.type == TokenType::INTEGER) {
        auto lit = std::make_unique<IntegerLiteral>();
        lit->token = cur_token_;
        lit->value = std::stoll(cur_token_.literal);
        left_exp = std::move(lit);
    } else if (cur_token_.type == TokenType::TRUE || cur_token_.type == TokenType::FALSE) {
        auto bool_lit = std::make_unique<BooleanLiteral>();
        bool_lit->token = cur_token_;
        bool_lit->value = cur_token_.type == TokenType::TRUE;
        left_exp = std::move(bool_lit);
    } else if (cur_token_.type == TokenType::BANG || cur_token_.type == TokenType::MINUS) {
        auto prefix_exp = std::make_unique<PrefixExpression>();
        prefix_exp->token = cur_token_;
        next_token();
        prefix_exp->right = parse_expression(PREFIX);
        left_exp = std::move(prefix_exp);
    } else if (cur_token_.type == TokenType::IF) {
        auto if_exp = std::make_unique<IfExpression>();
        if_exp->token = cur_token_;
        if (peek_token_.type != TokenType::LPAREN) {
            return nullptr;
        }
        next_token();
        next_token();
        if_exp->condition = parse_expression(LOWEST);
        if (peek_token_.type != TokenType::RPAREN) {
            return nullptr;
        }
        next_token();
        if (peek_token_.type != TokenType::LBRACE) {
            return nullptr;
        }
        next_token();
        if_exp->consequence = parse_block_statement();
        if (peek_token_.type == TokenType::ELSE) {
            next_token();
            if (peek_token_.type != TokenType::LBRACE) {
                return nullptr;
            }
            next_token();
            if_exp->alternative = parse_block_statement();
        }
        left_exp = std::move(if_exp);
    } else {
        return nullptr;
    }

    while (peek_token_.type != TokenType::SEMICOLON && precedence < peek_precedence()) {
        if (peek_token_.type == TokenType::LPAREN) {
            next_token();
            auto call_exp = std::make_unique<CallExpression>();
            call_exp->token = cur_token_;
            call_exp->function = std::move(left_exp);
            call_exp->arguments = parse_call_arguments();
            left_exp = std::move(call_exp);
        } else if (peek_token_.type == TokenType::INC || peek_token_.type == TokenType::DEC) {
            next_token();
            auto postfix_exp = std::make_unique<PostfixExpression>();
            postfix_exp->token = cur_token_;
            postfix_exp->left = std::move(left_exp);
            left_exp = std::move(postfix_exp);
        } else {
            next_token();
            auto infix_exp = std::make_unique<InfixExpression>();
            infix_exp->token = cur_token_;
            infix_exp->left = std::move(left_exp);
            Precedence p = cur_precedence();
            next_token();
            infix_exp->right = parse_expression(p);
            left_exp = std::move(infix_exp);
        }
    }

    return left_exp;
}

std::vector<std::unique_ptr<Expression>> Parser::parse_call_arguments() {
    std::vector<std::unique_ptr<Expression>> args;

    if (peek_token_.type == TokenType::RPAREN) {
        next_token();
        return args;
    }

    next_token();
    args.push_back(parse_expression(LOWEST));

    while (peek_token_.type == TokenType::COMMA) {
        next_token();
        next_token();
        args.push_back(parse_expression(LOWEST));
    }

    if (peek_token_.type != TokenType::RPAREN) {
        return {};
    }
    next_token();

    return args;
}
