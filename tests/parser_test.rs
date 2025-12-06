
use chtholly::ast::{Node, Statement, Expression};
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;

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

    assert_eq!(program.statements.len(), 3, "program.statements does not contain 3 statements. got={}", program.statements.len());

    let tests = vec![
        ("x", 5),
        ("y", 10),
        ("foobar", 838383),
    ];

    for (i, (name, value)) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        assert_let_statement(stmt, name, *value);
    }
}

#[test]
fn test_mut_statements() {
    let input = r#"
        mut x = 5;
        mut y = 10;
        mut foobar = 838383;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3, "program.statements does not contain 3 statements. got={}", program.statements.len());

    let tests = vec![
        ("x", 5),
        ("y", 10),
        ("foobar", 838383),
    ];

    for (i, (name, value)) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        assert_mut_statement(stmt, name, *value);
    }
}

fn assert_let_statement(stmt: &Statement, name: &str, value: i64) {
    assert_eq!(stmt.token_literal(), "let", "stmt.token_literal not 'let'. got={}", stmt.token_literal());

    if let Statement::Let { name: n, value: v, .. } = stmt {
        assert_eq!(n.value, name, "LetStatement name not '{}'. got={}", name, n.value);
        assert_eq!(n.token_literal(), name, "s.Name.TokenLiteral() not '{}'. got={}", name, n.token_literal());
        assert_integer_literal(v, value);
    } else {
        panic!("stmt not LetStatement. got={:?}", stmt);
    }
}

fn assert_mut_statement(stmt: &Statement, name: &str, value: i64) {
    assert_eq!(stmt.token_literal(), "mut", "stmt.token_literal not 'mut'. got={}", stmt.token_literal());

    if let Statement::Mut { name: n, value: v, .. } = stmt {
        assert_eq!(n.value, name, "MutStatement name not '{}'. got={}", name, n.value);
        assert_eq!(n.token_literal(), name, "s.Name.TokenLiteral() not '{}'. got={}", name, n.token_literal());
        assert_integer_literal(v, value);
    } else {
        panic!("stmt not MutStatement. got={:?}", stmt);
    }
}

fn assert_integer_literal(expr: &Expression, value: i64) {
    if let Expression::IntegerLiteral { value: v, .. } = expr {
        assert_eq!(*v, value, "IntegerLiteral value not {}. got={}", value, v);
    } else {
        panic!("expr not IntegerLiteral. got={:?}", expr);
    }
}
