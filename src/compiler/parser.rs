use crate::compiler::ast::{Expression, LetStatement, MutStatement, Statement};
use crate::compiler::lexer::{Lexer, Token};

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

    pub fn parse_program(&mut self) -> Vec<Statement> {
        let mut statements = Vec::new();
        while self.current_token != Token::Eof {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
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

        let ident = match &self.current_token {
            Token::Identifier(s) => s.clone(),
            _ => return None,
        };

        let mut data_type = None;
        if self.peek_token == Token::Colon {
            self.next_token();
            self.next_token();
            data_type = match &self.current_token {
                Token::Identifier(s) => Some(s.clone()),
                _ => return None,
            };
        }

        if !self.expect_peek(Token::Assign) {
            return None;
        }
        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Let(LetStatement {
            ident,
            data_type,
            value,
        }))
    }

    fn parse_mut_statement(&mut self) -> Option<Statement> {
        if !self.expect_peek(Token::Identifier("".to_string())) {
            return None;
        }

        let ident = match &self.current_token {
            Token::Identifier(s) => s.clone(),
            _ => return None,
        };

        let mut data_type = None;
        if self.peek_token == Token::Colon {
            self.next_token();
            self.next_token();
            data_type = match &self.current_token {
                Token::Identifier(s) => Some(s.clone()),
                _ => return None,
            };
        }

        if !self.expect_peek(Token::Assign) {
            return None;
        }
        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token == Token::Semicolon {
            self.next_token();
        }

        Some(Statement::Mut(MutStatement {
            ident,
            data_type,
            value,
        }))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left_exp = match self.current_token {
            Token::Identifier(ref s) => Expression::Identifier(s.clone()),
            Token::IntLiteral(i) => Expression::IntLiteral(i),
            Token::FloatLiteral(f) => Expression::FloatLiteral(f),
            Token::StringLiteral(ref s) => Expression::StringLiteral(s.clone()),
            Token::Fn => self.parse_function_literal()?,
            Token::Minus | Token::Bang => {
                let token = self.current_token.clone();
                self.next_token();
                let right = self.parse_expression(Precedence::Prefix)?;
                Expression::Prefix(token, Box::new(right))
            }
            _ => return None,
        };

        while self.peek_token != Token::Semicolon && precedence < self.peek_precedence() {
            match self.peek_token {
                Token::Plus | Token::Minus | Token::Slash | Token::Asterisk | Token::Equal | Token::NotEqual | Token::LessThan | Token::GreaterThan => {
                    self.next_token();
                    let op = self.current_token.clone();
                    self.next_token();
                    let right = self.parse_expression(self.get_precedence(&op))?;
                    left_exp = Expression::Infix(op, Box::new(left_exp), Box::new(right));
                }
                Token::LParen => {
                    self.next_token();
                    left_exp = self.parse_call_expression(left_exp)?;
                }
                _ => return Some(left_exp),
            }
        }

        Some(left_exp)
    }

    fn peek_precedence(&self) -> Precedence {
        self.get_precedence(&self.peek_token)
    }

    fn current_precedence(&self) -> Precedence {
        self.get_precedence(&self.current_token)
    }

    fn get_precedence(&self, token: &Token) -> Precedence {
        match token {
            Token::Equal | Token::NotEqual => Precedence::Equals,
            Token::LessThan | Token::GreaterThan => Precedence::LessGreater,
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Slash | Token::Asterisk => Precedence::Product,
            Token::LParen => Precedence::Call,
            _ => Precedence::Lowest,
        }
    }

    fn parse_return_statement(&mut self) -> Option<Statement> {
        self.next_token();
        let value = self.parse_expression(Precedence::Lowest)?;
        if self.peek_token == Token::Semicolon {
            self.next_token();
        }
        Some(Statement::Return(crate::compiler::ast::ReturnStatement { value }))
    }

    fn parse_expression_statement(&mut self) -> Option<Statement> {
        let expr = self.parse_expression(Precedence::Lowest)?;
        if self.peek_token == Token::Semicolon {
            self.next_token();
        }
        Some(Statement::Expression(expr))
    }

    fn parse_function_literal(&mut self) -> Option<Expression> {
        if !self.expect_peek(Token::LParen) {
            return None;
        }
        let params = self.parse_function_parameters()?;
        if !self.expect_peek(Token::LBrace) {
            return None;
        }
        let body = self.parse_block_statement();
        Some(Expression::FunctionLiteral(crate::compiler::ast::FunctionLiteral { params, body }))
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<String>> {
        let mut identifiers = Vec::new();
        if self.peek_token == Token::RParen {
            self.next_token();
            return Some(identifiers);
        }
        self.next_token();
        let ident = match &self.current_token {
            Token::Identifier(s) => s.clone(),
            _ => return None,
        };
        identifiers.push(ident);
        while self.peek_token == Token::Comma {
            self.next_token();
            self.next_token();
            let ident = match &self.current_token {
                Token::Identifier(s) => s.clone(),
                _ => return None,
            };
            identifiers.push(ident);
        }
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        Some(identifiers)
    }

    fn parse_block_statement(&mut self) -> crate::compiler::ast::BlockStatement {
        let mut statements = Vec::new();
        self.next_token();
        while self.current_token != Token::RBrace && self.current_token != Token::Eof {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
            }
            self.next_token();
        }
        crate::compiler::ast::BlockStatement { statements }
    }

    fn parse_call_expression(&mut self, function: Expression) -> Option<Expression> {
        let arguments = self.parse_call_arguments()?;
        Some(Expression::Call(crate::compiler::ast::CallExpression {
            function: Box::new(function),
            arguments,
        }))
    }

    fn parse_call_arguments(&mut self) -> Option<Vec<Expression>> {
        let mut args = Vec::new();
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

    fn expect_peek(&mut self, token: Token) -> bool {
        if std::mem::discriminant(&self.peek_token) == std::mem::discriminant(&token) {
            self.next_token();
            true
        } else {
            false
        }
    }
}

