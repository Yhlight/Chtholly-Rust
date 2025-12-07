use crate::compiler::lexer::Token;

#[derive(Debug, PartialEq, Clone)]
pub enum Statement {
    Let(String, Expression),
    Return(Expression),
    Expression(Expression),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expression {
    Identifier(String),
    IntegerLiteral(i64),
    FloatLiteral(f64),
    StringLiteral(String),
    BooleanLiteral(bool),
    Prefix(Token, Box<Expression>),
    Infix(Box<Expression>, Token, Box<Expression>),
    If {
        condition: Box<Expression>,
        consequence: Vec<Statement>,
        alternative: Option<Vec<Statement>>,
    },
    FunctionLiteral {
        parameters: Vec<String>,
        body: Vec<Statement>,
    },
    Call {
        function: Box<Expression>,
        arguments: Vec<Expression>,
    },
}

pub struct Parser<'a> {
    lexer: &'a mut crate::compiler::lexer::Lexer<'a>,
    current_token: Token,
    peek_token: Token,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: &'a mut crate::compiler::lexer::Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
        };
        p.next_token();
        p.next_token();
        p
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Vec<Statement> {
        let mut statements = Vec::new();

        while self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                statements.push(statement);
            }
            self.next_token();
        }

        statements
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }

        let name = match self.current_token.clone() {
            Token::Identifier(s) => s,
            _ => return None,
        };

        if !self.expect_peek(Token::Equal) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Let(name, value))
    }

    fn parse_return_statement(&mut self) -> Option<Statement> {
        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Return(value))
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Expression(expression))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left = match self.current_token.clone() {
            Token::Identifier(s) => Expression::Identifier(s),
            Token::Integer(i) => Expression::IntegerLiteral(i),
            _ => return None,
        };

        while !self.peek_token_is(Token::Semicolon) && precedence < self.peek_precedence() {
            self.next_token();
            left = self.parse_infix_expression(left)?;
        }

        Some(left)
    }

    fn parse_infix_expression(&mut self, left: Expression) -> Option<Expression> {
        let token = self.current_token.clone();
        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;
        Some(Expression::Infix(Box::new(left), token, Box::new(right)))
    }

    fn peek_token_is(&self, t: Token) -> bool {
        match t {
            Token::Identifier(_) => {
                matches!(self.peek_token, Token::Identifier(_))
            }
            _ => self.peek_token == t,
        }
    }

    fn expect_peek(&mut self, t: Token) -> bool {
        if self.peek_token_is(t) {
            self.next_token();
            true
        } else {
            false
        }
    }

    fn current_precedence(&self) -> Precedence {
        token_to_precedence(&self.current_token)
    }

    fn peek_precedence(&self) -> Precedence {
        token_to_precedence(&self.peek_token)
    }
}

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Equals,
    LessGreater,
    Sum,
    Product,
    Prefix,
    Call,
}

fn token_to_precedence(token: &Token) -> Precedence {
    match token {
        Token::EqualEqual => Precedence::Equals,
        Token::NotEqual => Precedence::Equals,
        Token::LessThan => Precedence::LessGreater,
        Token::GreaterThan => Precedence::LessGreater,
        Token::Plus => Precedence::Sum,
        Token::Minus => Precedence::Sum,
        Token::Slash => Precedence::Product,
        Token::Asterisk => Precedence::Product,
        Token::LParen => Precedence::Call,
        _ => Precedence::Lowest,
    }
}
