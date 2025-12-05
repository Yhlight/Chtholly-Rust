use crate::ast::{Expression, Literal, Program, Statement};
use crate::lexer::{Lexer, Token};

pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut parser = Parser {
            lexer,
            current_token: Token::Illegal,
            peek_token: Token::Illegal,
        };
        parser.next_token();
        parser.next_token();
        parser
    }

    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
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
        if !matches!(self.peek_token, Token::Ident(_)) {
            return None;
        }
        self.next_token();

        let name = match self.current_token.clone() {
            Token::Ident(name) => name,
            _ => return None,
        };

        if !matches!(self.peek_token, Token::Assign) {
            return None;
        }
        self.next_token();
        self.next_token();

        let expression = self.parse_expression()?;

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Let(name, expression))
    }

    fn parse_expression(&mut self) -> Option<Expression> {
        match &self.current_token {
            Token::Ident(name) => Some(Expression::Identifier(name.clone())),
            Token::Int(value) => Some(Expression::Literal(Literal::Int(*value))),
            Token::Double(value) => Some(Expression::Literal(Literal::Double(*value))),
            Token::String(value) => Some(Expression::Literal(Literal::String(value.clone()))),
            _ => None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{Expression, Literal, Statement};
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = r#"
            let x = 5;
            let y = 10.0;
            let foobar = "bar";
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 3);

        let expected = vec![
            Statement::Let("x".to_string(), Expression::Literal(Literal::Int(5))),
            Statement::Let("y".to_string(), Expression::Literal(Literal::Double(10.0))),
            Statement::Let("foobar".to_string(), Expression::Literal(Literal::String("bar".to_string()))),
        ];

        for (i, statement) in program.statements.iter().enumerate() {
            assert_eq!(*statement, expected[i]);
        }
    }

    #[test]
    fn test_let_statement_errors() {
        let input = r#"
            let x = ;
            let = 10;
            let z;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 0);
    }
}
