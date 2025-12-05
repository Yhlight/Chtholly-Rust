use crate::ast::{BlockStatement, Expression, Literal, Operator, Program, Statement};
use crate::lexer::{Lexer, Token};

#[derive(PartialEq, PartialOrd)]
enum Precedence {
    Lowest,
    Equals,
    LessGreater,
    Sum,
    Product,
    Call,
}

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
            Token::Fn => self.parse_fn_statement(),
            _ => {
                if !matches!(self.peek_token, Token::Let | Token::Fn | Token::Mut) {
                    self.parse_expression_statement()
                } else {
                    None
                }
            }
        }
    }

    fn parse_let_statement(&mut self) -> Option<Statement> {
        let mut is_mutable = false;
        if matches!(self.peek_token, Token::Mut) {
            is_mutable = true;
            self.next_token();
        }

        if !matches!(self.peek_token, Token::Ident(_)) {
            self.skip_to_semicolon();
            return None;
        }
        self.next_token();

        let name = match self.current_token.clone() {
            Token::Ident(name) => name,
            _ => return None, // Unreachable
        };

        if !matches!(self.peek_token, Token::Assign) {
            self.skip_to_semicolon();
            return None;
        }
        self.next_token(); // consume '='
        self.next_token(); // consume token to start expression

        let expression = self.parse_expression(Precedence::Lowest);
        if expression.is_none() {
            self.skip_to_semicolon();
            return None;
        }

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::Let(name, is_mutable, expression.unwrap()))
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expression = self.parse_expression(Precedence::Lowest)?;

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Statement::ExpressionStatement(expression))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left = self.parse_prefix()?;

        while !matches!(self.peek_token, Token::Semicolon) && precedence < self.peek_precedence() {
            self.next_token();
            left = self.parse_infix(left)?;
        }

        Some(left)
    }

    fn parse_prefix(&mut self) -> Option<Expression> {
        match &self.current_token {
            Token::Ident(name) => Some(Expression::Identifier(name.clone())),
            Token::Int(value) => Some(Expression::Literal(Literal::Int(*value))),
            Token::Double(value) => Some(Expression::Literal(Literal::Double(*value))),
            Token::String(value) => Some(Expression::Literal(Literal::String(value.clone()))),
            Token::Char(value) => Some(Expression::Literal(Literal::Char(*value))),
            Token::True => Some(Expression::Literal(Literal::Bool(true))),
            Token::False => Some(Expression::Literal(Literal::Bool(false))),
            Token::If => self.parse_if_expression(),
            Token::While => self.parse_while_expression(),
            _ => None,
        }
    }

    fn parse_binary_expression(&mut self, left: Expression) -> Option<Expression> {
        let operator = match self.current_token {
            Token::Plus => Operator::Plus,
            Token::Minus => Operator::Minus,
            Token::Star => Operator::Star,
            Token::Slash => Operator::Slash,
            Token::Lt => Operator::Lt,
            _ => return None,
        };

        let precedence = self.current_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Expression::Binary(operator, Box::new(left), Box::new(right)))
    }

    fn parse_call_expression(&mut self, function: Expression) -> Option<Expression> {
        let arguments = self.parse_call_arguments()?;
        Some(Expression::Call(Box::new(function), arguments))
    }

    fn parse_call_arguments(&mut self) -> Option<Vec<Expression>> {
        let mut arguments = Vec::new();

        if matches!(self.peek_token, Token::RParen) {
            self.next_token();
            return Some(arguments);
        }

        self.next_token();

        if let Some(expression) = self.parse_expression(Precedence::Lowest) {
            arguments.push(expression);
        }

        while matches!(self.peek_token, Token::Comma) {
            self.next_token();
            self.next_token();
            if let Some(expression) = self.parse_expression(Precedence::Lowest) {
                arguments.push(expression);
            }
        }

        if !matches!(self.peek_token, Token::RParen) {
            return None;
        }
        self.next_token();

        Some(arguments)
    }

    fn parse_infix(&mut self, left: Expression) -> Option<Expression> {
        match self.current_token {
            Token::Plus | Token::Minus | Token::Star | Token::Slash | Token::Lt => {
                self.parse_binary_expression(left)
            }
            Token::LParen => self.parse_call_expression(left),
            _ => None,
        }
    }

    fn peek_precedence(&self) -> Precedence {
        match self.peek_token {
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Star | Token::Slash => Precedence::Product,
            Token::Lt => Precedence::LessGreater,
            Token::LParen => Precedence::Call,
            _ => Precedence::Lowest,
        }
    }

    fn current_precedence(&self) -> Precedence {
        match self.current_token {
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Star | Token::Slash => Precedence::Product,
            Token::Lt => Precedence::LessGreater,
            Token::LParen => Precedence::Call,
            _ => Precedence::Lowest,
        }
    }

    fn parse_fn_statement(&mut self) -> Option<Statement> {
        if !matches!(self.peek_token, Token::Ident(_)) {
            return None;
        }
        self.next_token();

        let name = match self.current_token.clone() {
            Token::Ident(name) => name,
            _ => return None,
        };

        if !matches!(self.peek_token, Token::LParen) {
            return None;
        }
        self.next_token();

        let parameters = self.parse_function_parameters()?;

        if !matches!(self.peek_token, Token::LBrace) {
            return None;
        }
        self.next_token();

        let body = self.parse_block_statement()?;

        Some(Statement::Function(name, parameters, body))
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<String>> {
        let mut identifiers = Vec::new();

        if matches!(self.peek_token, Token::RParen) {
            self.next_token();
            return Some(identifiers);
        }

        self.next_token();

        if let Token::Ident(name) = self.current_token.clone() {
            identifiers.push(name);
        }

        while matches!(self.peek_token, Token::Comma) {
            self.next_token();
            self.next_token();
            if let Token::Ident(name) = self.current_token.clone() {
                identifiers.push(name);
            }
        }

        if !matches!(self.peek_token, Token::RParen) {
            return None;
        }
        self.next_token();

        Some(identifiers)
    }

    fn parse_block_statement(&mut self) -> Option<BlockStatement> {
        let mut statements = Vec::new();

        while !matches!(self.current_token, Token::RBrace) && !matches!(self.current_token, Token::Eof) {
            if let Some(statement) = self.parse_statement() {
                statements.push(statement);
            }
            self.next_token();
        }

        Some(BlockStatement { statements })
    }

    fn parse_if_expression(&mut self) -> Option<Expression> {
        if !matches!(self.peek_token, Token::LParen) {
            return None;
        }
        self.next_token();
        self.next_token();

        let condition = self.parse_expression(Precedence::Lowest)?;

        if !matches!(self.peek_token, Token::RParen) {
            return None;
        }
        self.next_token();

        if !matches!(self.peek_token, Token::LBrace) {
            return None;
        }
        self.next_token();

        let consequence = self.parse_block_statement()?;

        let mut alternative = None;
        if matches!(self.peek_token, Token::Else) {
            self.next_token();

            if !matches!(self.peek_token, Token::LBrace) {
                return None;
            }
            self.next_token();

            alternative = self.parse_block_statement();
        }

        Some(Expression::If(
            Box::new(condition),
            consequence,
            alternative,
        ))
    }

    fn parse_while_expression(&mut self) -> Option<Expression> {
        if !matches!(self.peek_token, Token::LParen) {
            return None;
        }
        self.next_token();
        self.next_token();

        let condition = self.parse_expression(Precedence::Lowest)?;

        if !matches!(self.peek_token, Token::RParen) {
            return None;
        }
        self.next_token();

        if !matches!(self.peek_token, Token::LBrace) {
            return None;
        }
        self.next_token();

        let body = self.parse_block_statement()?;

        Some(Expression::While(Box::new(condition), body))
    }

    fn skip_to_semicolon(&mut self) {
        while !matches!(self.current_token, Token::Semicolon | Token::Eof) {
            self.next_token();
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
            let mut y = 10.0;
            let foobar = "bar";
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 3);

        let expected = vec![
            Statement::Let("x".to_string(), false, Expression::Literal(Literal::Int(5))),
            Statement::Let("y".to_string(), true, Expression::Literal(Literal::Double(10.0))),
            Statement::Let(
                "foobar".to_string(),
                false,
                Expression::Literal(Literal::String("bar".to_string())),
            ),
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

    #[test]
    fn test_function_statement() {
        let input = r#"
            fn main(x, y) {
                let z = x;
            }
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        let expected = Statement::Function(
            "main".to_string(),
            vec!["x".to_string(), "y".to_string()],
            BlockStatement {
                statements: vec![Statement::Let(
                    "z".to_string(),
                    false,
                    Expression::Identifier("x".to_string()),
                )],
            },
        );

        assert_eq!(program.statements[0], expected);
    }

    #[test]
    fn test_operator_precedence() {
        let input = "1 + 2 * 3";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        let expected = Statement::ExpressionStatement(Expression::Binary(
            Operator::Plus,
            Box::new(Expression::Literal(Literal::Int(1))),
            Box::new(Expression::Binary(
                Operator::Star,
                Box::new(Expression::Literal(Literal::Int(2))),
                Box::new(Expression::Literal(Literal::Int(3))),
            )),
        ));

        assert_eq!(program.statements[0], expected);
    }

    #[test]
    fn test_if_else_expression() {
        let input = "if (x < y) { x } else { y }";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        let expected = Statement::ExpressionStatement(Expression::If(
            Box::new(Expression::Binary(
                Operator::Lt,
                Box::new(Expression::Identifier("x".to_string())),
                Box::new(Expression::Identifier("y".to_string())),
            )),
            BlockStatement {
                statements: vec![Statement::ExpressionStatement(Expression::Identifier(
                    "x".to_string(),
                ))],
            },
            Some(BlockStatement {
                statements: vec![Statement::ExpressionStatement(Expression::Identifier(
                    "y".to_string(),
                ))],
            }),
        ));

        assert_eq!(program.statements[0], expected);
    }

    #[test]
    fn test_call_expression() {
        let input = "add(1, 2 * 3)";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        let expected = Statement::ExpressionStatement(Expression::Call(
            Box::new(Expression::Identifier("add".to_string())),
            vec![
                Expression::Literal(Literal::Int(1)),
                Expression::Binary(
                    Operator::Star,
                    Box::new(Expression::Literal(Literal::Int(2))),
                    Box::new(Expression::Literal(Literal::Int(3))),
                ),
            ],
        ));

        assert_eq!(program.statements[0], expected);
    }

    #[test]
    fn test_while_expression() {
        let input = "while (x < y) { x }";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        let expected = Statement::ExpressionStatement(Expression::While(
            Box::new(Expression::Binary(
                Operator::Lt,
                Box::new(Expression::Identifier("x".to_string())),
                Box::new(Expression::Identifier("y".to_string())),
            )),
            BlockStatement {
                statements: vec![Statement::ExpressionStatement(Expression::Identifier(
                    "x".to_string(),
                ))],
            },
        ));

        assert_eq!(program.statements[0], expected);
    }
}
