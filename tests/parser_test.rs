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
