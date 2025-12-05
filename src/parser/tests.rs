use crate::lexer::lexer::Lexer;
use crate::parser::parser::Parser;
use crate::ast::ast::{Statement, Expression};

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
        assert_eq!(name_value.to_string(), expected_identifier.to_string());
    }
}

#[test]
fn test_identifier_expression() {
    let input = "foobar;";
    let lexer = Lexer::new(input.to_string());
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);
    let stmt = &program.statements[0];

    if let Statement::ExpressionStatement { expression, .. } = stmt {
        if let Expression::Identifier(ident) = expression {
            assert_eq!(ident.value, "foobar");
        } else {
            panic!("Expected Identifier, got {:?}", expression);
        }
    } else {
        panic!("Expected ExpressionStatement, got {:?}", stmt);
    }
}

#[test]
fn test_integer_literal_expression() {
    let input = "5;";
    let lexer = Lexer::new(input.to_string());
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);
    let stmt = &program.statements[0];

    if let Statement::ExpressionStatement { expression, .. } = stmt {
        if let Expression::IntegerLiteral { value, .. } = expression {
            assert_eq!(*value, 5);
        } else {
            panic!("Expected IntegerLiteral, got {:?}", expression);
        }
    } else {
        panic!("Expected ExpressionStatement, got {:?}", stmt);
    }
}

#[test]
fn test_parsing_prefix_expressions() {
    let prefix_tests = vec![("!5;", "!", 5), ("-15;", "-", 15)];

    for (input, operator, value) in prefix_tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);
        let stmt = &program.statements[0];

        if let Statement::ExpressionStatement { expression, .. } = stmt {
            if let Expression::PrefixExpression {
                operator: op,
                right,
                ..
            } = expression
            {
                assert_eq!(op, operator);
                if let Expression::IntegerLiteral { value: val, .. } = &**right {
                    assert_eq!(*val, value);
                } else {
                    panic!("Expected IntegerLiteral, got {:?}", right);
                }
            } else {
                panic!("Expected PrefixExpression, got {:?}", expression);
            }
        } else {
            panic!("Expected ExpressionStatement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_parsing_infix_expressions() {
    let infix_tests = vec![
        ("5 + 5;", 5, "+", 5),
        ("5 - 5;", 5, "-", 5),
        ("5 * 5;", 5, "*", 5),
        ("5 / 5;", 5, "/", 5),
        ("5 > 5;", 5, ">", 5),
        ("5 < 5;", 5, "<", 5),
        ("5 == 5;", 5, "==", 5),
        ("5 != 5;", 5, "!=", 5),
    ];

    for (input, left_value, operator, right_value) in infix_tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);
        let stmt = &program.statements[0];

        if let Statement::ExpressionStatement { expression, .. } = stmt {
            if let Expression::InfixExpression {
                left,
                operator: op,
                right,
                ..
            } = expression
            {
                if let Expression::IntegerLiteral { value, .. } = &**left {
                    assert_eq!(*value, left_value);
                } else {
                    panic!("Expected IntegerLiteral, got {:?}", left);
                }
                assert_eq!(op, operator);
                if let Expression::IntegerLiteral { value, .. } = &**right {
                    assert_eq!(*value, right_value);
                } else {
                    panic!("Expected IntegerLiteral, got {:?}", right);
                }
            } else {
                panic!("Expected InfixExpression, got {:?}", expression);
            }
        } else {
            panic!("Expected ExpressionStatement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_operator_precedence_parsing() {
    let tests = vec![
        ("-a * b", "((-a) * b)"),
        ("!-a", "(!(-a))"),
        ("a + b + c", "((a + b) + c)"),
        ("a + b - c", "((a + b) - c)"),
        ("a * b * c", "((a * b) * c)"),
        ("a * b / c", "((a * b) / c)"),
        ("a + b / c", "(a + (b / c))"),
        ("a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"),
        ("3 + 4; -5 * 5", "(3 + 4)"),
        ("5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"),
        ("5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"),
        (
            "3 + 4 * 5 == 3 * 1 + 4 * 5",
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))",
        ),
    ];

    for (input, expected) in tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        let actual = program.statements[0].to_string();
        assert_eq!(actual, expected);
    }
}
