//! The Parser for the Chtholly language.

use crate::ast::{self, Node, Program, Statement, LetStatement, Identifier, Expression, IntegerLiteral, FloatLiteral, StringLiteral, Boolean, ExpressionStatement, PrefixExpression, InfixExpression, ReturnStatement, BlockStatement, IfExpression, FunctionLiteral, CallExpression};
use crate::lexer::token::Token;
use crate::lexer::Lexer;

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

/// The Parser struct holds the state of the parser.
pub struct Parser<'a> {
    lexer: Lexer<'a>,
    cur_token: Token,
    peek_token: Token,
    errors: Vec<String>,
}

impl<'a> Parser<'a> {
    /// Creates a new Parser.
    pub fn new(lexer: Lexer<'a>) -> Self {
        let mut p = Parser {
            lexer,
            cur_token: Token::Illegal,
            peek_token: Token::Illegal,
            errors: Vec::new(),
        };
        p.next_token();
        p.next_token();
        p
    }

    /// Advances the tokens in the stream.
    fn next_token(&mut self) {
        self.cur_token = self.peek_token.clone();
        self.peek_token = self.lexer.next_token();
    }

    /// Returns the list of parsing errors.
    pub fn errors(&self) -> &Vec<String> {
        &self.errors
    }

    pub fn parse_program(&mut self) -> Program {
        let mut program = Program {
            statements: Vec::new(),
        };
        while self.cur_token != Token::Eof {
            if let Some(stmt) = self.parse_statement() {
                program.statements.push(stmt);
            }
            self.next_token();
        }
        program
    }

    fn parse_statement(&mut self) -> Option<Statement> {
        match self.cur_token {
            Token::Let => self.parse_let_statement().map(Statement::Let),
            Token::Return => self.parse_return_statement().map(Statement::Return),
            _ => self.parse_expression_statement().map(Statement::Expression),
        }
    }

    fn parse_let_statement(&mut self) -> Option<LetStatement> {
        let token = self.cur_token.clone();

        if !self.expect_peek(Token::Ident("".to_string())) {
            return None;
        }

        let name = Identifier {
            token: self.cur_token.clone(),
            value: match &self.cur_token {
                Token::Ident(s) => s.clone(),
                _ => return None,
            },
        };

        if !self.expect_peek(Token::Assign) {
            return None;
        }

        self.next_token();

        let value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(LetStatement { token, name, value })
    }

    fn parse_return_statement(&mut self) -> Option<ReturnStatement> {
        let token = self.cur_token.clone();
        self.next_token();

        let return_value = self.parse_expression(Precedence::Lowest)?;

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(ReturnStatement { token, return_value })
    }

    fn parse_expression_statement(&mut self) -> Option<ExpressionStatement> {
        let stmt = ExpressionStatement {
            token: self.cur_token.clone(),
            expression: self.parse_expression(Precedence::Lowest)?,
        };

        if self.peek_token_is(&Token::Semicolon) {
            self.next_token();
        }

        Some(stmt)
    }

