// It's idiomatic to put integration tests in a `tests` directory next to `src`
// https://doc.rust-lang.org/book/ch11-03-test-organization.html#integration-tests

// Note that we are in a different crate, so we need to use `chthollyc` to refer
// to the library crate.
use chthollyc::ast::{
    BinaryOp, Expression, FunctionCall, FunctionDeclaration, LiteralValue, Parameter, Statement,
    Type,
};
use chthollyc::parser::{
    parse_comment, parse_expression, parse_let_statement, parse_literal, parse_program, parse_type,
};

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
fn test_parse_type() {
    assert_eq!(parse_type("i32"), Ok(("", Type::I32)));
    assert_eq!(parse_type("bool"), Ok(("", Type::Bool)));
    assert!(parse_type("string").is_err());
}

#[test]
fn test_parse_literal() {
    assert_eq!(
        parse_literal("123"),
        Ok((
            "",
            Expression::Literal(LiteralValue::Integer(123))
        ))
    );
    assert_eq!(
        parse_literal("true"),
        Ok((
            "",
            Expression::Literal(LiteralValue::Boolean(true))
        ))
    );
}

#[test]
fn test_parse_let_statement_immutable() {
    let input = "let x: i32 = 10;";
    let expected = Statement::Let {
        name: "x".to_string(),
        is_mutable: false,
        type_annotation: Some(Type::I32),
        value: Expression::Literal(LiteralValue::Integer(10)),
    };
    assert_eq!(parse_let_statement(input), Ok(("", expected)));
}

#[test]
fn test_parse_function_body_with_expression_statement() {
    let input = "fn main() { myFunction(); }";
    let expected = vec![Statement::FunctionDeclaration(FunctionDeclaration {
        name: "main".to_string(),
        parameters: vec![],
        return_type: Type::Void,
        body: vec![Statement::ExpressionStatement(Expression::FunctionCall(
            FunctionCall {
                name: "myFunction".to_string(),
                arguments: vec![],
            },
        ))],
    })];
    assert_eq!(parse_program(input), Ok(("", expected)));
}

#[test]
fn test_parse_function_declaration_no_params_no_return() {
    let input = "fn myFunction() { let x = 5; }";
    let expected = vec![Statement::FunctionDeclaration(FunctionDeclaration {
        name: "myFunction".to_string(),
        parameters: vec![],
        return_type: Type::Void,
        body: vec![Statement::Let {
            name: "x".to_string(),
            is_mutable: false,
            type_annotation: None,
            value: Expression::Literal(LiteralValue::Integer(5)),
        }],
    })];
    assert_eq!(parse_program(input), Ok(("", expected)));
}

#[test]
fn test_parse_function_declaration_with_params_and_return() {
    let input = "fn add(a: i32, b: i32): i32 { }";
    let expected = vec![Statement::FunctionDeclaration(FunctionDeclaration {
        name: "add".to_string(),
        parameters: vec![
            Parameter {
                name: "a".to_string(),
                type_annotation: Type::I32,
            },
            Parameter {
                name: "b".to_string(),
                type_annotation: Type::I32,
            },
        ],
        return_type: Type::I32,
        body: vec![],
    })];
    assert_eq!(parse_program(input), Ok(("", expected)));
}

#[test]
fn test_no_nested_functions() {
    let input = "fn outer() { fn inner() {} }";
    let result = parse_program(input);
    assert!(result.is_err());
}

#[test]
fn test_parse_function_call_no_args() {
    let input = "myFunction()";
    let expected = Expression::FunctionCall(FunctionCall {
        name: "myFunction".to_string(),
        arguments: vec![],
    });
    assert_eq!(parse_expression(input), Ok(("", expected)));
}

#[test]
fn test_parse_function_call_with_args() {
    let input = "add(1, 2 * 3)";
    let expected = Expression::FunctionCall(FunctionCall {
        name: "add".to_string(),
        arguments: vec![
            Expression::Literal(LiteralValue::Integer(1)),
            Expression::Binary {
                op: BinaryOp::Multiply,
                left: Box::new(Expression::Literal(LiteralValue::Integer(2))),
                right: Box::new(Expression::Literal(LiteralValue::Integer(3))),
            },
        ],
    });
    assert_eq!(parse_expression(input), Ok(("", expected)));
}

#[test]
fn test_parse_expression_simple() {
    let input = "1 + 2";
    let expected = Expression::Binary {
        op: BinaryOp::Add,
        left: Box::new(Expression::Literal(LiteralValue::Integer(1))),
        right: Box::new(Expression::Literal(LiteralValue::Integer(2))),
    };
    assert_eq!(parse_expression(input), Ok(("", expected)));
}

#[test]
fn test_parse_expression_precedence() {
    let input = "1 + 2 * 3";
    let expected = Expression::Binary {
        op: BinaryOp::Add,
        left: Box::new(Expression::Literal(LiteralValue::Integer(1))),
        right: Box::new(Expression::Binary {
            op: BinaryOp::Multiply,
            left: Box::new(Expression::Literal(LiteralValue::Integer(2))),
            right: Box::new(Expression::Literal(LiteralValue::Integer(3))),
        }),
    };
    assert_eq!(parse_expression(input), Ok(("", expected)));
}

#[test]
fn test_parse_expression_parentheses() {
    let input = "(1 + 2) * 3";
    let expected = Expression::Binary {
        op: BinaryOp::Multiply,
        left: Box::new(Expression::Binary {
            op: BinaryOp::Add,
            left: Box::new(Expression::Literal(LiteralValue::Integer(1))),
            right: Box::new(Expression::Literal(LiteralValue::Integer(2))),
        }),
        right: Box::new(Expression::Literal(LiteralValue::Integer(3))),
    };
    assert_eq!(parse_expression(input), Ok(("", expected)));
}

#[test]
fn test_parse_let_statement_with_expression() {
    let input = "let x = 1 + 2 * 3;";
    let expected = Statement::Let {
        name: "x".to_string(),
        is_mutable: false,
        type_annotation: None,
        value: Expression::Binary {
            op: BinaryOp::Add,
            left: Box::new(Expression::Literal(LiteralValue::Integer(1))),
            right: Box::new(Expression::Binary {
                op: BinaryOp::Multiply,
                left: Box::new(Expression::Literal(LiteralValue::Integer(2))),
                right: Box::new(Expression::Literal(LiteralValue::Integer(3))),
            }),
        },
    };
    assert_eq!(parse_let_statement(input), Ok(("", expected)));
}

#[test]
fn test_parse_let_statement_mutable() {
    let input = "let mut y: bool = false;";
    let expected = Statement::Let {
        name: "y".to_string(),
        is_mutable: true,
        type_annotation: Some(Type::Bool),
        value: Expression::Literal(LiteralValue::Boolean(false)),
    };
    assert_eq!(parse_let_statement(input), Ok(("", expected)));
}

#[test]
fn test_parse_let_statement_no_type_annotation() {
    let input = "let z = 100;";
    let expected = Statement::Let {
        name: "z".to_string(),
        is_mutable: false,
        type_annotation: None,
        value: Expression::Literal(LiteralValue::Integer(100)),
    };
    assert_eq!(parse_let_statement(input), Ok(("", expected)));
}
