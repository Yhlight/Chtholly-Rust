
use chtholly::ast::{Node, Statement};
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

    let tests = vec!["x", "y", "foobar"];

    for (i, tt) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        assert_let_statement(stmt, tt);
    }
}

fn assert_let_statement(stmt: &Statement, name: &str) {
    assert_eq!(stmt.token_literal(), "let", "stmt.token_literal not 'let'. got={}", stmt.token_literal());

    if let Statement::Let { name: n, .. } = stmt {
        assert_eq!(n.value, name, "LetStatement name not '{}'. got={}", name, n.value);
        assert_eq!(n.token_literal(), name, "s.Name.TokenLiteral() not '{}'. got={}", name, n.token_literal());
    } else {
        panic!("stmt not LetStatement. got={:?}", stmt);
    }
}
