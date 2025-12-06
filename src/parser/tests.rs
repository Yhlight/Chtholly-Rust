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
fn test_switch_case_expression() {
    let input = r#"
        switch (x) {
            case 1 {
                return true;
            }
            case 2 {
                fallthrough;
            }
            case 3 {
                break;
            }
        }
    "#;

    let lexer = Lexer::new(input.to_string());
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);
    let stmt = &program.statements[0];

    if let Statement::ExpressionStatement { expression, .. } = stmt {
        if let Expression::SwitchExpression {
            condition,
            cases,
            ..
        } = expression
        {
            assert_eq!(condition.to_string(), "x");
            assert_eq!(cases.len(), 3);

            // Test case 1
            if let Statement::CaseStatement { value, body, .. } = &cases[0] {
                assert_eq!(value.to_string(), "1");
                assert_eq!(body.statements.len(), 1);
                assert_eq!(body.statements[0].to_string(), "return true");
            } else {
                panic!("Expected CaseStatement, got {:?}", cases[0]);
            }

            // Test case 2
            if let Statement::CaseStatement { value, body, .. } = &cases[1] {
                assert_eq!(value.to_string(), "2");
                assert_eq!(body.statements.len(), 1);
                assert_eq!(body.statements[0].to_string(), "fallthrough");
            } else {
                panic!("Expected CaseStatement, got {:?}", cases[1]);
            }

            // Test case 3
            if let Statement::CaseStatement { value, body, .. } = &cases[2] {
                assert_eq!(value.to_string(), "3");
                assert_eq!(body.statements.len(), 1);
                assert_eq!(body.statements[0].to_string(), "break");
            } else {
                panic!("Expected CaseStatement, got {:?}", cases[2]);
            }
        } else {
            panic!("Expected SwitchExpression, got {:?}", expression);
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
fn test_boolean_expression() {
    let tests = vec![("true;", true), ("false;", false)];

    for (input, expected) in tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);
        let stmt = &program.statements[0];

        if let Statement::ExpressionStatement { expression, .. } = stmt {
            if let Expression::Boolean { value, .. } = expression {
                assert_eq!(*value, expected);
            } else {
                panic!("Expected Boolean, got {:?}", expression);
            }
        } else {
            panic!("Expected ExpressionStatement, got {:?}", stmt);
        }
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
        ("true", "true"),
        ("false", "false"),
        ("3 > 5 == false", "((3 > 5) == false)"),
        ("3 < 5 == true", "((3 < 5) == true)"),
        ("1 + (2 + 3) + 4", "((1 + (2 + 3)) + 4)"),
        ("(5 + 5) * 2", "((5 + 5) * 2)"),
        ("2 / (5 + 5)", "(2 / (5 + 5))"),
        ("-(5 + 5)", "(-(5 + 5))"),
        ("!(true == true)", "(!(true == true))"),
    ];

    for (input, expected) in tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        let actual = program.statements[0].to_string();
        assert_eq!(actual, expected);
    }
}

#[test]
fn test_if_expression() {
    let input = "if (x < y) { x }";
    let lexer = Lexer::new(input.to_string());
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);
    let stmt = &program.statements[0];

    if let Statement::ExpressionStatement { expression, .. } = stmt {
        if let Expression::IfExpression {
            condition,
            consequence,
            alternative,
            ..
        } = expression
        {
            assert_eq!(condition.to_string(), "(x < y)");
            assert_eq!(consequence.statements.len(), 1);
            let cons_stmt = &consequence.statements[0];
            if let Statement::ExpressionStatement { expression, .. } = cons_stmt {
                assert_eq!(expression.to_string(), "x");
            } else {
                panic!("Expected ExpressionStatement, got {:?}", cons_stmt);
            }
            assert!(alternative.is_none());
        } else {
            panic!("Expected IfExpression, got {:?}", expression);
        }
    } else {
        panic!("Expected ExpressionStatement, got {:?}", stmt);
    }
}

#[test]
fn test_if_else_expression() {
    let input = "if (x < y) { x } else { y }";
    let lexer = Lexer::new(input.to_string());
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);
    let stmt = &program.statements[0];

    if let Statement::ExpressionStatement { expression, .. } = stmt {
        if let Expression::IfExpression {
            condition,
            consequence,
            alternative,
            ..
        } = expression
        {
            assert_eq!(condition.to_string(), "(x < y)");
            assert_eq!(consequence.statements.len(), 1);
            let cons_stmt = &consequence.statements[0];
            if let Statement::ExpressionStatement { expression, .. } = cons_stmt {
                assert_eq!(expression.to_string(), "x");
            } else {
                panic!("Expected ExpressionStatement, got {:?}", cons_stmt);
            }
            assert!(alternative.is_some());
            let alt_stmt = &alternative.as_ref().unwrap().statements[0];
            if let Statement::ExpressionStatement { expression, .. } = alt_stmt {
                assert_eq!(expression.to_string(), "y");
            } else {
                panic!("Expected ExpressionStatement, got {:?}", alt_stmt);
            }
        } else {
            panic!("Expected IfExpression, got {:?}", expression);
        }
    } else {
        panic!("Expected ExpressionStatement, got {:?}", stmt);
    }
}
