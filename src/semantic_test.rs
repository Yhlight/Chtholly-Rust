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
        "type mismatch: I64 and F64"
    );
}

#[test]
fn test_type_annotation_mismatch() {
    let input = r#"
        let x: i64 = 10.5;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);

    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "type mismatch: annotated type I64, but got F64"
    );
}

#[test]
fn test_type_inference() {
    let input = r#"
        let x = 5;
        let y: i64 = x;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);

    assert_eq!(analyzer.errors.len(), 0);
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
        "if condition must be a boolean, but got I64"
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

#[test]
fn test_while_condition_not_boolean() {
    let input = "while (5) { 10 }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);
    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "while condition must be a boolean, but got I64"
    );
}

#[test]
fn test_break_outside_loop() {
    let input = "break;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    let mut analyzer = SemanticAnalyzer::new();
    analyzer.analyze(&program);
    assert_eq!(analyzer.errors.len(), 1);
    assert_eq!(
        analyzer.errors[0],
        "break statement outside of a loop"
    );
}
