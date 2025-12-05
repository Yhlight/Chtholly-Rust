use crate::lexer::lexer::Lexer;
use crate::parser::parser::Parser;
use crate::ast::ast::Statement;

#[test]
fn test_let_statements() {
    let input = r#"
        let x = 5;
        let y = 10;
        let foobar = 838383;
        "#;

    let lexer = Lexer::new(input.to_string());
    let mut parser = Parser::new(lexer);

    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3);

    let expected_identifiers = vec!["x", "y", "foobar"];

    for (i, expected_identifier) in expected_identifiers.iter().enumerate() {
        let statement = &program.statements[i];
        let name_value = match statement {
            Statement::LetStatement { name, .. } => &name.value,
            _ => panic!("Expected LetStatement, got {:?}", statement),
        };
        assert_eq!(name_value, expected_identifier);
    }
}
