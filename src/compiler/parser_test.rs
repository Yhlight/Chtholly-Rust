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
