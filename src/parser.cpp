#include "parser.h"
#include <stdexcept>
#include <variant>
#include <iostream>

namespace Chtholly
{
    Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)) {}

    std::vector<std::shared_ptr<Stmt>> Parser::parse()
    {
        std::vector<std::shared_ptr<Stmt>> statements;
        while (!isAtEnd())
        {
            statements.push_back(declaration());
        }
        return statements;
    }

    void Parser::parseError(const Token& token, const std::string& message) {
        hadError_ = true;
        if (token.type == TokenType::END_OF_FILE) {
            std::cerr << "[line " << token.line << "] Error at end: " << message << std::endl;
        } else {
            std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << std::endl;
        }
    }

    std::shared_ptr<Stmt> Parser::declaration()
    {
        try
        {
            if (match({TokenType::CLASS})) return classDeclaration();
            if (match({TokenType::ENUM})) return enumDeclaration();
            if (match({TokenType::FN})) return functionDeclaration();
            if (match({TokenType::STRUCT})) return structDeclaration();
            if (match({TokenType::LET})) {
                bool isMutable = match({TokenType::MUT});
                auto letStmt = letDeclaration(isMutable);
                consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
                return letStmt;
            }
            return statement();
        }
        catch (const std::runtime_error& error)
        {
            synchronize();
            return nullptr;
        }
    }

    Token Parser::parseType()
    {
        std::string typeString = "";
        Token startToken = peek();
        bool isRef = false;

        if (match({TokenType::AMPERSAND}))
        {
            typeString += "&";
            isRef = true;
            if (match({TokenType::MUT}))
            {
                typeString += "mut ";
            }
        }

        if (match({TokenType::IDENTIFIER, TokenType::I32, TokenType::F64, TokenType::BOOL, TokenType::CHAR, TokenType::STRING, TokenType::VOID}))
        {
            Token typeNameToken = previous();
            typeString += typeNameToken.lexeme;

            if (isRef) {
                return Token{TokenType::IDENTIFIER, typeString, std::monostate{}, startToken.line};
            }
            return typeNameToken;
        }

        parseError(peek(), "Expect type annotation.");
        return previous();
    }


    std::shared_ptr<Stmt> Parser::letDeclaration(bool isMutable)
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");

        Token type;
        bool hasType = false;
        if (match({TokenType::COLON}))
        {
            type = parseType();
            hasType = true;
        }

        std::shared_ptr<Expr> initializer = nullptr;
        if (match({TokenType::EQUAL}))
        {
            initializer = expression();
        }

        if (!hasType && !initializer)
        {
            parseError(name, "Variable declaration must have a type annotation or an initializer.");
        }

        return std::make_shared<LetStmt>(name, type, initializer, isMutable);
    }

    std::shared_ptr<Stmt> Parser::statement()
    {
        if (match({TokenType::IF})) return ifStatement();
        if (match({TokenType::SWITCH})) return switchStatement();
        if (match({TokenType::WHILE})) return whileStatement();
        if (match({TokenType::FOR})) return forStatement();
        if (match({TokenType::RETURN})) return returnStatement();
        if (match({TokenType::BREAK}))
        {
            Token keyword = previous();
            consume(TokenType::SEMICOLON, "Expect ';' after 'break'.");
            return std::make_shared<BreakStmt>(keyword);
        }
        if (match({ TokenType::CONTINUE }))
        {
            Token keyword = previous();
            consume(TokenType::SEMICOLON, "Expect ';' after 'continue'.");
            return std::make_shared<ContinueStmt>(keyword);
        }
        if (match({TokenType::FALLTHROUGH}))
        {
            Token keyword = previous();
            consume(TokenType::SEMICOLON, "Expect ';' after 'fallthrough'.");
            return std::make_shared<FallthroughStmt>(keyword);
        }
        if (match({TokenType::LEFT_BRACE})) return std::make_shared<BlockStmt>(block());
        return expressionStatement();
    }

    std::shared_ptr<Stmt> Parser::ifStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

        std::shared_ptr<Stmt> thenBranch = statement();
        std::shared_ptr<Stmt> elseBranch = nullptr;
        if (match({TokenType::ELSE}))
        {
            elseBranch = statement();
        }

        return std::make_shared<IfStmt>(condition, thenBranch, elseBranch);
    }

    std::shared_ptr<Stmt> Parser::switchStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'switch'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after switch condition.");

        consume(TokenType::LEFT_BRACE, "Expect '{' before switch cases.");

        std::vector<std::shared_ptr<CaseStmt>> cases;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            if (match({TokenType::CASE}))
            {
                std::shared_ptr<Expr> caseCondition = expression();
                consume(TokenType::COLON, "Expect ':' after case condition.");
                std::shared_ptr<Stmt> body = statement();
                cases.push_back(std::make_shared<CaseStmt>(caseCondition, body));
            }
            else if (match({TokenType::DEFAULT}))
            {
                consume(TokenType::COLON, "Expect ':' after 'default'.");
                std::shared_ptr<Stmt> body = statement();
                cases.push_back(std::make_shared<CaseStmt>(nullptr, body));
            }
            else
            {
                parseError(peek(), "Expect 'case' or 'default'.");
                break;
            }
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after switch cases.");

        return std::make_shared<SwitchStmt>(condition, cases);
    }

    std::shared_ptr<Stmt> Parser::whileStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        std::shared_ptr<Expr> condition = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        std::shared_ptr<Stmt> body = statement();

        return std::make_shared<WhileStmt>(condition, body);
    }

    std::shared_ptr<Stmt> Parser::forStatement()
    {
        consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");

        std::shared_ptr<Stmt> initializer;
        if (match({TokenType::SEMICOLON}))
        {
            initializer = nullptr;
        }
        else if (match({TokenType::LET}))
        {
            bool isMutable = match({TokenType::MUT});
            initializer = letDeclaration(isMutable);
            consume(TokenType::SEMICOLON, "Expect ';' after for loop initializer.");
        }
        else
        {
            initializer = expressionStatement();
        }

        std::shared_ptr<Expr> condition = nullptr;
        if (!check(TokenType::SEMICOLON))
        {
            condition = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

        std::shared_ptr<Expr> increment = nullptr;
        if (!check(TokenType::RIGHT_PAREN))
        {
            increment = expression();
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

        std::shared_ptr<Stmt> body = statement();

        return std::make_shared<ForStmt>(initializer, condition, increment, body);
    }

    std::shared_ptr<Stmt> Parser::returnStatement()
    {
        Token keyword = previous();
        std::shared_ptr<Expr> value = nullptr;
        if (!check(TokenType::SEMICOLON))
        {
            value = expression();
        }
        consume(TokenType::SEMICOLON, "Expect ';' after return value.");
        return std::make_shared<ReturnStmt>(keyword, value);
    }

    std::vector<std::shared_ptr<Stmt>> Parser::block()
    {
        std::vector<std::shared_ptr<Stmt>> statements;

        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            statements.push_back(declaration());
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return statements;
    }

    std::shared_ptr<Stmt> Parser::expressionStatement()
    {
        std::shared_ptr<Expr> expr = expression();
        consume(TokenType::SEMICOLON, "Expect ';' after expression.");
        return std::make_shared<ExpressionStmt>(expr);
    }

    std::shared_ptr<Expr> Parser::expression()
    {
        return assignment();
    }

    std::shared_ptr<Expr> Parser::assignment()
    {
        std::shared_ptr<Expr> expr = equality();

        if (match({TokenType::EQUAL}))
        {
            Token equals = previous();
            std::shared_ptr<Expr> value = assignment();

            if (auto varExpr = std::dynamic_pointer_cast<VariableExpr>(expr))
            {
                Token name = varExpr->name;
                return std::make_shared<AssignExpr>(name, value);
            }
        else if (auto getExpr = std::dynamic_pointer_cast<GetExpr>(expr))
        {
            return std::make_shared<SetExpr>(getExpr->object, getExpr->name, value);
        }
            parseError(equals, "Invalid assignment target.");
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::equality()
    {
        std::shared_ptr<Expr> expr = comparison();

        while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = comparison();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::comparison()
    {
        std::shared_ptr<Expr> expr = term();

        while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = term();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::term()
    {
        std::shared_ptr<Expr> expr = factor();

        while (match({TokenType::MINUS, TokenType::PLUS}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = factor();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::factor()
    {
        std::shared_ptr<Expr> expr = unary();

        while (match({TokenType::SLASH, TokenType::STAR}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = unary();
            expr = std::make_shared<BinaryExpr>(expr, op, right);
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::unary()
    {
        if (match({TokenType::BANG, TokenType::MINUS}))
        {
            Token op = previous();
            std::shared_ptr<Expr> right = unary();
            return std::make_shared<UnaryExpr>(op, right);
        }

        if (match({TokenType::AMPERSAND}))
        {
            Token op = previous();
            bool isMutable = match({TokenType::MUT});
            std::shared_ptr<Expr> right = unary();
            return std::make_shared<UnaryExpr>(op, right, isMutable);
        }

        return call();
    }

    std::shared_ptr<Expr> Parser::primary()
    {
        if (match({TokenType::FALSE})) return std::make_shared<LiteralExpr>(false);
        if (match({TokenType::TRUE})) return std::make_shared<LiteralExpr>(true);
        if (match({TokenType::SELF})) return std::make_shared<ThisExpr>(previous());

        if (match({TokenType::INTEGER, TokenType::FLOAT, TokenType::STRING, TokenType::CHAR}))
        {
            return std::make_shared<LiteralExpr>(std::visit(
                [](auto&& arg) -> std::variant<std::monostate, int, double, std::string, char, bool> { return arg; },
                previous().literal
            ));
        }

        if (match({TokenType::IDENTIFIER}))
        {
            Token name = previous();
            if (match({TokenType::LEFT_BRACE}))
            {
                std::vector<std::pair<Token, std::shared_ptr<Expr>>> initializers;
                while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
                {
                    Token fieldName = consume(TokenType::IDENTIFIER, "Expect field name in initializer.");
                    consume(TokenType::COLON, "Expect ':' after field name.");
                    std::shared_ptr<Expr> value = expression();
                    initializers.push_back({fieldName, value});
                    if (!match({TokenType::COMMA}))
                    {
                        break;
                    }
                }
                consume(TokenType::RIGHT_BRACE, "Expect '}' after struct initializer.");
                return std::make_shared<StructInitializerExpr>(name, initializers);
            }
            return std::make_shared<VariableExpr>(name);
        }

        if (match({TokenType::LEFT_PAREN}))
        {
            std::shared_ptr<Expr> expr = expression();
            consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
            return expr;
        }

        parseError(peek(), "Expect expression.");
        throw std::runtime_error("Expect expression.");
    }

    std::shared_ptr<Expr> Parser::call()
    {
        std::shared_ptr<Expr> expr = primary();

        while (true)
        {
            if (match({TokenType::LEFT_PAREN}))
            {
                expr = finishCall(expr);
            }
            else if (match({TokenType::DOT}))
            {
                Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
                expr = std::make_shared<GetExpr>(expr, name);
            }
            else
            {
                break;
            }
        }

        return expr;
    }

    std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> callee)
    {
        std::vector<std::shared_ptr<Expr>> arguments;
        if (!check(TokenType::RIGHT_PAREN))
        {
            do
            {
                if (arguments.size() >= 255)
                {
                    parseError(peek(), "Can't have more than 255 arguments.");
                }
                arguments.push_back(expression());
            } while (match({TokenType::COMMA}));
        }

        Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");

        return std::make_shared<CallExpr>(callee, paren, arguments);
    }

    bool Parser::match(const std::vector<TokenType>& types)
    {
        for (TokenType type : types)
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }
        return false;
    }

    Token Parser::consume(TokenType type, const std::string& message)
    {
        if (check(type)) return advance();
        parseError(peek(), message);
        throw std::runtime_error(message);
    }

    bool Parser::check(TokenType type) const
    {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    Token Parser::advance()
    {
        if (!isAtEnd()) current++;
        return previous();
    }

    bool Parser::isAtEnd() const
    {
        return peek().type == TokenType::END_OF_FILE;
    }

    Token Parser::peek() const
    {
        return tokens[current];
    }

    Token Parser::previous() const
    {
        return tokens[current - 1];
    }

    void Parser::synchronize()
    {
        advance();
        while (!isAtEnd())
        {
            if (previous().type == TokenType::SEMICOLON) return;
            switch (peek().type)
            {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            }
            advance();
        }
    }

    std::shared_ptr<Stmt> Parser::functionDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect function name.");
        consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
        std::vector<Token> parameters;
        std::vector<Token> parameterTypes;
        if (!check(TokenType::RIGHT_PAREN))
        {
            do
            {
                if (parameters.size() >= 255)
                {
                    parseError(peek(), "Can't have more than 255 parameters.");
                }
                parameters.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
                consume(TokenType::COLON, "Expect ':' after parameter name.");
                parameterTypes.push_back(parseType());
            } while (match({TokenType::COMMA}));
        }
        consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");

        consume(TokenType::COLON, "Expect ':' before return type.");
        Token returnType = parseType();

        consume(TokenType::LEFT_BRACE, "Expect '{' before function body.");
        std::shared_ptr<BlockStmt> body = std::make_shared<BlockStmt>(block());
        return std::make_shared<FunctionStmt>(name, parameters, parameterTypes, returnType, body);
    }

    std::shared_ptr<Stmt> Parser::structDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect struct name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' before struct body.");

        std::vector<std::shared_ptr<LetStmt>> fields;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            if (match({TokenType::LET}))
            {
                bool isMutable = match({TokenType::MUT});
                fields.push_back(std::dynamic_pointer_cast<LetStmt>(letDeclaration(isMutable)));
                consume(TokenType::SEMICOLON, "Expect ';' after field declaration.");
            }
            else
            {
                parseError(peek(), "Expect 'let' in struct body.");
                break;
            }
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");
        return std::make_shared<StructStmt>(name, fields);
    }

    std::shared_ptr<Stmt> Parser::classDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");

        std::vector<std::shared_ptr<LetStmt>> fields;
        std::vector<std::shared_ptr<FunctionStmt>> methods;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            if (match({TokenType::LET}))
            {
                bool isMutable = match({TokenType::MUT});
                fields.push_back(std::dynamic_pointer_cast<LetStmt>(letDeclaration(isMutable)));
                consume(TokenType::SEMICOLON, "Expect ';' after field declaration.");
            }
            else if (match({TokenType::FN}))
            {
                methods.push_back(std::dynamic_pointer_cast<FunctionStmt>(functionDeclaration()));
            }
            else
            {
                parseError(peek(), "Expect 'let' or 'fn' in class body.");
                break;
            }
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
        return std::make_shared<ClassStmt>(name, fields, methods);
    }

    std::shared_ptr<Stmt> Parser::enumDeclaration()
    {
        Token name = consume(TokenType::IDENTIFIER, "Expect enum name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' before enum body.");

        std::vector<Token> variants;
        std::vector<std::vector<Token>> variantTypes;
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd())
        {
            variants.push_back(consume(TokenType::IDENTIFIER, "Expect variant name."));
            if (match({TokenType::LEFT_PAREN}))
            {
                std::vector<Token> types;
                do
                {
                    types.push_back(parseType());
                } while (match({TokenType::COMMA}));
                consume(TokenType::RIGHT_PAREN, "Expect ')' after variant types.");
                variantTypes.push_back(types);
            }
            else
            {
                variantTypes.push_back({});
            }

            if (!match({TokenType::COMMA}))
            {
                break;
            }
        }

        consume(TokenType::RIGHT_BRACE, "Expect '}' after enum body.");
        return std::make_shared<EnumStmt>(name, variants, variantTypes);
    }

} // namespace Chtholly
