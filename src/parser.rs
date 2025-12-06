use crate::ast::{Expression, Identifier, Program, Statement};
use crate::lexer::Lexer;
use crate::token::{Token, TokenKind};
use std::collections::HashMap;

type PrefixParseFn = fn(&mut Parser) -> Option<Expression>;
type InfixParseFn = fn(&mut Parser, Expression) -> Option<Expression>;

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    And,         // &&
    Or,          // ||
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
}

fn precedents(token_kind: &TokenKind) -> Precedence {
    match token_kind {
        TokenKind::Eq => Precedence::Equals,
        TokenKind::NotEq => Precedence::Equals,
        TokenKind::Lt => Precedence::LessGreater,
        TokenKind::Gt => Precedence::LessGreater,
        TokenKind::LtEq => Precedence::LessGreater,
        TokenKind::GtEq => Precedence::LessGreater,
        TokenKind::Plus => Precedence::Sum,
        TokenKind::Minus => Precedence::Sum,
        TokenKind::Slash => Precedence::Product,
        TokenKind::Asterisk => Precedence::Product,
        TokenKind::And => Precedence::And,
        TokenKind::Or => Precedence::Or,
        _ => Precedence::Lowest,
    }
}

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
    prefix_parse_fns: HashMap<TokenKind, PrefixParseFn>,
    infix_parse_fns: HashMap<TokenKind, InfixParseFn>,
}

