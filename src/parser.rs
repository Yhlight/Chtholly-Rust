
use crate::ast::{Expression, Identifier, Program, Statement};
use crate::lexer::Lexer;
use crate::token::{Token, TokenKind};
use std::collections::HashMap;

type PrefixParseFn = fn(&mut Parser) -> Option<Expression>;

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
    prefix_parse_fns: HashMap<TokenKind, PrefixParseFn>,
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
        };
        p.register_prefix(TokenKind::Int, |parser| {
            let value = parser.current_token.literal.parse::<i64>().unwrap();
            Some(Expression::IntegerLiteral {
                token: parser.current_token.clone(),
                value,
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
            _ => self.parse_expression_statement(),
        }
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

    fn parse_expression(&mut self, _precedence: Precedence) -> Option<Expression> {
        let prefix = self.prefix_parse_fns.get(&self.current_token.kind)?;
        prefix(self)
    }

    fn parse_function_literal(&mut self) -> Option<Expression> {
        let token = self.current_token.clone();
        if !self.expect_peek(TokenKind::Ident) {
            return None;
        }
        let _name = Identifier {
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
        while self.current_token.kind != TokenKind::RBrace && self.current_token.kind != TokenKind::Eof {
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
}

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
}
