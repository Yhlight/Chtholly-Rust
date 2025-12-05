use chtholly::ast::{Expression, Statement};
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;

#[test]
fn test_let_statement() {
    let input = "let x = 5;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Let(name, value) = statement {
        assert_eq!(name, "x");
        if let Expression::Int(val) = value {
            assert_eq!(*val, 5);
        } else {
            panic!("value is not an Int");
        }
    } else {
        panic!("statement is not a Let statement");
    }
}

#[test]
fn test_mut_statement() {
    let input = "mut x = 5;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Mut(name, value) = statement {
        assert_eq!(name, "x");
        if let Expression::Int(val) = value {
            assert_eq!(*val, 5);
        } else {
            panic!("value is not an Int");
        }
    } else {
        panic!("statement is not a Mut statement");
    }
}

#[test]
fn test_if_expression() {
    let input = "if (x < y) { x }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::If {
            condition: _condition,
            consequence,
            alternative,
        } = expression
        {
            assert!(alternative.is_none());
            assert_eq!(consequence.len(), 1);
        } else {
            panic!("expression is not an If expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_if_else_expression() {
    let input = "if (x < y) { x } else { y }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::If {
            condition: _condition,
            consequence,
            alternative,
        } = expression
        {
            assert!(alternative.is_some());
            assert_eq!(consequence.len(), 1);
            assert_eq!(alternative.as_ref().unwrap().len(), 1);
        } else {
            panic!("expression is not an If expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}
