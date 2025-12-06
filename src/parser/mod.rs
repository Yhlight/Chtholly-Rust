use crate::ast::{BlockStatement, Expression, Infix, Prefix, Statement};
use crate::lexer::{token::Token, Lexer};

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
    Call,        // myFunction(X)
    Index,       // array[index]
}

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
            errors: vec![],
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

    pub fn parse_program(&mut self) -> BlockStatement {
        let mut statements = vec![];
        while self.current_token != Token::Eof {
            match self.parse_statement() {
                Some(statement) => statements.push(statement),
                None => (),
            }
            self.next_token();
        }
        statements
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            Token::Mut => self.parse_mut_statement(),
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }
        let name = match &self.current_token {
            Token::Identifier(name) => name.clone(),
            _ => return None,
        };
        if !self.expect_peek(Token::Assign) {
            return None;
        }
        self.next_token();
        let value = self.parse_expression(Precedence::Lowest)?;
        if self.peek_token == Token::Semicolon {
            self.next_token();
        }
        Some(Statement::Let(name, value))
    }

    fn parse_mut_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }
        let name = match &self.current_token {
            Token::Identifier(name) => name.clone(),
            _ => return None,
        };
        if !self.expect_peek(Token::Assign) {
            return None;
        }
        self.next_token();
        let value = self.parse_expression(Precedence::Lowest)?;
        if self.peek_token == Token::Semicolon {
            self.next_token();
        }
        Some(Statement::Mut(name, value))
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
            match self.peek_token {
            Token::Plus
            | Token::Minus
            | Token::Star
            | Token::Slash
            | Token::Percent
            | Token::Equal
            | Token::NotEqual
            | Token::LessThan
            | Token::LessThanOrEqual
            | Token::GreaterThan
            | Token::GreaterThanOrEqual
            | Token::LParen
            | Token::LBracket => {
                    self.next_token();
                    left = self.parse_infix_expression(left)?;
                }
                _ => return Some(left),
            }
        }

        Some(left)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match &self.current_token {
            Token::Identifier(name) => Some(Expression::Identifier(name.clone())),
            Token::Int(value) => Some(Expression::Int(*value)),
            Token::Double(value) => Some(Expression::Double(*value)),
            Token::String(value) => Some(Expression::String(value.clone())),
            Token::Char(value) => Some(Expression::Char(*value)),
            Token::Bool(value) => Some(Expression::Bool(*value)),
            Token::Not | Token::Minus => self.parse_prefix_expression(),
            Token::LParen => self.parse_grouped_expression(),
            Token::LBracket => self.parse_array_literal(),
            Token::If => self.parse_if_expression(),
            Token::Fn => self.parse_function_literal(),
            Token::While => self.parse_while_expression(),
            _ => None,
        }
    }

    fn parse_prefix_expression(&mut self) -> Option<Expression> {
        let prefix = match self.current_token {
            Token::Not => Prefix::Not,
            Token::Minus => Prefix::Minus,
            _ => return None,
        };
        self.next_token();
        let right = self.parse_expression(Precedence::Prefix)?;
        Some(Expression::Prefix(prefix, Box::new(right)))
    }

    fn parse_infix_expression(&mut self, left: Expression) -> Option<Expression> {
        match self.current_token {
            Token::LParen => return self.parse_call_expression(left),
            Token::LBracket => return self.parse_index_expression(left),
            _ => (),
        }

        let infix = match self.current_token {
            Token::Plus => Infix::Plus,
            Token::Minus => Infix::Minus,
            Token::Star => Infix::Star,
            Token::Slash => Infix::Slash,
            Token::Percent => Infix::Percent,
            Token::Equal => Infix::Equal,
            Token::NotEqual => Infix::NotEqual,
            Token::LessThan => Infix::LessThan,
            Token::LessThanOrEqual => Infix::LessThanOrEqual,
            Token::GreaterThan => Infix::GreaterThan,
            Token::GreaterThanOrEqual => Infix::GreaterThanOrEqual,
            _ => return None,
        };
        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;
        Some(Expression::Infix(infix, Box::new(left), Box::new(right)))
    }

    fn parse_index_expression(&mut self, left: Expression) -> Option<Expression> {
        self.next_token();
        let index = self.parse_expression(Precedence::Lowest)?;
        if !self.expect_peek(Token::RBracket) {
            return None;
        }
        Some(Expression::Index(Box::new(left), Box::new(index)))
    }

    fn parse_array_literal(&mut self) -> Option<Expression> {
        let elements = self.parse_expression_list(Token::RBracket)?;
        Some(Expression::Array(elements))
    }

    fn parse_expression_list(&mut self, end: Token) -> Option<Vec<Expression>> {
        let mut list = vec![];
        if self.peek_token == end {
            self.next_token();
            return Some(list);
        }
        self.next_token();
        list.push(self.parse_expression(Precedence::Lowest)?);
        while self.peek_token == Token::Comma {
            self.next_token();
            self.next_token();
            list.push(self.parse_expression(Precedence::Lowest)?);
        }
        if !self.expect_peek(end) {
            return None;
        }
        Some(list)
    }

    fn parse_grouped_expression(&mut self) -> Option<Expression> {
        self.next_token();
        let exp = self.parse_expression(Precedence::Lowest);
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        exp
    }

    fn parse_call_expression(&mut self, function: Expression) -> Option<Expression> {
        let arguments = self.parse_call_arguments()?;
        Some(Expression::Call {
            function: Box::new(function),
            arguments,
        })
    }

    fn parse_call_arguments(&mut self) -> Option<Vec<Expression>> {
        let mut args = vec![];
        if self.peek_token == Token::RParen {
            self.next_token();
            return Some(args);
        }
        self.next_token();
        args.push(self.parse_expression(Precedence::Lowest)?);
        while self.peek_token == Token::Comma {
            self.next_token();
            self.next_token();
            args.push(self.parse_expression(Precedence::Lowest)?);
        }
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        Some(args)
    }

    fn parse_function_literal(&mut self) -> Option<Expression> {
        if !self.expect_peek(Token::LParen) {
            return None;
        }
        let parameters = self.parse_function_parameters()?;
        if !self.expect_peek(Token::LBrace) {
            return None;
        }
        let body = self.parse_block_statement();
        Some(Expression::Function { parameters, body })
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<String>> {
        let mut identifiers = vec![];
        if self.peek_token == Token::RParen {
            self.next_token();
            return Some(identifiers);
        }
        self.next_token();
        if let Token::Identifier(name) = &self.current_token {
            identifiers.push(name.clone());
        } else {
            return None;
        }
        while self.peek_token == Token::Comma {
            self.next_token();
            self.next_token();
            if let Token::Identifier(name) = &self.current_token {
                identifiers.push(name.clone());
            } else {
                return None;
            }
        }
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        Some(identifiers)
    }

    fn parse_if_expression(&mut self) -> Option<Expression> {
        if !self.expect_peek(Token::LParen) {
            return None;
        }
        self.next_token();
        let condition = self.parse_expression(Precedence::Lowest)?;
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        if !self.expect_peek(Token::LBrace) {
            return None;
        }
        let consequence = self.parse_block_statement();
        let mut alternative = None;
        if self.peek_token == Token::Else {
            self.next_token();
            if !self.expect_peek(Token::LBrace) {
                return None;
            }
            alternative = Some(self.parse_block_statement());
        }
        Some(Expression::If {
            condition: Box::new(condition),
            consequence,
            alternative,
        })
    }

    fn parse_while_expression(&mut self) -> Option<Expression> {
        if !self.expect_peek(Token::LParen) {
            return None;
        }
        self.next_token();
        let condition = self.parse_expression(Precedence::Lowest)?;
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        if !self.expect_peek(Token::LBrace) {
            return None;
        }
        let body = self.parse_block_statement();
        Some(Expression::While {
            condition: Box::new(condition),
            body,
        })
    }

    fn parse_block_statement(&mut self) -> BlockStatement {
        let mut statements = vec![];
        self.next_token();
        while self.current_token != Token::RBrace && self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                statements.push(statement);
            }
            self.next_token();
        }
        statements
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if self.peek_token_is(&token) {
            self.next_token();
            true
        } else {
            self.peek_error(&token);
            false
        }
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

    fn current_precedence(&self) -> Precedence {
        Self::token_to_precedence(&self.current_token)
    }

    fn peek_precedence(&self) -> Precedence {
        Self::token_to_precedence(&self.peek_token)
    }

    fn token_to_precedence(token: &Token) -> Precedence {
        match token {
            Token::Equal | Token::NotEqual => Precedence::Equals,
            Token::LessThan | Token::LessThanOrEqual | Token::GreaterThan | Token::GreaterThanOrEqual => {
                Precedence::LessGreater
            }
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Star | Token::Slash | Token::Percent => Precedence::Product,
            Token::LParen => Precedence::Call,
            Token::LBracket => Precedence::Index,
            _ => Precedence::Lowest,
        }
    }
}