impl<'a> Parser<'a> {
    pub fn new(mut lexer: Lexer<'a>) -> Self {
        let current_token = lexer.next_token();
        let peek_token = lexer.next_token();
        let mut p = Self {
            lexer,
            current_token,
            peek_token,
            errors: Vec::new(),
            prefix_parse_fns: HashMap::new(),
            infix_parse_fns: HashMap::new(),
        };
        p.register_prefix(TokenKind::Ident, |parser| {
            Some(Expression::Identifier(Identifier {
                token: parser.current_token.clone(),
                value: parser.current_token.literal.clone(),
            }))
        });
        p.register_prefix(TokenKind::Int, |parser| {
            let value = match parser.current_token.literal.parse::<i64>() {
                Ok(v) => v,
                Err(_) => {
                    parser.errors.push(format!(
                        "could not parse {} as integer",
                        parser.current_token.literal
                    ));
                    return None;
                }
            };
            Some(Expression::IntegerLiteral {
                token: parser.current_token.clone(),
                value,
            })
        });
        p.register_prefix(TokenKind::True, |parser| {
            Some(Expression::BooleanLiteral {
                token: parser.current_token.clone(),
                value: true,
            })
        });
        p.register_prefix(TokenKind::False, |parser| {
            Some(Expression::BooleanLiteral {
                token: parser.current_token.clone(),
                value: false,
            })
        });
        p.register_prefix(TokenKind::String, |parser| {
            Some(Expression::StringLiteral {
                token: parser.current_token.clone(),
                value: parser.current_token.literal.clone(),
            })
        });
        p.register_prefix(TokenKind::Char, |parser| {
            let value = match parser.current_token.literal.chars().next() {
                Some(c) => c,
                None => {
                    parser.errors.push(format!(
                        "could not parse {} as char",
                        parser.current_token.literal
                    ));
                    return None;
                }
            };
            Some(Expression::CharLiteral {
                token: parser.current_token.clone(),
                value,
            })
        });
        p.register_prefix(TokenKind::Bang, |parser| {
            let token = parser.current_token.clone();
            parser.next_token();
            let right = parser.parse_expression(Precedence::Prefix)?;
            Some(Expression::PrefixExpression {
                token,
                operator: "!".to_string(),
                right: Box::new(right),
            })
        });
        p.register_prefix(TokenKind::If, |parser| {
            let token = parser.current_token.clone();
            if !parser.expect_peek(TokenKind::LParen) {
                return None;
            }
            parser.next_token();
            let condition = parser.parse_expression(Precedence::Lowest)?;
            if !parser.expect_peek(TokenKind::RParen) {
                return None;
            }
            if !parser.expect_peek(TokenKind::LBrace) {
                return None;
            }
            let consequence = parser.parse_block_statement()?;
            let mut alternative = None;
            if parser.peek_token.kind == TokenKind::Else {
                parser.next_token();
                if !parser.expect_peek(TokenKind::LBrace) {
                    return None;
                }
                alternative = Some(Box::new(parser.parse_block_statement()?));
            }
            Some(Expression::IfExpression {
                token,
                condition: Box::new(condition),
                consequence: Box::new(consequence),
                alternative,
            })
        });
        p.register_prefix(TokenKind::While, |parser| {
            let token = parser.current_token.clone();
            if !parser.expect_peek(TokenKind::LParen) {
                return None;
            }
            parser.next_token();
            let condition = parser.parse_expression(Precedence::Lowest)?;
            if !parser.expect_peek(TokenKind::RParen) {
                return None;
            }
            if !parser.expect_peek(TokenKind::LBrace) {
                return None;
            }
            let body = parser.parse_block_statement()?;
            Some(Expression::WhileExpression {
                token,
                condition: Box::new(condition),
                body: Box::new(body),
            })
        });

        p.register_infix(TokenKind::Plus, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "+".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::Minus, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "-".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::Asterisk, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "*".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::Slash, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "/".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::Eq, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "==".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::NotEq, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "!=".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::Lt, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "<".to_string(),
                right: Box::new(right),
            })
        });

        p.register_infix(TokenKind::Gt, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: ">".to_string(),
                right: Box::new(right),
            })
        });
        p.register_infix(TokenKind::GtEq, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: ">=".to_string(),
                right: Box::new(right),
            })
        });
        p.register_infix(TokenKind::LtEq, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "<=".to_string(),
                right: Box::new(right),
            })
        });
        p.register_infix(TokenKind::And, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "&&".to_string(),
                right: Box::new(right),
            })
        });
        p.register_infix(TokenKind::Or, |parser, left| {
            let token = parser.current_token.clone();
            let precedence = parser.cur_precedence();
            parser.next_token();
            let right = parser.parse_expression(precedence)?;
            Some(Expression::InfixExpression {
                token,
                left: Box::new(left),
                operator: "||".to_string(),
                right: Box::new(right),
            })
        });

        p
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };

        while self.current_token.kind != TokenKind::Eof {
            if let Some(stmt) = self.parse_statement() {
                program.statements.push(stmt);
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token.kind {
            TokenKind::Let => self.parse_let_statement(),
            TokenKind::Mut => self.parse_mut_statement(),
            TokenKind::Function => self.parse_function_statement(),
            TokenKind::Continue => self.parse_continue_statement(),
            TokenKind::Break => self.parse_break_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_continue_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();
        if self.peek_token.kind == TokenKind::Semicolon {
            self.next_token();
        }
        Some(Statement::Continue(token))
    }

    fn parse_break_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();
        if self.peek_token.kind == TokenKind::Semicolon {
            self.next_token();
        }
        Some(Statement::Break(token))
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();

        if !self.expect_peek(TokenKind::Ident) {
            return None;
        }

        let name = Identifier {
            token: self.current_token.clone(),
            value: self.current_token.literal.clone(),
        };

        if !self.expect_peek(TokenKind::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token.kind == TokenKind::Semicolon {
            self.next_token();
        }

        Some(Statement::Let { token, name, value })
    }

    fn parse_mut_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();

        if !self.expect_peek(TokenKind::Ident) {
            return None;
        }

        let name = Identifier {
            token: self.current_token.clone(),
            value: self.current_token.literal.clone(),
        };

        if !self.expect_peek(TokenKind::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token.kind == TokenKind::Semicolon {
            self.next_token();
        }

        Some(Statement::Mut { token, name, value })
    }

    fn parse_function_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_function_literal()?;
        Some(Statement::ExpressionStatement { expression })
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_expression(Precedence::Lowest)?;
        let stmt = Statement::ExpressionStatement { expression };
        if self.peek_token.kind == TokenKind::Semicolon {
            self.next_token();
        }
        Some(stmt)
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let prefix = self.prefix_parse_fns.get(&self.current_token.kind)?;
        let mut left_exp = prefix(self)?;

        while self.peek_token.kind != TokenKind::Semicolon && precedence < self.peek_precedence() {
            let infix = *self.infix_parse_fns.get(&self.peek_token.kind)?;
            self.next_token();
            left_exp = infix(self, left_exp)?;
        }

        Some(left_exp)
    }

    fn parse_function_literal(&mut self) -> Option<Expression> {
        let token = self.current_token.clone();
        if !self.expect_peek(TokenKind::Ident) {
            return None;
        }
        let name = Identifier {
            token: self.current_token.clone(),
            value: self.current_token.literal.clone(),
        };
        if !self.expect_peek(TokenKind::LParen) {
            return None;
        }
        let parameters = self.parse_function_parameters()?;
        if !self.expect_peek(TokenKind::LBrace) {
            return None;
        }
        let body = self.parse_block_statement()?;
        Some(Expression::FunctionLiteral {
            token,
            name,
            parameters,
            body: Box::new(body),
        })
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<Identifier>> {
        let mut identifiers = Vec::new();
        if self.peek_token.kind == TokenKind::RParen {
            self.next_token();
            return Some(identifiers);
        }
        self.next_token();
        let ident = Identifier {
            token: self.current_token.clone(),
            value: self.current_token.literal.clone(),
        };
        identifiers.push(ident);
        while self.peek_token.kind == TokenKind::Comma {
            self.next_token();
            self.next_token();
            let ident = Identifier {
                token: self.current_token.clone(),
                value: self.current_token.literal.clone(),
            };
            identifiers.push(ident);
        }
        if !self.expect_peek(TokenKind::RParen) {
            return None;
        }
        Some(identifiers)
    }

    fn parse_block_statement(&mut self) -> Option<Statement> {
        let mut statements = Vec::new();
        self.next_token();
        while self.current_token.kind != TokenKind::RBrace
            && self.current_token.kind != TokenKind::Eof
        {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
            }
            self.next_token();
        }
        Some(Statement::Block(statements))
    }

    fn expect_peek(&mut self, t: TokenKind) -> bool {
        if self.peek_token.kind == t {
            self.next_token();
            true
        } else {
            self.peek_error(t);
            false
        }
    }

    fn peek_error(&mut self, t: TokenKind) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            t, self.peek_token.kind
        );
        self.errors.push(msg);
    }

    fn register_prefix(&mut self, token_kind: TokenKind, f: PrefixParseFn) {
        self.prefix_parse_fns.insert(token_kind, f);
    }

    fn register_infix(&mut self, token_kind: TokenKind, f: InfixParseFn) {
        self.infix_parse_fns.insert(token_kind, f);
    }

    fn peek_precedence(&self) -> Precedence {
        precedents(&self.peek_token.kind)
    }

    fn cur_precedence(&self) -> Precedence {
        precedents(&self.current_token.kind)
    }
}
