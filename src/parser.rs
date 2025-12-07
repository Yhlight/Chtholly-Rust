use crate::lexer::{Lexer, Token};

pub type Program = Vec<Statement>;

#[derive(Debug, PartialEq)]
pub enum Statement {
    Let(String, Expression),
}

#[derive(Debug, PartialEq)]
pub enum Expression {
    Identifier(String),
    Int(i64),
}

pub struct Parser {
    lexer: Lexer,
    current_token: Token,
    peek_token: Token,
}

impl Parser {
    pub fn new(lexer: Lexer) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
        };
        parser.next_token();
        parser.next_token();
        parser
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
            _ => None,
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }

        let name = match self.current_token.clone() {
            Token::Identifier(name) => name,
            _ => return None,
        };

        if !self.expect_peek(Token::Assign) {
            return None;
        }

        self.next_token(); // Skip the expression for now

        while self.current_token != Token::Semicolon {
            self.next_token();
        }

        // A dummy expression for now
        Some(Statement::Let(name, Expression::Int(0)))
    }

    fn expect_peek(&mut self, token: Token) -> bool {
        if std::mem::discriminant(&self.peek_token) == std::mem::discriminant(&token) {
            self.next_token();
            true
        } else {
            false
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statement() {
        let input = "let x = 5;".to_string();
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.len(), 1);
        assert_eq!(
            program[0],
            Statement::Let("x".to_string(), Expression::Int(0))
        );
    }
}
