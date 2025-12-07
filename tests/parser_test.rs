// tests/parser_test.rs

use chtholly_lib::parser;

#[test]
fn test_parse_single_line_comment() {
    let input = "// This is a single-line comment\n";
    let result = parser::parse(input);
    assert!(result.is_ok());
}

#[test]
fn test_parse_multi_line_comment() {
    let input = "/* This is a multi-line comment */";
    let result = parser::parse(input);
    assert!(result.is_ok());
}

#[test]
fn test_parse_main_function() {
    let input = "fn main(args: string[]): Result<i32, SystemError> {}";
    let result = parser::parse(input);
    assert!(result.is_ok());
}
