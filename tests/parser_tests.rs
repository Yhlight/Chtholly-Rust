// tests/parser_tests.rs

use app::parser;
use app::ast::{Program, Statement, MainFunction};

#[test]
fn test_parse_main_function() {
    let source = "fn main() {}";
    let expected_ast = Program {
        body: vec![Statement::MainFunction(MainFunction)],
    };
    let ast = parser::parse(source).unwrap();
    assert_eq!(ast, expected_ast);
}
