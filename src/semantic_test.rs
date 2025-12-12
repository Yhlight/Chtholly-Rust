use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::semantic::SemanticAnalyzer;

#[test]
fn test_type_checking() {
    let input = r#"
        let x = 5;
        let y = 10.5;
        let z = x + y;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);

    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "type mismatch: Integer and Float"
    );
}

#[test]
fn test_undeclared_variable() {
    let input = "x + 5;";

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);

    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "undeclared variable: x"
    );
}

#[test]
fn test_reassign_to_immutable_variable() {
    let input = r#"
        let x = 5;
        x = 10;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);

    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "cannot assign to immutable variable `x`"
    );
}

#[test]
fn test_if_condition_not_boolean() {
    let input = "if (5) { 10 }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);
    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "if condition must be a boolean, but got Integer"
    );
}

#[test]
fn test_if_scoping() {
    let input = r#"
        let x = 5;
        if (true) {
            let x = 10;
        }
    "#;
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);
    assert_eq!(analyzer.errors.len(), 0);
}
