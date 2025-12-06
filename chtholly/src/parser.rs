use crate::ast::{self, BlockStatement, Program, Type};
use crate::lexer::Lexer;
use crate::token::Token;

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
    Call,        // myFunction(X)
}

fn token_precedence(token: &Token) -> Precedence {
    match token {
        Token::Equal | Token::NotEqual => Precedence::Equals,
        Token::LessThan | Token::GreaterThan => Precedence::LessGreater,
        Token::Plus | Token::Minus => Precedence::Sum,
        Token::Slash | Token::Asterisk => Precedence::Product,
        Token::LParen => Precedence::Call,
        _ => Precedence::Lowest,
    }
}

pub struct Parser {
    lexer: Lexer,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl Parser {
    pub fn new(lexer: Lexer) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Illegal,
            peek_token: Token::Illegal,
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
        let mut program = Program::new();

        while self.current_token != Token::Eof {
            match self.parse_statement() {
                Some(statement) => program.statements.push(statement),
                None => {}
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<ast::Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            Token::Mut => self.parse_mut_statement(),
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_return_statement(&mut self) -> Option<ast::Statement> {
        self.next_token();

        let return_value = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        return_value.map(ast::Statement::Return)
    }

    fn parse_expression_statement(&mut self) -> Option<ast::Statement> {
        let expression = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        expression.map(ast::Statement::Expression)
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<ast::Expression> {
        let mut left_exp = match &self.current_token {
            Token::Identifier(_) => self.parse_identifier(),
            Token::Int(_) => self.parse_integer_literal(),
            Token::Float(_) => self.parse_float_literal(),
            Token::String(_) => self.parse_string_literal(),
            Token::Char(_) => self.parse_char_literal(),
            Token::True | Token::False => self.parse_boolean(),
            Token::If => self.parse_if_expression(),
            Token::Fn => self.parse_function_literal(),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            _ => None,
        };

        while !self.peek_token_is(&Token::Semicolon) && precedence < self.peek_precedence() {
            match self.peek_token {
                Token::Plus | Token::Minus | Token::Slash | Token::Asterisk | Token::Equal | Token::NotEqual | Token::LessThan | Token::GreaterThan => {
                    self.next_token();
                    left_exp = self.parse_infix_expression(left_exp.unwrap());
                }
                Token::LParen => {
                    self.next_token();
                    left_exp = self.parse_call_expression(left_exp.unwrap());
                }
                _ => return left_exp,
            }
        }

        left_exp
    }

    fn parse_identifier(&mut self) -> Option<ast::Expression> {
        if let Token::Identifier(name) = &self.current_token {
            Some(ast::Expression::Identifier(ast::Identifier(name.clone())))
        } else {
            None
        }
    }

    fn parse_integer_literal(&mut self) -> Option<ast::Expression> {
        if let Token::Int(value) = self.current_token {
            Some(ast::Expression::IntLiteral(value))
        } else {
            None
        }
    }

    fn parse_float_literal(&mut self) -> Option<ast::Expression> {
        if let Token::Float(value) = self.current_token {
            Some(ast::Expression::FloatLiteral(value))
        } else {
            None
        }
    }

    fn parse_string_literal(&mut self) -> Option<ast::Expression> {
        if let Token::String(value) = &self.current_token {
            Some(ast::Expression::StringLiteral(value.clone()))
        } else {
            None
        }
    }

    fn parse_char_literal(&mut self) -> Option<ast::Expression> {
        if let Token::Char(value) = self.current_token {
            Some(ast::Expression::CharLiteral(value))
        } else {
            None
        }
    }

    fn parse_boolean(&mut self) -> Option<ast::Expression> {
        match self.current_token {
            Token::True => Some(ast::Expression::Boolean(true)),
            Token::False => Some(ast::Expression::Boolean(false)),
            _ => None,
        }
    }

    fn parse_prefix_expression(&mut self) -> Option<ast::Expression> {
        let token = self.current_token.clone();
        self.next_token();
        let right = self.parse_expression(Precedence::Prefix);
        right.map(|r| ast::Expression::Prefix(token, Box::new(r)))
    }

    fn parse_infix_expression(&mut self, left: ast::Expression) -> Option<ast::Expression> {
        let token = self.current_token.clone();
        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence);
        right.map(|r| ast::Expression::Infix(Box::new(left), token, Box::new(r)))
    }

    fn parse_if_expression(&mut self) -> Option<ast::Expression> {
        if !self.expect_peek(&Token::LParen) {
            return None;
        }

        self.next_token();
        let condition = self.parse_expression(Precedence::Lowest);

        if !self.expect_peek(&Token::RParen) {
            return None;
        }

        if !self.expect_peek(&Token::LBrace) {
            return None;
        }

        let consequence = self.parse_block_statement();

        let mut alternative = None;
        if self.peek_token_is(&Token::Else) {
            self.next_token();

            if !self.expect_peek(&Token::LBrace) {
                return None;
            }

            alternative = Some(self.parse_block_statement());
        }

        condition.map(|c| ast::Expression::If {
            condition: Box::new(c),
            consequence,
            alternative,
        })
    }

    fn parse_block_statement(&mut self) -> BlockStatement {
        let mut statements = Vec::new();
        self.next_token();

        while !self.current_token_is(Token::RBrace) && !self.current_token_is(Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
            }
            self.next_token();
        }

        BlockStatement { statements }
    }

    fn parse_function_literal(&mut self) -> Option<ast::Expression> {
        if !self.expect_peek(&Token::LParen) {
            return None;
        }

        let parameters = self.parse_function_parameters()?;

        if !self.expect_peek(&Token::LBrace) {
            return None;
        }

        let body = self.parse_block_statement();

        Some(ast::Expression::FunctionLiteral { parameters, body })
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<(ast::Identifier, Type)>> {
        let mut identifiers = Vec::new();

        if self.peek_token_is(&Token::RParen) {
            self.next_token();
            return Some(identifiers);
        }

        self.next_token();

        let ident = if let Token::Identifier(name) = &self.current_token {
            ast::Identifier(name.clone())
        } else {
            return None;
        };

        if !self.expect_peek(&Token::Colon) {
            return None;
        }

        self.next_token();
        let type_ = self.parse_type()?;
        identifiers.push((ident, type_));

        while self.peek_token_is(&Token::Comma) {
            self.next_token();
            self.next_token();
            let ident = if let Token::Identifier(name) = &self.current_token {
                ast::Identifier(name.clone())
            } else {
                return None;
            };

            if !self.expect_peek(&Token::Colon) {
                return None;
            }

            self.next_token();
            let type_ = self.parse_type()?;
            identifiers.push((ident, type_));
        }

        if !self.expect_peek(&Token::RParen) {
            return None;
        }

        Some(identifiers)
    }

    fn parse_type(&mut self) -> Option<Type> {
        match self.current_token {
            Token::IntType => Some(Type::Int),
            Token::DoubleType => Some(Type::Double),
            Token::CharType => Some(Type::Char),
            Token::StringType => Some(Type::String),
            Token::BoolType => Some(Type::Bool),
            Token::VoidType => Some(Type::Void),
            _ => {
                let msg = format!("expected a type, got {:?}", self.current_token);
                self.errors.push(msg);
                None
            }
        }
    }

    fn parse_call_expression(&mut self, function: ast::Expression) -> Option<ast::Expression> {
        let arguments = self.parse_call_arguments();
        Some(ast::Expression::Call {
            function: Box::new(function),
            arguments,
        })
    }

    fn parse_call_arguments(&mut self) -> Vec<ast::Expression> {
        let mut args = Vec::new();

        if self.peek_token_is(&Token::RParen) {
            self.next_token();
            return args;
        }

        self.next_token();
        if let Some(exp) = self.parse_expression(Precedence::Lowest) {
            args.push(exp);
        }

        while self.peek_token_is(&Token::Comma) {
            self.next_token();
            self.next_token();
            if let Some(exp) = self.parse_expression(Precedence::Lowest) {
                args.push(exp);
            }
        }

        if !self.expect_peek(&Token::RParen) {
            return Vec::new();
        }

        args
    }

    fn parse_let_statement(&mut self) -> Option<ast::Statement> {
        if !self.expect_peek(&Token::Identifier("".to_string())) {
            return None;
        }

        let name = match &self.current_token {
            Token::Identifier(name) => ast::Identifier(name.clone()),
            _ => return None,
        };

        let mut type_ = None;
        if self.peek_token_is(&Token::Colon) {
            self.next_token();
            self.next_token();
            type_ = self.parse_type();
        }

        if !self.expect_peek(&Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        value.map(|v| ast::Statement::Let(name, type_, v))
    }

    fn parse_mut_statement(&mut self) -> Option<ast::Statement> {
        if !self.expect_peek(&Token::Identifier("".to_string())) {
            return None;
        }

        let name = match &self.current_token {
            Token::Identifier(name) => ast::Identifier(name.clone()),
            _ => return None,
        };

        let mut type_ = None;
        if self.peek_token_is(&Token::Colon) {
            self.next_token();
            self.next_token();
            type_ = self.parse_type();
        }

        if !self.expect_peek(&Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest);

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        value.map(|v| ast::Statement::Mut(name, type_, v))
    }

    fn expect_peek(&mut self, t: &Token) -> bool {
        if self.peek_token_is(t) {
            self.next_token();
            true
        } else {
            self.peek_error(t);
            false
        }
    }

    fn peek_error(&mut self, t: &Token) {
        let msg = format!(
            "expected next token to be {:?}, got {:?} instead",
            t, self.peek_token
        );
        self.errors.push(msg);
    }

    fn current_token_is(&self, t: Token) -> bool {
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(&t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn peek_precedence(&self) -> Precedence {
        token_precedence(&self.peek_token)
    }

    fn current_precedence(&self) -> Precedence {
        token_precedence(&self.current_token)
    }
}

#[cfg(test)]
mod tests;
