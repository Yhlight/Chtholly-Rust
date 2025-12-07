use super::{ast::*, lexer::*};

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
        };
        // Prime the parser with two tokens.
        parser.next_token();
        parser.next_token();
        parser
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut nodes = Vec::new();

        while self.current_token != Token::Eof {
            if let Some(statement) = self.parse_statement() {
                nodes.push(Node::Statement(statement));
            }
            self.next_token();
        }

        Program { nodes }
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            _ => None,
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        // Expect identifier
        let name = match &self.peek_token {
            Token::Identifier(name) => name.clone(),
            _ => return None,
        };
        self.next_token();

        // Expect assign
        if !self.expect_peek(Token::Assign) {
            return None;
        }
        self.next_token();

        let initializer = self.parse_expression()?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Let {
            name,
            type_annotation: None,
            initializer: Box::new(initializer),
        })
    }

    fn parse_expression(&mut self) -> Option<Expression> {
        match self.current_token {
            Token::Integer(value) => Some(Expression::IntegerLiteral(value)),
            _ => None,
        }
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if self.peek_token == token {
            self.next_token();
            true
        } else {
            false
        }
    }
}