    fn parse_expression(&mut self, precedence: Precedence) -> Option<Expression> {
        let mut left_exp = match &self.cur_token {
            Token::Ident(_) => self.parse_identifier(),
            Token::Int(_) => self.parse_integer_literal(),
            Token::True | Token::False => self.parse_boolean(),
            Token::LParen => self.parse_grouped_expression(),
            Token::Bang | Token::Minus => self.parse_prefix_expression(),
            Token::If => self.parse_if_expression(),
            Token::Function => self.parse_function_literal(),
            _ => None,
        }?;

        while !self.peek_token_is(&Token::Semicolon) && precedence < self.peek_precedence() {
            match self.peek_token {
                Token::Plus | Token::Minus | Token::Slash | Token::Asterisk | Token::Eq | Token::NotEq | Token::Lt | Token::Gt => {
                    self.next_token();
                    left_exp = self.parse_infix_expression(left_exp)?;
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

    fn parse_identifier(&mut self) -> Option<Expression> {
        match &self.cur_token {
            Token::Ident(value) => Some(Expression::Identifier(Identifier {
                token: self.cur_token.clone(),
                value: value.clone(),
            })),
            _ => None,
        }
    }

    fn parse_integer_literal(&mut self) -> Option<Expression> {
        match self.cur_token {
            Token::Int(value) => Some(Expression::IntegerLiteral(IntegerLiteral {
                token: self.cur_token.clone(),
                value,
            })),
            _ => None,
        }
    }

    fn parse_boolean(&mut self) -> Option<Expression> {
        Some(Expression::Boolean(Boolean {
            token: self.cur_token.clone(),
            value: self.cur_token_is(&Token::True),
        }))
    }

    fn parse_grouped_expression(&mut self) -> Option<Expression> {
        self.next_token();
        let exp = self.parse_expression(Precedence::Lowest);
        if !self.expect_peek(Token::RParen) {
            return None;
        }
        exp
    }

    fn parse_prefix_expression(&mut self) -> Option<Expression> {
        let token = self.cur_token.clone();
        let operator = self.cur_token.to_string();

        self.next_token();

        let right = self.parse_expression(Precedence::Prefix)?;

        Some(Expression::Prefix(PrefixExpression {
            token,
            operator,
            right: Box::new(right),
        }))
    }

    fn parse_infix_expression(&mut self, left: Expression) -> Option<Expression> {
        let token = self.cur_token.clone();
        let operator = self.cur_token.to_string();
        let precedence = self.cur_precedence();
        self.next_token();
        let right = self.parse_expression(precedence)?;

        Some(Expression::Infix(InfixExpression {
            token,
            left: Box::new(left),
            operator,
            right: Box::new(right),
        }))
    }

    fn parse_if_expression(&mut self) -> Option<Expression> {
        let token = self.cur_token.clone();

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

        let consequence = self.parse_block_statement()?;

        let mut alternative = None;
        if self.peek_token_is(&Token::Else) {
            self.next_token();

            if !self.expect_peek(Token::LBrace) {
                return None;
            }

            alternative = self.parse_block_statement();
        }

        Some(Expression::If(IfExpression {
            token,
            condition: Box::new(condition),
            consequence,
            alternative,
        }))
    }

    fn parse_block_statement(&mut self) -> Option<BlockStatement> {
        let token = self.cur_token.clone();
        let mut statements = Vec::new();

        self.next_token();

        while !self.cur_token_is(&Token::RBrace) && !self.cur_token_is(&Token::Eof) {
            if let Some(stmt) = self.parse_statement() {
                statements.push(stmt);
            }
            self.next_token();
        }

        Some(BlockStatement { token, statements })
    }

    fn parse_function_literal(&mut self) -> Option<Expression> {
        let token = self.cur_token.clone();

        if !self.expect_peek(Token::LParen) {
            return None;
        }

        let parameters = self.parse_function_parameters()?;

        if !self.expect_peek(Token::LBrace) {
            return None;
        }

        let body = self.parse_block_statement()?;

        Some(Expression::FunctionLiteral(FunctionLiteral {
            token,
            parameters,
            body,
        }))
    }

    fn parse_function_parameters(&mut self) -> Option<Vec<Identifier>> {
        let mut identifiers = Vec::new();

        if self.peek_token_is(&Token::RParen) {
            self.next_token();
            return Some(identifiers);
        }

        self.next_token();

        let ident = Identifier {
            token: self.cur_token.clone(),
            value: self.cur_token.to_string(),
        };
        identifiers.push(ident);

        while self.peek_token_is(&Token::Comma) {
            self.next_token();
            self.next_token();
            let ident = Identifier {
                token: self.cur_token.clone(),
                value: self.cur_token.to_string(),
            };
            identifiers.push(ident);
        }

        if !self.expect_peek(Token::RParen) {
            return None;
        }

        Some(identifiers)
    }

    fn parse_call_expression(&mut self, function: Expression) -> Option<Expression> {
        let token = self.cur_token.clone();
        let arguments = self.parse_call_arguments()?;
        Some(Expression::Call(CallExpression {
            token,
            function: Box::new(function),
            arguments,
        }))
    }

    fn parse_call_arguments(&mut self) -> Option<Vec<Expression>> {
        let mut args = Vec::new();

        if self.peek_token_is(&Token::RParen) {
            self.next_token();
            return Some(args);
        }

        self.next_token();
        args.push(self.parse_expression(Precedence::Lowest)?);

        while self.peek_token_is(&Token::Comma) {
            self.next_token();
            self.next_token();
            args.push(self.parse_expression(Precedence::Lowest)?);
        }

        if !self.expect_peek(Token::RParen) {
            return None;
        }

        Some(args)
    }

    fn cur_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.cur_token) == std::mem::discriminant(t)
    }

    fn peek_token_is(&self, t: &Token) -> bool {
        std::mem::discriminant(&self.peek_token) == std::mem::discriminant(t)
    }

    fn expect_peek(&mut self, t: Token) -> bool {
        if self.peek_token_is(&t) {
            self.next_token();
            true
        } else {
            self.peek_error(&t);
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

    fn get_precedence(token: &Token) -> Precedence {
        match token {
            Token::Eq | Token::NotEq => Precedence::Equals,
            Token::Lt | Token::Gt => Precedence::LessGreater,
            Token::Plus | Token::Minus => Precedence::Sum,
            Token::Slash | Token::Asterisk => Precedence::Product,
            Token::LParen => Precedence::Call,
            _ => Precedence::Lowest,
        }
    }

    fn peek_precedence(&self) -> Precedence {
        Self::get_precedence(&self.peek_token)
    }

    fn cur_precedence(&self) -> Precedence {
        Self::get_precedence(&self.cur_token)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::Node;
    use crate::lexer::Lexer;

    #[test]
    fn test_let_statements() {
        let input = "
let x = 5;
let y = 10;
let foobar = 838383;
";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(
            program.0.statements.len(),
            3,
            "program.statements does not contain 3 statements. got={}",
            program.0.statements.len()
        );

        let tests = vec![
            ("x", 5),
            ("y", 10),
            ("foobar", 838383),
        ];

        for (i, (expected_ident, expected_val)) in tests.iter().enumerate() {
            let stmt = &program.0.statements[i];
            assert_let_statement(stmt, expected_ident);

            if let Statement::Let(let_stmt) = stmt {
                assert_literal_expression(&let_stmt.value, expected_val);
            }
        }
    }

    #[test]
    fn test_return_statements() {
        let input = "
return 5;
return 10;
return 993322;
";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(program.0.statements.len(), 3);

        for stmt in program.0.statements {
            if let Statement::Return(return_stmt) = stmt {
                assert_eq!(return_stmt.token_literal(), "return");
            } else {
                panic!("statement not ReturnStatement. got={:?}", stmt);
            }
        }
    }

    #[test]
    fn test_expression_statement() {
        let input = "foobar;";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(program.0.statements.len(), 1);

        if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
            if let Expression::Identifier(ident) = &exp_stmt.expression {
                assert_eq!(ident.value, "foobar");
                assert_eq!(ident.token_literal(), "foobar");
            } else {
                panic!("expression not Identifier. got={:?}", exp_stmt.expression);
            }
        } else {
            panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
        }
    }

    #[test]
    fn test_boolean_expression() {
        let tests = vec![
            ("true;", true),
            ("false;", false),
        ];

        for (input, expected) in tests {
            let program = parse_program(input);
            check_parser_errors(&program.1);

            assert_eq!(program.0.statements.len(), 1);
            if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
                if let Expression::Boolean(boolean) = &exp_stmt.expression {
                    assert_eq!(boolean.value, expected);
                } else {
                    panic!("expression not Boolean. got={:?}", exp_stmt.expression);
                }
            } else {
                panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
            }
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
            ("3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"),
            ("5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"),
            ("5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"),
            ("3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"),
            ("1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"),
            ("(5 + 5) * 2", "((5 + 5) * 2)"),
            ("2 / (5 + 5)", "(2 / (5 + 5))"),
            ("-(5 + 5)", "(-(5 + 5))"),
            ("!(true == true)", "(!(true == true))"),
        ];

        for (input, expected) in tests {
            let program = parse_program(input);
            check_parser_errors(&program.1);
            assert_eq!(program.0.string(), expected);
        }
    }

    #[test]
    fn test_prefix_expressions() {
        let prefix_tests = vec![
            ("!5;", "!", 5),
            ("-15;", "-", 15),
        ];

        for (input, operator, value) in prefix_tests {
            let program = parse_program(input);
            check_parser_errors(&program.1);

            assert_eq!(program.0.statements.len(), 1);
            if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
                if let Expression::Prefix(prefix_exp) = &exp_stmt.expression {
                    assert_eq!(prefix_exp.operator, operator);
                    assert_literal_expression(&prefix_exp.right, value);
                } else {
                    panic!("expression not PrefixExpression. got={:?}", exp_stmt.expression);
                }
            } else {
                panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
            }
        }
    }

    #[test]
    fn test_infix_expressions() {
        let infix_tests = vec![
            ("5 + 5;", 5, "+", 5),
            ("5 - 5;", 5, "-", 5),
            ("5 * 5;", 5, "*", 5),
            ("5 / 5;", 5, "/", 5),
            ("5 > 5;", 5, ">", 5),
            ("5 < 5;", 5, "<", 5),
            ("5 == 5;", 5, "==", 5),
            ("5 != 5;", 5, "!=", 5),
        ];

        for (input, left_val, operator, right_val) in infix_tests {
            let program = parse_program(input);
            check_parser_errors(&program.1);

            assert_eq!(program.0.statements.len(), 1);
            if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
                if let Expression::Infix(infix_exp) = &exp_stmt.expression {
                    assert_literal_expression(&infix_exp.left, left_val);
                    assert_eq!(infix_exp.operator, operator);
                    assert_literal_expression(&infix_exp.right, right_val);
                } else {
                    panic!("expression not InfixExpression. got={:?}", exp_stmt.expression);
                }
            } else {
                panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
            }
        }
    }

