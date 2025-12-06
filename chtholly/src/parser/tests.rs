use crate::ast::{self, Expression, Program, Statement, Type};
use crate::lexer::Lexer;
use crate::parser::Parser;
use crate::token::Token;

#[test]
fn test_let_statements() {
    let input = "
        let x: int = 5;
        let y: double = 10;
        let foobar: bool = true;
    "
    .to_string();

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3);

    let names = vec!["x", "y", "foobar"];
    let types = vec![Type::Int, Type::Double, Type::Bool];

    for (i, (name, type_)) in names.iter().zip(types.iter()).enumerate() {
        let stmt = &program.statements[i];
        if let Statement::Let(ident, t, _) = stmt {
            assert_eq!(ident.0, *name);
            assert_eq!(t.as_ref().unwrap(), type_);
        } else {
            panic!("expected let statement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_mut_statements() {
    let input = "
        mut x: int = 5;
        mut y: double = 10;
        mut foobar: bool = true;
    "
    .to_string();

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3);

    let names = vec!["x", "y", "foobar"];
    let types = vec![Type::Int, Type::Double, Type::Bool];

    for (i, (name, type_)) in names.iter().zip(types.iter()).enumerate() {
        let stmt = &program.statements[i];
        if let Statement::Mut(ident, t, _) = stmt {
            assert_eq!(ident.0, *name);
            assert_eq!(t.as_ref().unwrap(), type_);
        } else {
            panic!("expected mut statement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_return_statements() {
    let input = "
        return 5;
        return 10;
        return 993322;
    "
    .to_string();

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 3);

    for stmt in program.statements {
        if let Statement::Return(_) = stmt {
        } else {
            panic!("expected return statement, got {:?}", stmt);
        }
    }
}

#[test]
fn test_identifier_expression() {
    let input = "foobar;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::Identifier(ident) = expr {
            assert_eq!(ident.0, "foobar");
        } else {
            panic!("expected identifier, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_integer_literal_expression() {
    let input = "5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::IntLiteral(value) = expr {
            assert_eq!(*value, 5);
        } else {
            panic!("expected integer literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_float_literal_expression() {
    let input = "5.5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::FloatLiteral(value) = expr {
            assert_eq!(*value, 5.5);
        } else {
            panic!("expected float literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_string_literal_expression() {
    let input = "\"hello world\";".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::StringLiteral(value) = expr {
            assert_eq!(*value, "hello world");
        } else {
            panic!("expected string literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_boolean_expression() {
    let input = "true;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::Boolean(value) = expr {
            assert_eq!(*value, true);
        } else {
            panic!("expected boolean, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_prefix_expressions() {
    let prefix_tests = vec![
        ("!5;", Token::Bang, 5),
        ("-15;", Token::Minus, 15),
    ];

    for (input, operator, value) in prefix_tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        if let Statement::Expression(expr) = &program.statements[0] {
            if let Expression::Prefix(op, right) = expr {
                assert_eq!(*op, operator);
                if let Expression::IntLiteral(val) = **right {
                    assert_eq!(val, value);
                } else {
                    panic!("expected integer literal, got {:?}", right);
                }
            } else {
                panic!("expected prefix expression, got {:?}", expr);
            }
        } else {
            panic!("expected expression statement, got {:?}", program.statements[0]);
        }
    }
}

#[test]
fn test_infix_expressions() {
    let infix_tests = vec![
        ("5 + 5;", 5, Token::Plus, 5),
        ("5 - 5;", 5, Token::Minus, 5),
        ("5 * 5;", 5, Token::Asterisk, 5),
        ("5 / 5;", 5, Token::Slash, 5),
        ("5 > 5;", 5, Token::GreaterThan, 5),
        ("5 < 5;", 5, Token::LessThan, 5),
        ("5 == 5;", 5, Token::Equal, 5),
        ("5 != 5;", 5, Token::NotEqual, 5),
    ];

    for (input, left_value, operator, right_value) in infix_tests {
        let lexer = Lexer::new(input.to_string());
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(program.statements.len(), 1);

        if let Statement::Expression(expr) = &program.statements[0] {
            if let Expression::Infix(left, op, right) = expr {
                if let Expression::IntLiteral(val) = **left {
                    assert_eq!(val, left_value);
                } else {
                    panic!("expected integer literal, got {:?}", left);
                }

                assert_eq!(*op, operator);

                if let Expression::IntLiteral(val) = **right {
                    assert_eq!(val, right_value);
                } else {
                    panic!("expected integer literal, got {:?}", right);
                }
            } else {
                panic!("expected infix expression, got {:?}", expr);
            }
        } else {
            panic!("expected expression statement, got {:?}", program.statements[0]);
        }
    }
}

#[test]
fn test_if_expression() {
    let input = "if (x < y) { x }".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::If {
            condition,
            consequence,
            alternative,
        } = expr
        {
            if let Expression::Infix(left, op, right) = &**condition {
                if let Expression::Identifier(ident) = &**left {
                    assert_eq!(ident.0, "x");
                } else {
                    panic!("expected identifier, got {:?}", left);
                }
                assert_eq!(*op, Token::LessThan);
                if let Expression::Identifier(ident) = &**right {
                    assert_eq!(ident.0, "y");
                } else {
                    panic!("expected identifier, got {:?}", right);
                }
            } else {
                panic!("expected infix expression, got {:?}", condition);
            }

            assert_eq!(consequence.statements.len(), 1);
            if let Statement::Expression(expr) = &consequence.statements[0] {
                if let Expression::Identifier(ident) = expr {
                    assert_eq!(ident.0, "x");
                } else {
                    panic!("expected identifier, got {:?}", expr);
                }
            } else {
                panic!(
                    "expected expression statement, got {:?}",
                    consequence.statements[0]
                );
            }

            assert!(alternative.is_none());
        } else {
            panic!("expected if expression, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_if_else_expression() {
    let input = "if (x < y) { x } else { y }".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::If {
            condition,
            consequence,
            alternative,
        } = expr
        {
            if let Expression::Infix(left, op, right) = &**condition {
                if let Expression::Identifier(ident) = &**left {
                    assert_eq!(ident.0, "x");
                } else {
                    panic!("expected identifier, got {:?}", left);
                }
                assert_eq!(*op, Token::LessThan);
                if let Expression::Identifier(ident) = &**right {
                    assert_eq!(ident.0, "y");
                } else {
                    panic!("expected identifier, got {:?}", right);
                }
            } else {
                panic!("expected infix expression, got {:?}", condition);
            }

            assert_eq!(consequence.statements.len(), 1);
            if let Statement::Expression(expr) = &consequence.statements[0] {
                if let Expression::Identifier(ident) = expr {
                    assert_eq!(ident.0, "x");
                } else {
                    panic!("expected identifier, got {:?}", expr);
                }
            } else {
                panic!(
                    "expected expression statement, got {:?}",
                    consequence.statements[0]
                );
            }

            assert!(alternative.is_some());
            let alternative = alternative.clone().unwrap();
            assert_eq!(alternative.statements.len(), 1);
            if let Statement::Expression(expr) = &alternative.statements[0] {
                if let Expression::Identifier(ident) = expr {
                    assert_eq!(ident.0, "y");
                } else {
                    panic!("expected identifier, got {:?}", expr);
                }
            } else {
                panic!(
                    "expected expression statement, got {:?}",
                    alternative.statements[0]
                );
            }
        } else {
            panic!("expected if expression, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_function_literal_parsing() {
    let input = "fn(x: int, y: int) { x + y; }".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::FunctionLiteral { parameters, body } = expr {
            assert_eq!(parameters.len(), 2);
            assert_eq!(parameters[0].0.0, "x");
            assert_eq!(parameters[0].1, Type::Int);
            assert_eq!(parameters[1].0.0, "y");
            assert_eq!(parameters[1].1, Type::Int);

            assert_eq!(body.statements.len(), 1);
            if let Statement::Expression(expr) = &body.statements[0] {
                if let Expression::Infix(left, op, right) = expr {
                    if let Expression::Identifier(ident) = &**left {
                        assert_eq!(ident.0, "x");
                    } else {
                        panic!("expected identifier, got {:?}", left);
                    }
                    assert_eq!(*op, Token::Plus);
                    if let Expression::Identifier(ident) = &**right {
                        assert_eq!(ident.0, "y");
                    } else {
                        panic!("expected identifier, got {:?}", right);
                    }
                } else {
                    panic!("expected infix expression, got {:?}", expr);
                }
            } else {
                panic!(
                    "expected expression statement, got {:?}",
                    body.statements[0]
                );
            }
        } else {
            panic!("expected function literal, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_call_expression_parsing() {
    let input = "add(1, 2 * 3, 4 + 5);".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::Expression(expr) = &program.statements[0] {
        if let Expression::Call {
            function,
            arguments,
        } = expr
        {
            if let Expression::Identifier(ident) = &**function {
                assert_eq!(ident.0, "add");
            } else {
                panic!("expected identifier, got {:?}", function);
            }

            assert_eq!(arguments.len(), 3);
        } else {
            panic!("expected call expression, got {:?}", expr);
        }
    } else {
        panic!("expected expression statement, got {:?}", program.statements[0]);
    }
}

#[test]
fn test_invalid_type_annotation() {
    let input = "let x: 123 = 5;".to_string();
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    parser.parse_program();

    assert_eq!(parser.errors().len(), 1);
}
