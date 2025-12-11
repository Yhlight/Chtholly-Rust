// It's idiomatic to put integration tests in a `tests` directory next to `src`
// https://doc.rust-lang.org/book/ch11-03-test-organization.html#integration-tests

// Note that we are in a different crate, so we need to use `app` to refer
// to the library crate.
use app::parser::{parse_comment, parse_main_function};

#[test]
fn test_parse_single_line_comment() {
    let input = "// this is a comment\n";
    let result = parse_comment(input);
    assert_eq!(result, Ok(("\n", " this is a comment")));
}

#[test]
fn test_parse_multi_line_comment() {
    let input = "/* this is a multi-line comment */";
    let result = parse_comment(input);
    assert_eq!(result, Ok(("", " this is a multi-line comment ")));
}

#[test]
fn test_parse_main_function() {
    let input = r#"
        fn main(args: string[]): Result<i32, SystemError> {
            // some code
        }
    "#;
    let result = parse_main_function(input);
    assert!(result.is_ok());
    let (remaining, body) = result.unwrap();
    assert_eq!(remaining.trim(), "");
    assert_eq!(body.trim(), "// some code");
}
