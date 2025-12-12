use crate::ast::{Statement, Expression};
use crate::lexer::Lexer;
use crate::parser::Parser;

#[test]
fn test_let_statements() {
    let input = r#"
        let x = 5;
        let mut y = 10;
        let foobar = 838383;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 3);

    let expected = vec![
        ("x", false),
        ("y", true),
        ("foobar", false),
    ];

    for (i, statement) in program.iter().enumerate() {
        if let Statement::Let { name, mutable, .. } = statement {
            assert_eq!(name, expected[i].0);
            assert_eq!(*mutable, expected[i].1);
        } else {
            assert!(false, "Expected a let statement, but got something else.");
        }
    }
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

    assert_eq!(program.len(), 3);

    for statement in program.iter() {
        assert!(matches!(statement, Statement::Return(_)));
    }
}

#[test]
fn test_identifier_expression() {
    let input = "foobar;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 1);

    if let Statement::Expression(Expression::Identifier(name)) = &program[0] {
        assert_eq!(name, "foobar");
    } else {
        assert!(false, "Expected an identifier expression, but got something else.");
    }
}

#[test]
fn test_integer_literal_expression() {
    let input = "5;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 1);

    if let Statement::Expression(Expression::Integer(value)) = &program[0] {
        assert_eq!(*value, 5);
    } else {
        assert!(false, "Expected an integer literal expression, but got something else.");
    }
}

#[test]
fn test_if_statement() {
    let input = "if (x > y) { x }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 1);

    if let Statement::If { condition, consequence, alternative } = &program[0] {
        assert!(matches!(condition, Expression::Infix(_, _, _)));
        assert!(matches!(**consequence, Statement::Block(_)));
        assert!(alternative.is_none());
    } else {
        panic!("Expected an if statement, but got something else.");
    }
}

#[test]
fn test_if_else_statement() {
    let input = "if (x > y) { x } else { y }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 1);

    if let Statement::If { condition, consequence, alternative } = &program[0] {
        assert!(matches!(condition, Expression::Infix(_, _, _)));
        assert!(matches!(**consequence, Statement::Block(_)));
        assert!(alternative.is_some());
    } else {
        panic!("Expected an if-else statement, but got something else.");
    }
}