#[derive(PartialEq, PartialOrd, Clone, Copy)]
enum Precedence {
    Lowest,
    Equals,      // ==
    LessGreater, // > or <
    Sum,         // +
    Product,     // *
    Prefix,      // -X or !X
    Call,        // myFunction(X)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler::ast::{
        BlockStatement, CallExpression, Expression, FunctionLiteral, LetStatement, MutStatement, Statement,
    };
    use crate::compiler::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = r#"
let x = 5;
let y = 10.5;
let foobar = "hello";
"#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.len(), 3);

        let expected = vec![
            Statement::Let(LetStatement {
                ident: "x".to_string(),
                data_type: None,
                value: Expression::IntLiteral(5),
            }),
            Statement::Let(LetStatement {
                ident: "y".to_string(),
                data_type: None,
                value: Expression::FloatLiteral(10.5),
            }),
            Statement::Let(LetStatement {
                ident: "foobar".to_string(),
                data_type: None,
                value: Expression::StringLiteral("hello".to_string()),
            }),
        ];

        for (i, stmt) in program.iter().enumerate() {
            assert_eq!(*stmt, expected[i]);
        }
    }

    #[test]
    fn test_mut_statements() {
        let input = r#"
mut x = 5;
mut y: f64 = 10.5;
"#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.len(), 2);

        let expected = vec![
            Statement::Mut(MutStatement {
                ident: "x".to_string(),
                data_type: None,
                value: Expression::IntLiteral(5),
            }),
            Statement::Mut(MutStatement {
                ident: "y".to_string(),
                data_type: Some("f64".to_string()),
                value: Expression::FloatLiteral(10.5),
            }),
        ];

        for (i, stmt) in program.iter().enumerate() {
            assert_eq!(*stmt, expected[i]);
        }
    }

    #[test]
    fn test_return_statements() {
        let input = r#"
return 5;
return 10.5;
return "hello";
"#;
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.len(), 3);

        let expected = vec![
            Statement::Return(crate::compiler::ast::ReturnStatement {
                value: Expression::IntLiteral(5),
            }),
            Statement::Return(crate::compiler::ast::ReturnStatement {
                value: Expression::FloatLiteral(10.5),
            }),
            Statement::Return(crate::compiler::ast::ReturnStatement {
                value: Expression::StringLiteral("hello".to_string()),
            }),
        ];

        for (i, stmt) in program.iter().enumerate() {
            assert_eq!(*stmt, expected[i]);
        }
    }

    #[test]
    fn test_function_literal_parsing() {
        let input = "fn(x, y) { x + y; }";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.len(), 1);
        let stmt = program[0].clone();
        let expected_body = BlockStatement {
            statements: vec![Statement::Expression(Expression::Infix(
                Token::Plus,
                Box::new(Expression::Identifier("x".to_string())),
                Box::new(Expression::Identifier("y".to_string())),
            ))],
        };
        let expected = Statement::Expression(Expression::FunctionLiteral(FunctionLiteral {
            params: vec!["x".to_string(), "y".to_string()],
            body: expected_body,
        }));
        assert_eq!(stmt, expected);
    }

    #[test]
    fn test_call_expression_parsing() {
        let input = "add(1, 2 * 3, 4 + 5);";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.len(), 1);
        let stmt = program[0].clone();
        let expected = Statement::Expression(Expression::Call(CallExpression {
            function: Box::new(Expression::Identifier("add".to_string())),
            arguments: vec![
                Expression::IntLiteral(1),
                Expression::Infix(
                    Token::Asterisk,
                    Box::new(Expression::IntLiteral(2)),
                    Box::new(Expression::IntLiteral(3)),
                ),
                Expression::Infix(
                    Token::Plus,
                    Box::new(Expression::IntLiteral(4)),
                    Box::new(Expression::IntLiteral(5)),
                ),
            ],
        }));
        assert_eq!(stmt, expected);
    }
}
