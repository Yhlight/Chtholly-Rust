use crate::lexer::lexer::Lexer;
use crate::lexer::token::Token;
use crate::ast::ast::{Program, Statement, Expression, Identifier};

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
}

pub struct Parser {
    lexer: Lexer,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl Parser {
    pub fn new(mut lexer: Lexer) -> Self {
        let current_token = lexer.next_token();
        let peek_token = lexer.next_token();
        Parser {
            lexer,
            current_token,
            peek_token,
            errors: Vec::new(),
        }
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program::new();

        while self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                program.statements.push(statement);
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let let_token = self.current_token.clone();

        if !self.expect_peek(&Token::Identifier("".to_string())) {
            return None;
        }

        let name = Identifier {
            token: self.current_token.clone(),
            value: match &self.current_token {
                Token::Identifier(s) => s.clone(),
                _ => return None,
            },
        };

        if !self.expect_peek(&Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::LetStatement {
            token: let_token,
            name,
            value,
        })
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();

        let expression = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::ExpressionStatement { token, expression })
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left_exp = self.parse_prefix()?;

        while !self.peek_token_is(&Token::Semicolon) && precedence < self.peek_precedence() {
            if let Some(infix_parser) = self.get_infix_parser() {
                self.next_token();
                left_exp = infix_parser(self, left_exp)?;
            } else {
                return Some(left_exp);
            }
        }

        Some(left_exp)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match self.current_token {
            Token::Identifier(_) => self.parse_identifier(),
            Token::Integer(_) => self.parse_integer_literal(),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            _ => None,
        }
    }

    fn parse_identifier(&self) -> Option<Expression> {
        Some(Expression::Identifier(Identifier {
            token: self.current_token.clone(),
            value: match &self.current_token {
                Token::Identifier(s) => s.clone(),
                _ => return None,
            },
        }))
    }

    fn parse_integer_literal(&self) -> Option<Expression> {
        if let Token::Integer(value) = self.current_token {
            Some(Expression::IntegerLiteral {
                token: self.current_token.clone(),
                value,
            })
        } else {
            None
        }
    }

    fn parse_prefix_expression(&mut self) -> Option<Expression> {
        let token = self.current_token.clone();
        let operator = match &self.current_token {
            Token::Bang => "!",
            Token::Minus => "-",
            _ => return None,
        };

        self.next_token();

        let right = self.parse_expression(Precedence::Prefix)?;

        Some(Expression::PrefixExpression {
            token,
            operator: operator.to_string(),
            right: Box::new(right),
        })
    }

    fn get_infix_parser(&self) -> Option<fn(&mut Parser, Expression) -> Option<Expression>> {
        match self.peek_token {
            Token::Plus | Token::Minus | Token::Slash | Token::Asterisk | Token::Eq | Token::NotEq | Token::Lt | Token::Gt => {
                Some(Parser::parse_infix_expression)
            }
            _ => None,
        }
    }

    fn parse_infix_expression(&mut self, left: Expression) -> Option<Expression> {
        let token = self.current_token.clone();
        let operator = match &self.current_token {
            Token::Plus => "+",
            Token::Minus => "-",
            Token::Slash => "/",
            Token::Asterisk => "*",
            Token::Eq => "==",
            Token::NotEq => "!=",
            Token::Lt => "<",
            Token::Gt => ">",
            _ => return None,
        };

        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Expression::InfixExpression {
            token,
            left: Box::new(left),
            operator: operator.to_string(),
            right: Box::new(right),
        })
    }

    fn peek_precedence(&self) -> Precedence {
        self.get_precedence(&self.peek_token)
    }

    fn current_precedence(&self) -> Precedence {
        self.get_precedence(&self.current_token)
    }

    fn get_precedence(&self, token: &Token) -> Precedence {
        match token {
            Token::Eq | Token::NotEq => Precedence::Equals,
            Token::Lt | Token::Gt => Precedence::LessGreater,
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Slash | Token::Asterisk => Precedence::Product,
            _ => Precedence::Lowest,
        }
    }

    fn peek_token_is(&self, token_type: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(token_type)
    }

    fn expect_peek(&mut self, token_type: &Token) -> bool {
        if self.peek_token_is(token_type) {
            self.next_token();
            true
        } else {
            self.peek_error(token_type);
            false
        }
    }

    fn peek_error(&mut self, token_type: &Token) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            token_type, self.peek_token
        );
        self.errors.push(msg);
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }
}
