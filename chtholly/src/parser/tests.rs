use crate::ast::{self, Expression, Program, Statement};
use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::token::Token;

#[test]
fn test_let_statements() {
    let input = "
        let x = 5;
        let y = 10;
        let foobar = 838383;
    "
    .to_string();

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3);

    let names = vec!["x", "y", "foobar"];

    for (i, name) in names.iter().enumerate() {
        let stmt = &program.statements[i];
        if let Statement::Let(ident, _) = stmt {
            assert_eq!(ident.0, *name);
        } else {
            panic!("expected let statement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_return_statements() {
    let input = "
        return 5;
        return 10;
        return 993322;
    "
    .to_string();

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3);

    for stmt in program.statements {
        if let Statement::Return(_) = stmt {
        } else {
            panic!("expected return statement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_identifier_expression() {
    let input = "foobar;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::Identifier(ident) = expr {
            assert_eq!(ident.0, "foobar");
        } else {
            panic!("expected identifier, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_integer_literal_expression() {
    let input = "5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::IntLiteral(value) = expr {
            assert_eq!(*value, 5);
        } else {
            panic!("expected integer literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_float_literal_expression() {
    let input = "5.5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::FloatLiteral(value) = expr {
            assert_eq!(*value, 5.5);
        } else {
            panic!("expected float literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_string_literal_expression() {
    let input = "\"hello world\";".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::StringLiteral(value) = expr {
            assert_eq!(*value, "hello world");
        } else {
            panic!("expected string literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_boolean_expression() {
    let input = "true;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::Boolean(value) = expr {
            assert_eq!(*value, true);
        } else {
            panic!("expected boolean, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_prefix_expressions() {
    let prefix_tests = vec![
        ("!5;", Token::Bang, 5),
        ("-15;", Token::Minus, 15),
    ];

    for (input, operator, value) in prefix_tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        if let Statement::Expression(expr) = &program.statements[0] {
            if let Expression::Prefix(op, right) = expr {
                assert_eq!(*op, operator);
                if let Expression::IntLiteral(val) = **right {
                    assert_eq!(val, value);
                } else {
                    panic!("expected integer literal, got {:?}", right);
                }
            } else {
                panic!("expected prefix expression, got {:?}", expr);
            }
        } else {
            panic!("expected expression statement, got {:?}", program.statements[0]);
        }
    }
}

#[test]
fn test_infix_expressions() {
    let infix_tests = vec![
        ("5 + 5;", 5, Token::Plus, 5),
        ("5 - 5;", 5, Token::Minus, 5),
        ("5 * 5;", 5, Token::Asterisk, 5),
        ("5 / 5;", 5, Token::Slash, 5),
        ("5 > 5;", 5, Token::GreaterThan, 5),
        ("5 < 5;", 5, Token::LessThan, 5),
        ("5 == 5;", 5, Token::Equal, 5),
        ("5 != 5;", 5, Token::NotEqual, 5),
    ];

    for (input, left_value, operator, right_value) in infix_tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        if let Statement::Expression(expr) = &program.statements[0] {
            if let Expression::Infix(left, op, right) = expr {
                if let Expression::IntLiteral(val) = **left {
                    assert_eq!(val, left_value);
                } else {
                    panic!("expected integer literal, got {:?}", left);
                }

                assert_eq!(*op, operator);

                if let Expression::IntLiteral(val) = **right {
                    assert_eq!(val, right_value);
                } else {
                    panic!("expected integer literal, got {:?}", right);
                }
            } else {
                panic!("expected infix expression, got {:?}", expr);
            }
        } else {
            panic!("expected expression statement, got {:?}", program.statements[0]);
        }
    }
}
