use crate::lexer::Lexer;
use crate::token::Token;
use crate::ast::{self, Program, Statement, Expression};

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

fn token_to_precedence(token: &Token) -> Precedence {
    match token {
        Token::Eq | Token::NotEq => Precedence::Equals,
        Token::Lt | Token::Gt => Precedence::LessGreater,
        Token::Plus | Token::Minus => Precedence::Sum,
        Token::Slash | Token::Asterisk => Precedence::Product,
        _ => Precedence::Lowest,
    }
}


pub struct Parser<'a> {
    lexer: Lexer<'a>,
    current_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            current_token: Token::Eof,
            peek_token: Token::Eof,
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    fn peek_error(&mut self, t: &Token) {
        let msg = format!("expected next token to be {:?}, got {:?} instead", t, self.peek_token);
        self.errors.push(msg);
    }

    fn no_prefix_parse_fn_error(&mut self, t: &Token) {
        let msg = format!("no prefix parse function for {:?} found", t);
        self.errors.push(msg);
    }


    fn next_token(&mut self) {
        self.current_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };

        while !matches!(self.current_token, Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                program.statements.push(stmt);
            }
            self.next_token();
        }

        program
    }

    fn parse_statement(&mut self) -> Option<Box<dyn Statement>> {
        match self.current_token {
            Token::Let => self.parse_let_statement(),
            Token::Return => self.parse_return_statement(),
            _ => self.parse_expression_statement(),
        }
    }

    fn parse_expression_statement(&mut self) -> Option<Box<dyn Statement>> {
        let token = self.current_token.clone();

        match self.parse_expression(Precedence::Lowest) {
            Some(expression) => {
                if matches!(self.peek_token, Token::Semicolon) {
                    self.next_token();
                }
                Some(Box::new(ast::ExpressionStatement { token, expression }))
            },
            None => None,
        }
    }


    fn parse_let_statement(&mut self) -> Option<Box<dyn Statement>> {
        let let_token = self.current_token.clone();

        if !matches!(self.peek_token, Token::Identifier(_)) {
            self.peek_error(&Token::Identifier("".to_string()));
            return None;
        }
        self.next_token();

        let name = match self.current_token.clone() {
            Token::Identifier(s) => ast::Identifier {
                token: self.current_token.clone(),
                value: s,
            },
            _ => unreachable!(),
        };

        if !matches!(self.peek_token, Token::Assign) {
            self.peek_error(&Token::Assign);
            return None;
        }
        self.next_token();
        self.next_token();

        let value = match self.parse_expression(Precedence::Lowest) {
            Some(expr) => expr,
            None => {
                return None;
            }
        };

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        let stmt = ast::LetStatement {
            token: let_token,
            name,
            value,
        };

        Some(Box::new(stmt))
    }

    fn parse_return_statement(&mut self) -> Option<Box<dyn Statement>> {
        let return_token = self.current_token.clone();
        self.next_token();

        let return_value = match self.parse_expression(Precedence::Lowest) {
            Some(expr) => expr,
            None => return None,
        };

        if matches!(self.peek_token, Token::Semicolon) {
            self.next_token();
        }

        Some(Box::new(ast::ReturnStatement {
            token: return_token,
            return_value,
        }))
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Box<dyn Expression>> {
        let token_clone = self.current_token.clone();
        let mut left_exp = match token_clone {
            Token::Identifier(_) => self.parse_identifier(),
            Token::Int(_) => self.parse_integer_literal(),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            Token::True | Token::False => self.parse_boolean(),
            _ => {
                self.no_prefix_parse_fn_error(&token_clone);
                return None;
            }
        };

        while !matches!(self.peek_token, Token::Semicolon) && precedence < self.peek_precedence() {
            match self.peek_token {
                Token::Plus | Token::Minus | Token::Slash | Token::Asterisk | Token::Eq | Token::NotEq | Token::Lt | Token::Gt => {
                    self.next_token();
                    left_exp = self.parse_infix_expression(left_exp.unwrap());
                }
                _ => return left_exp,
            }
        }

        left_exp
    }

    fn parse_infix_expression(&mut self, left: Box<dyn Expression>) -> Option<Box<dyn Expression>> {
        let token = self.current_token.clone();
        let operator = self.current_token.to_string();
        let precedence = self.cur_precedence();
        self.next_token();
        let right = self.parse_expression(precedence).unwrap();

        Some(Box::new(ast::InfixExpression {
            token,
            left,
            operator,
            right,
        }))
    }

    fn cur_precedence(&self) -> Precedence {
        token_to_precedence(&self.current_token)
    }

    fn peek_precedence(&self) -> Precedence {
        token_to_precedence(&self.peek_token)
    }


    fn parse_prefix_expression(&mut self) -> Option<Box<dyn Expression>> {
        let token = self.current_token.clone();
        let operator = self.current_token.to_string();

        self.next_token();

        let right = match self.parse_expression(Precedence::Prefix) {
            Some(expr) => expr,
            None => return None,
        };

        Some(Box::new(ast::PrefixExpression {
            token,
            operator,
            right,
        }))
    }

    fn parse_boolean(&mut self) -> Option<Box<dyn Expression>> {
        Some(Box::new(ast::BooleanLiteral {
            token: self.current_token.clone(),
            value: matches!(self.current_token, Token::True),
        }))
    }

    fn parse_identifier(&mut self) -> Option<Box<dyn Expression>> {
        if let Token::Identifier(s) = self.current_token.clone() {
            Some(Box::new(ast::Identifier {
                token: self.current_token.clone(),
                value: s,
            }))
        } else {
            None
        }
    }

    fn parse_integer_literal(&mut self) -> Option<Box<dyn Expression>> {
        if let Token::Int(val) = self.current_token {
            Some(Box::new(ast::IntegerLiteral {
                token: self.current_token.clone(),
                value: val,
            }))
        } else {
            None
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{self, LetStatement, Node, Statement, Expression, IntegerLiteral};
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let tests = vec![
            ("let x = 5;", "x", 5),
            ("let y = 10;", "y", 10),
            ("let foobar = 838383;", "foobar", 838383),
        ];

        for (input, expected_identifier, expected_value) in tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            check_parser_errors(&parser);

            assert_eq!(program.statements.len(), 1);
            let stmt = &program.statements[0];
            test_let_statement(stmt, expected_identifier);
            let let_stmt = stmt.as_any().downcast_ref::<LetStatement>().unwrap();
            test_integer_literal(&let_stmt.value, expected_value);
        }
    }

    fn test_let_statement(stmt: &Box<dyn Statement>, name: &str) {
        assert_eq!(stmt.token_literal(), "let", "stmt.token_literal not 'let'. got={}", stmt.token_literal());

        let let_stmt = stmt.as_any().downcast_ref::<LetStatement>().expect("statement not a LetStatement");

        assert_eq!(let_stmt.name.value, name, "let_stmt.name.value not '{}'. got={}", name, let_stmt.name.value);
        assert_eq!(let_stmt.name.token_literal(), name, "let_stmt.name.token_literal() not '{}'. got={}", name, let_stmt.name.token_literal());
    }

    fn test_integer_literal(expr: &Box<dyn Expression>, value: i64) {
        let integ = expr.as_any().downcast_ref::<IntegerLiteral>().expect("expr not IntegerLiteral");

        assert_eq!(integ.value, value);
        assert_eq!(integ.token_literal(), value.to_string());
    }


    fn check_parser_errors(parser: &Parser) {
        if parser.errors().is_empty() {
            return;
        }

        eprintln!("Parser has {} errors", parser.errors().len());
        for msg in parser.errors() {
            eprintln!("Parser error: {}", msg);
        }
        panic!("Parser has errors");
    }

    #[test]
    fn test_let_statement_error_handling() {
        let input = "let x = ;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_program();

        assert_eq!(parser.errors().len(), 1, "Expected 1 error, but got {} {:?}", parser.errors().len(), parser.errors());
    }

    #[test]
    fn test_return_statements() {
        let input = r#"
            return 5;
            return 10;
            return 993322;
        "#;

        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        check_parser_errors(&parser);

        assert_eq!(program.statements.len(), 3);

        for stmt in program.statements {
            assert_eq!(stmt.token_literal(), "return");
            let _return_stmt = stmt.as_any().downcast_ref::<ast::ReturnStatement>().expect("statement not ReturnStatement");
        }
    }

    #[test]
    fn test_identifier_expression() {
        let input = "foobar;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        check_parser_errors(&parser);

        assert_eq!(program.statements.len(), 1);
        let stmt = program.statements[0].as_any().downcast_ref::<ast::ExpressionStatement>().expect("statement not ExpressionStatement");
        let ident = stmt.expression.as_any().downcast_ref::<ast::Identifier>().expect("expression not Identifier");
        assert_eq!(ident.value, "foobar");
        assert_eq!(ident.token_literal(), "foobar");
    }

    #[test]
    fn test_parsing_prefix_expressions() {
        let prefix_tests = vec![
            ("!5;", "!", 5),
            ("-15;", "-", 15),
        ];

        for (input, operator, value) in prefix_tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            check_parser_errors(&parser);

            assert_eq!(program.statements.len(), 1);
            let stmt = program.statements[0].as_any().downcast_ref::<ast::ExpressionStatement>().expect("statement not ExpressionStatement");
            let exp = stmt.expression.as_any().downcast_ref::<ast::PrefixExpression>().expect("expression not PrefixExpression");
            assert_eq!(exp.operator, operator);
            test_integer_literal(&exp.right, value);
        }
    }

    #[test]
    fn test_boolean_expression() {
        let tests = vec![
            ("true;", true),
            ("false;", false),
        ];

        for (input, expected_value) in tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            check_parser_errors(&parser);

            assert_eq!(program.statements.len(), 1);
            let stmt = program.statements[0].as_any().downcast_ref::<ast::ExpressionStatement>().expect("statement not ExpressionStatement");
            let boolean = stmt.expression.as_any().downcast_ref::<ast::BooleanLiteral>().expect("expression not BooleanLiteral");
            assert_eq!(boolean.value, expected_value);
        }
    }

    #[test]
    fn test_prefix_expression_error_handling() {
        let input = "!;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_program();

        assert_eq!(parser.errors().len(), 1, "Expected 1 error, but got {} {:?}", parser.errors().len(), parser.errors());
    }

    #[test]
    fn test_return_statement_error_handling() {
        let input = "return ;";
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        parser.parse_program();

        assert_eq!(parser.errors().len(), 1, "Expected 1 error, but got {} {:?}", parser.errors().len(), parser.errors());
    }

    #[test]
    fn test_parsing_infix_expressions() {
        let infix_tests = vec![
            ("5 + 5;", 5, "+", 5),
            ("5 - 5;", 5, "-", 5),
            ("5 * 5;", 5, "*", 5),
            ("5 / 5;", 5, "/", 5),
        ];

        for (input, left_value, operator, right_value) in infix_tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            check_parser_errors(&parser);

            assert_eq!(program.statements.len(), 1);
            let stmt = program.statements[0].as_any().downcast_ref::<ast::ExpressionStatement>().expect("statement not ExpressionStatement");
            let exp = stmt.expression.as_any().downcast_ref::<ast::InfixExpression>().expect("expression not InfixExpression");

            test_integer_literal(&exp.left, left_value);
            assert_eq!(exp.operator, operator);
            test_integer_literal(&exp.right, right_value);
        }
    }

    #[test]
    fn test_operator_precedence_parsing() {
        let tests = vec![
            ("-a * b", "((-a) * b)"),
            ("!-a", "(!(-a))"),
            ("a + b + c", "((a + b) + c)"),
            ("a + b - c", "((a + b) - c)"),
            ("a * b * c", "((a * b) * c)"),
            ("a * b / c", "((a * b) / c)"),
            ("a + b / c", "(a + (b / c))"),
            ("a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"),
            ("5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"),
            ("5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"),
            ("3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"),
        ];

        for (input, expected) in tests {
            let lexer = Lexer::new(input);
            let mut parser = Parser::new(lexer);
            let program = parser.parse_program();
            check_parser_errors(&parser);

            let actual = program.to_string();
            assert_eq!(actual, expected);
        }
    }
}