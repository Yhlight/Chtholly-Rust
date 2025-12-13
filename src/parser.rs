use crate::ast::{Program, Statement, Expression, Parameter};
use crate::lexer::{Lexer, Token};

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
            errors: Vec::new(),
        };

        parser.next_token();
        parser.next_token();

        parser
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Vec::new();

        while self.current_token != Token::Eof {
            match self.parse_statement() {
                Some(statement) => program.push(statement),
                None => (),
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            Token::Fn => self.parse_fn_statement(),
            Token::Return => self.parse_return_statement(),
            Token::If => self.parse_if_statement(),
            Token::While => self.parse_while_statement(),
            Token::Break => Some(Statement::Break),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_fn_statement(&mut self) -> Option<Statement> {
        if !self.peek_token_is(&Token::Identifier("".to_string())) {
            self.peek_error(&Token::Identifier("".to_string()));
            return None;
        }
        self.next_token();

        let name = if let Token::Identifier(name) = self.current_token.clone() {
            name
        } else {
            return None;
        };

        if !self.peek_token_is(&Token::LParen) {
            self.peek_error(&Token::LParen);
            return None;
        }
        self.next_token();

        let parameters = self.parse_function_parameters()?;

        if !self.peek_token_is(&Token::Colon) {
            self.peek_error(&Token::Colon);
            return None;
        }
        self.next_token();
        self.next_token();

        let return_type = if let Token::Identifier(name) = self.current_token.clone() {
            name
        } else {
            return None;
        };

        if !self.peek_token_is(&Token::LBrace) {
            self.peek_error(&Token::LBrace);
            return None;
        }
        self.next_token();

        let body = self.parse_block_statement()?;

        Some(Statement::Function {
            name,
            parameters,
            return_type,
            body: Box::new(body),
        })
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<Parameter>> {
        let mut parameters = Vec::new();

        if self.peek_token_is(&Token::RParen) {
            self.next_token();
            return Some(parameters);
        }

        self.next_token();

        loop {
            let name = if let Token::Identifier(name) = self.current_token.clone() {
                name
            } else {
                return None;
            };

            if !self.peek_token_is(&Token::Colon) {
                self.peek_error(&Token::Colon);
                return None;
            }
            self.next_token();
            self.next_token();

            let type_annotation = if let Token::Identifier(name) = self.current_token.clone() {
                name
            } else {
                return None;
            };

            parameters.push(Parameter {
                name,
                type_annotation,
            });

            if !self.peek_token_is(&Token::Comma) {
                break;
            }
            self.next_token();
            self.next_token();
        }

        if !self.peek_token_is(&Token::RParen) {
            self.peek_error(&Token::RParen);
            return None;
        }
        self.next_token();

        Some(parameters)
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let mutable = if self.peek_token_is(&Token::Mut) {
            self.next_token();
            true
        } else {
            false
        };

        if !self.peek_token_is(&Token::Identifier("".to_string())) {
            self.peek_error(&Token::Identifier("".to_string()));
            return None;
        }
        self.next_token();

        let name = if let Token::Identifier(name) = self.current_token.clone() {
            name
        } else {
            return None;
        };

        if !self.peek_token_is(&Token::Equal) {
            self.peek_error(&Token::Equal);
            return None;
        }
        self.next_token();

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Let { name, mutable, value })
    }

    fn parse_return_statement(&mut self) -> Option<Statement> {
        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Return(value))
    }

    fn parse_if_statement(&mut self) -> Option<Statement> {
        if !self.peek_token_is(&Token::LParen) {
            self.peek_error(&Token::LParen);
            return None;
        }
        self.next_token();
        self.next_token();

        let condition = self.parse_expression(Precedence::Lowest)?;

        if !self.peek_token_is(&Token::RParen) {
            self.peek_error(&Token::RParen);
            return None;
        }
        self.next_token();

        if !self.peek_token_is(&Token::LBrace) {
            self.peek_error(&Token::LBrace);
            return None;
        }
        self.next_token();

        let consequence = self.parse_block_statement()?;

        let alternative = if self.peek_token_is(&Token::Else) {
            self.next_token();
            if !self.peek_token_is(&Token::LBrace) {
                self.peek_error(&Token::LBrace);
                return None;
            }
            self.next_token();
            Some(Box::new(self.parse_block_statement()?))
        } else {
            None
        };

        Some(Statement::If {
            condition,
            consequence: Box::new(consequence),
            alternative,
        })
    }

    fn parse_while_statement(&mut self) -> Option<Statement> {
        if !self.peek_token_is(&Token::LParen) {
            self.peek_error(&Token::LParen);
            return None;
        }
        self.next_token();
        self.next_token();

        let condition = self.parse_expression(Precedence::Lowest)?;

        if !self.peek_token_is(&Token::RParen) {
            self.peek_error(&Token::RParen);
            return None;
        }
        self.next_token();

        if !self.peek_token_is(&Token::LBrace) {
            self.peek_error(&Token::LBrace);
            return None;
        }
        self.next_token();

        let body = self.parse_block_statement()?;

        Some(Statement::While {
            condition,
            body: Box::new(body),
        })
    }

    fn parse_block_statement(&mut self) -> Option<Statement> {
        let mut statements = Vec::new();
        self.next_token();

        while !self.current_token_is(Token::RBrace) && !self.current_token_is(Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
            }
            self.next_token();
        }

        Some(Statement::Block(statements))
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Expression(expression))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left = self.parse_prefix()?;

        while self.peek_token != Token::Semicolon && precedence < self.peek_precedence() {
            self.next_token();
            left = self.parse_infix(left.clone())?;
        }

        Some(left)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match self.current_token.clone() {
            Token::Identifier(name) => Some(Expression::Identifier(name)),
            Token::Integer(value) => Some(Expression::Integer(value)),
            Token::Float(value) => Some(Expression::Float(value)),
            Token::String(value) => Some(Expression::String(value)),
            Token::Char(value) => Some(Expression::Char(value)),
            Token::True => Some(Expression::Boolean(true)),
            Token::False => Some(Expression::Boolean(false)),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            _ => None,
        }
    }

    fn parse_infix(&mut self, left: Expression) -> Option<Expression> {
        match self.current_token {
            Token::Equal => {
                if let Expression::Identifier(name) = left {
                    let precedence = self.current_precedence();
                    self.next_token();
                    let value = self.parse_expression(precedence)?;
                    Some(Expression::Assign {
                        name,
                        value: Box::new(value),
                    })
                } else {
                    None
                }
            }
            _ => {
                let token = self.current_token.clone();
                let precedence = self.current_precedence();
                self.next_token();
                let right = self.parse_expression(precedence)?;
                Some(Expression::Infix(Box::new(left), token, Box::new(right)))
            }
        }
    }

    fn parse_prefix_expression(&mut self) -> Option<Expression> {
        let token = self.current_token.clone();
        self.next_token();
        let right = self.parse_expression(Precedence::Prefix)?;
        Some(Expression::Prefix(token, Box::new(right)))
    }

    fn current_token_is(&self, token: Token) -> bool {
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(&token)
    }

    fn peek_token_is(&self, token: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(token)
    }

    fn peek_error(&mut self, token: &Token) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            token, self.peek_token
        );
        self.errors.push(msg);
    }

    fn peek_precedence(&self) -> Precedence {
        token_to_precedence(&self.peek_token)
    }

    fn current_precedence(&self) -> Precedence {
        token_to_precedence(&self.current_token)
    }
}

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Assign,
    Equals,
    LessGreater,
    Sum,
    Product,
    Prefix,
    Call,
}

fn token_to_precedence(token: &Token) -> Precedence {
    match token {
        Token::Equal => Precedence::Assign,
        Token::EqualEqual | Token::NotEqual => Precedence::Equals,
        Token::LessThan | Token::GreaterThan | Token::LessThanOrEqual | Token::GreaterThanOrEqual => Precedence::LessGreater,
        Token::Plus | Token::Minus => Precedence::Sum,
        Token::Slash | Token::Asterisk => Precedence::Product,
        Token::LParen => Precedence::Call,
        _ => Precedence::Lowest,
    }
}
