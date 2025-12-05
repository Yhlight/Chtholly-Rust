use crate::lexer::lexer::Lexer;
use crate::lexer::token::Token;
use crate::ast::ast::{Program, Statement, Identifier};

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
            _ => None,
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

        // TODO: We're skipping the expression until we have a parser for it.
        while !self.current_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::LetStatement {
            token: let_token,
            name,
        })
    }

    fn current_token_is(&self, token_type: &Token) -> bool {
        std::mem::discriminant(&self.current_token) == std::mem::discriminant(token_type)
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
