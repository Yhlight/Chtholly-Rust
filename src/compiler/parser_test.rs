use crate::compiler::lexer::Lexer;
use crate::compiler::parser::{Expression, Parser, Statement};

#[test]
fn test_let_statement() {
    let input = "let x = 5;";
    let mut lexer = Lexer::new(input);
    let mut parser = Parser::new(&mut lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 1);

    let statement = &program[0];
    match statement {
        Statement::Let(name, value) => {
            assert_eq!(name, "x");
            match value {
                Expression::IntegerLiteral(val) => assert_eq!(*val, 5),
                _ => panic!("Expected integer literal"),
            }
        }
        _ => panic!("Expected let statement"),
    }
}

#[test]
fn test_prefix_expression() {
    let input = "!5; -15;";
    let mut lexer = Lexer::new(input);
    let mut parser = Parser::new(&mut lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 2);

    let expected = vec![
        ("!", 5),
        ("-", 15),
    ];

    for (i, (op, val)) in expected.iter().enumerate() {
        match &program[i] {
            Statement::Expression(Expression::Prefix(token, right)) => {
                assert_eq!(token_to_string(token), *op);
                match **right {
                    Expression::IntegerLiteral(v) => assert_eq!(v, *val),
                    _ => panic!("Expected integer literal"),
                }
            }
            _ => panic!("Expected prefix expression"),
        }
    }
}

#[test]
fn test_infix_expression() {
    let input = "5 + 5; 5 - 5; 5 * 5; 5 / 5; 5 > 5; 5 < 5; 5 == 5; 5 != 5;";
    let mut lexer = Lexer::new(input);
    let mut parser = Parser::new(&mut lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 8);

    let expected = vec![
        ("+"),
        ("-"),
        ("*"),
        ("/"),
        (">"),
        ("<"),
        ("=="),
        ("!="),
    ];

    for (i, op) in expected.iter().enumerate() {
        match &program[i] {
            Statement::Expression(Expression::Infix(_, token, _)) => {
                assert_eq!(token_to_string(token), *op);
            }
            _ => panic!("Expected infix expression"),
        }
    }
}

#[test]
fn test_boolean_literal() {
    let input = "true; false;";
    let mut lexer = Lexer::new(input);
    let mut parser = Parser::new(&mut lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 2);

    let expected = vec![true, false];

    for (i, val) in expected.iter().enumerate() {
        match &program[i] {
            Statement::Expression(Expression::BooleanLiteral(v)) => assert_eq!(v, val),
            _ => panic!("Expected boolean literal"),
        }
    }
}

fn token_to_string(token: &crate::compiler::lexer::Token) -> String {
    match token {
        crate::compiler::lexer::Token::Plus => "+".to_string(),
        crate::compiler::lexer::Token::Minus => "-".to_string(),
        crate::compiler::lexer::Token::Asterisk => "*".to_string(),
        crate::compiler::lexer::Token::Slash => "/".to_string(),
        crate::compiler::lexer::Token::GreaterThan => ">".to_string(),
        crate::compiler::lexer::Token::LessThan => "<".to_string(),
        crate::compiler::lexer::Token::EqualEqual => "==".to_string(),
        crate::compiler::lexer::Token::NotEqual => "!=".to_string(),
        crate::compiler::lexer::Token::Bang => "!".to_string(),
        _ => "".to_string(),
    }
}