    #[test]
    fn test_if_expression() {
        let input = "if (x < y) { x }";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(program.0.statements.len(), 1);
        if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
            if let Expression::If(if_exp) = &exp_stmt.expression {
                assert_eq!(if_exp.consequence.statements.len(), 1);
                assert!(if_exp.alternative.is_none());
            } else {
                panic!("expression not IfExpression. got={:?}", exp_stmt.expression);
            }
        } else {
            panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
        }
    }

    #[test]
    fn test_if_else_expression() {
        let input = "if (x < y) { x } else { y }";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(program.0.statements.len(), 1);
        if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
            if let Expression::If(if_exp) = &exp_stmt.expression {
                assert_eq!(if_exp.consequence.statements.len(), 1);
                assert!(if_exp.alternative.is_some());
            } else {
                panic!("expression not IfExpression. got={:?}", exp_stmt.expression);
            }
        } else {
            panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
        }
    }

    #[test]
    fn test_function_literal_parsing() {
        let input = "fn(x, y) { x + y; }";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(program.0.statements.len(), 1);
        if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
            if let Expression::FunctionLiteral(func) = &exp_stmt.expression {
                assert_eq!(func.parameters.len(), 2);
                assert_eq!(func.parameters[0].value, "x");
                assert_eq!(func.parameters[1].value, "y");
                assert_eq!(func.body.statements.len(), 1);
            } else {
                panic!("expression not FunctionLiteral. got={:?}", exp_stmt.expression);
            }
        } else {
            panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
        }
    }

    #[test]
    fn test_call_expression_parsing() {
        let input = "add(1, 2 * 3, 4 + 5);";
        let program = parse_program(input);
        check_parser_errors(&program.1);

        assert_eq!(program.0.statements.len(), 1);
        if let Statement::Expression(exp_stmt) = &program.0.statements[0] {
            if let Expression::Call(call_exp) = &exp_stmt.expression {
                assert_eq!(call_exp.arguments.len(), 3);
            } else {
                panic!("expression not CallExpression. got={:?}", exp_stmt.expression);
            }
        } else {
            panic!("statement not ExpressionStatement. got={:?}", program.0.statements[0]);
        }
    }

    fn parse_program(input: &str) -> (Program, Parser) {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        (program, parser)
    }

    fn assert_let_statement(s: &Statement, name: &str) {
        assert_eq!(s.token_literal(), "let");

        if let Statement::Let(let_stmt) = s {
            assert_eq!(let_stmt.name.value, name);
            assert_eq!(let_stmt.name.token_literal(), name);
        } else {
            panic!("s not Statement::Let. got={:?}", s);
        }
    }

    fn assert_literal_expression<T>(exp: &Expression, expected: T)
    where
        T: std::fmt::Display + PartialEq,
    {
        match exp {
            Expression::IntegerLiteral(il) => {
                assert_eq!(il.value.to_string(), expected.to_string());
            }
            // Add other literal types here
            _ => panic!("Expression not a literal. got={:?}", exp),
        }
    }

    fn check_parser_errors(parser: &Parser) {
        let errors = parser.errors();
        if errors.is_empty() {
            return;
        }

        eprintln!("parser has {} errors", errors.len());
        for msg in errors {
            eprintln!("parser error: {}", msg);
        }
        panic!("parser has errors");
    }
}
