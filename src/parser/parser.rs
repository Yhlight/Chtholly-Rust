use crate::lexer::lexer::Lexer;
use crate::lexer::token::Token;
use crate::ast::ast::{Program, Statement, Expression, Identifier, BlockStatement};

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
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let let_token = self.current_token.clone();

        if !matches!(self.peek_token, Token::Identifier(_)) {
            self.peek_error(&Token::Identifier("identifier".to_string()));
            return None;
        }
        self.next_token();

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

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::LetStatement {
            token: let_token,
            name,
            value,
        })
    }

    fn parse_return_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();

        self.next_token();

        let return_value = self.parse_expression(Precedence::Lowest)?;

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::ReturnStatement {
            token,
            return_value,
        })
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let token = self.current_token.clone();

        let expression = self.parse_expression(Precedence::Lowest)?;

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::ExpressionStatement { token, expression })
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left_exp = self.parse_prefix()?;

        while !matches!(self.peek_token, Token::Semicolon) && precedence < self.peek_precedence() {
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
            Token::True | Token::False => self.parse_boolean(),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            Token::If => self.parse_if_expression(),
            Token::LParen => self.parse_grouped_expression(),
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

    fn parse_boolean(&self) -> Option<Expression> {
        Some(Expression::Boolean {
            token: self.current_token.clone(),
            value: matches!(self.current_token, Token::True),
        })
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

    fn parse_if_expression(&mut self) -> Option<Expression> {
        let token = self.current_token.clone();

        if !self.expect_peek(&Token::LParen) {
            return None;
        }

        self.next_token();
        let condition = self.parse_expression(Precedence::Lowest)?;

        if !self.expect_peek(&Token::RParen) {
            return None;
        }

        if !self.expect_peek(&Token::LBrace) {
            return None;
        }

        let consequence = self.parse_block_statement();

        let mut alternative = None;
        if matches!(self.peek_token, Token::Else) {
            self.next_token();

            if !self.expect_peek(&Token::LBrace) {
                return None;
            }

            alternative = Some(self.parse_block_statement());
        }

        Some(Expression::IfExpression {
            token,
            condition: Box::new(condition),
            consequence,
            alternative,
        })
    }

    fn parse_grouped_expression(&mut self) -> Option<Expression> {
        self.next_token();

        let exp = self.parse_expression(Precedence::Lowest);

        if !self.expect_peek(&Token::RParen) {
            return None;
        }

        exp
    }

    fn parse_block_statement(&mut self) -> BlockStatement {
        let token = self.current_token.clone();
        let mut statements = Vec::new();

        self.next_token();

        while !matches!(self.current_token, Token::RBrace) && !matches!(self.current_token, Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
            }
            self.next_token();
        }

        BlockStatement { token, statements }
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

    fn expect_peek(&mut self, token_type: &Token) -> bool {
        if self.peek_token == *token_type {
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

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }
}
