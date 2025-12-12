use crate::ast::{Program, Statement, Expression};
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
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
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

    fn peek_token_is(&self, token: &Token) -> bool {
        matches!(
            (&self.peek_token, token),
            (Token::Identifier(_), Token::Identifier(_))
                | (Token::Integer(_), Token::Integer(_))
                | (Token::Float(_), Token::Float(_))
                | (Token::String(_), Token::String(_))
                | (Token::Char(_), Token::Char(_))
        ) || self.peek_token == *token
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
