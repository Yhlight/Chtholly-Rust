use crate::ast::{Program, Statement, Expression};
use crate::lexer::Lexer;
use crate::parser::Parser;

#[test]
fn test_let_statements() {
    let input = r#"
        let x = 5;
        let y = 10;
        let foobar = 838383;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.len(), 3);

    let expected_identifiers = vec!["x", "y", "foobar"];

    for (i, statement) in program.iter().enumerate() {
        if let Statement::Let(name, _) = statement {
            assert_eq!(name, expected_identifiers[i]);
        } else {
            panic!("Expected a let statement, but got something else.");
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
        if let Statement::Return(_) = statement {
            // expected
        } else {
            panic!("Expected a return statement, but got something else.");
        }
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
        panic!("Expected an identifier expression, but got something else.");
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
        panic!("Expected an integer literal expression, but got something else.");
    }
}
