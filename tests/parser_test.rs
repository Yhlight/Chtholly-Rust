use chtholly::ast::{Expression, Node, Statement};
use chtholly::lexer::Lexer;
use chtholly::parser::Parser;
use std::any::Any;

#[test]
fn test_let_statements() {
    let input = r#"
        let x = 5;
        let y = 10;
        let foobar = 838383;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(
        program.statements.len(),
        3,
        "program.statements does not contain 3 statements. got={}",
        program.statements.len()
    );

    let tests = vec![("x", 5), ("y", 10), ("foobar", 838383)];

    for (i, (name, value)) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        assert_let_statement(stmt, name, *value);
    }
}

#[test]
fn test_mut_statements() {
    let input = r#"
        mut x = 5;
        mut y = 10;
        mut foobar = 838383;
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(
        program.statements.len(),
        3,
        "program.statements does not contain 3 statements. got={}",
        program.statements.len()
    );

    let tests = vec![("x", 5), ("y", 10), ("foobar", 838383)];

    for (i, (name, value)) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        assert_mut_statement(stmt, name, *value);
    }
}

#[test]
fn test_infix_expressions() {
    let infix_tests: Vec<(&str, Box<dyn Any>, &str, Box<dyn Any>)> = vec![
        ("5 + 5;", Box::new(5i64), "+", Box::new(5i64)),
        ("5 - 5;", Box::new(5i64), "-", Box::new(5i64)),
        ("5 * 5;", Box::new(5i64), "*", Box::new(5i64)),
        ("5 / 5;", Box::new(5i64), "/", Box::new(5i64)),
        ("5 > 5;", Box::new(5i64), ">", Box::new(5i64)),
        ("5 < 5;", Box::new(5i64), "<", Box::new(5i64)),
        ("5 == 5;", Box::new(5i64), "==", Box::new(5i64)),
        ("5 != 5;", Box::new(5i64), "!=", Box::new(5i64)),
        ("5 >= 5;", Box::new(5i64), ">=", Box::new(5i64)),
        ("5 <= 5;", Box::new(5i64), "<=", Box::new(5i64)),
        ("true == true", Box::new(true), "==", Box::new(true)),
        ("true != false", Box::new(true), "!=", Box::new(false)),
        ("false == false", Box::new(false), "==", Box::new(false)),
        ("true && true", Box::new(true), "&&", Box::new(true)),
        ("true || false", Box::new(true), "||", Box::new(false)),
    ];

    for (input, left, operator, right) in infix_tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program.statements.len(),
            1,
            "program.statements has not enough statements. got={}",
            program.statements.len()
        );

        if let Statement::ExpressionStatement { expression } = &program.statements[0] {
            assert_infix_expression(expression, left.as_ref(), operator, right.as_ref());
        } else {
            panic!(
                "program.statements[0] is not ExpressionStatement. got={:?}",
                program.statements[0]
            );
        }
    }
}

#[test]
fn test_mut_statements_with_type() {
    let input = r#"
        mut x: int = 5;
        mut y: string = "hello";
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 2);

    let tests = vec![("x", "int"), ("y", "string")];

    for (i, (name, type_val)) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        if let Statement::Mut { name: n, value_type, .. } = stmt {
            assert_eq!(n.value, *name);
            assert!(value_type.is_some());
            assert_eq!(value_type.as_ref().unwrap().value, *type_val);
        } else {
            panic!("stmt not MutStatement. got={:?}", stmt);
        }
    }
}

#[test]
fn test_boolean_expressions() {
    let tests = vec![("true", true), ("false", false)];

    for (input, expected) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program.statements.len(),
            1,
            "program.statements has not enough statements. got={}",
            program.statements.len()
        );

        if let Statement::ExpressionStatement { expression } = &program.statements[0] {
            assert_boolean_literal(expression, expected);
        } else {
            panic!(
                "program.statements[0] is not ExpressionStatement. got={:?}",
                program.statements[0]
            );
        }
    }
}

#[test]
fn test_bang_operator() {
    let tests = vec![("!true", "!", true), ("!false", "!", false)];

    for (input, operator, value) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();

        assert_eq!(
            program.statements.len(),
            1,
            "program.statements has not enough statements. got={}",
            program.statements.len()
        );

        if let Statement::ExpressionStatement { expression } = &program.statements[0] {
            assert_prefix_expression(expression, operator, value);
        } else {
            panic!(
                "program.statements[0] is not ExpressionStatement. got={:?}",
                program.statements[0]
            );
        }
    }
}

#[test]
fn test_if_expression() {
    let input = "if (x < y) { x }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(
        program.statements.len(),
        1,
        "program.statements has not enough statements. got={}",
        program.statements.len()
    );

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::IfExpression { alternative, .. } = expression {
            // For now, we'll just check that the alternative is None.
            assert!(alternative.is_none());
        } else {
            panic!("expression is not IfExpression. got={:?}", expression);
        }
    } else {
        panic!(
            "program.statements[0] is not ExpressionStatement. got={:?}",
            program.statements[0]
        );
    }
}

#[test]
fn test_if_else_expression() {
    let input = "if (x < y) { x } else { y }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(
        program.statements.len(),
        1,
        "program.statements has not enough statements. got={}",
        program.statements.len()
    );

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::IfExpression { alternative, .. } = expression {
            // For now, we'll just check that the alternative is Some.
            assert!(alternative.is_some());
        } else {
            panic!("expression is not IfExpression. got={:?}", expression);
        }
    } else {
        panic!(
            "program.statements[0] is not ExpressionStatement. got={:?}",
            program.statements[0]
        );
    }
}

fn assert_let_statement(stmt: &Statement, name: &str, value: i64) {
    assert_eq!(
        stmt.token_literal(),
        "let",
        "stmt.token_literal not 'let'. got={}",
        stmt.token_literal()
    );

    if let Statement::Let {
        name: n, value: v, ..
    } = stmt
    {
        assert_eq!(
            n.value, name,
            "LetStatement name not '{}'. got={}",
            name, n.value
        );
        assert_eq!(
            n.token_literal(),
            name,
            "s.Name.TokenLiteral() not '{}'. got={}",
            name,
            n.token_literal()
        );
        assert_integer_literal(v, value);
    } else {
        panic!("stmt not LetStatement. got={:?}", stmt);
    }
}

fn assert_mut_statement(stmt: &Statement, name: &str, value: i64) {
    assert_eq!(
        stmt.token_literal(),
        "mut",
        "stmt.token_literal not 'mut'. got={}",
        stmt.token_literal()
    );

    if let Statement::Mut {
        name: n, value: v, ..
    } = stmt
    {
        assert_eq!(
            n.value, name,
            "MutStatement name not '{}'. got={}",
            name, n.value
        );
        assert_eq!(
            n.token_literal(),
            name,
            "s.Name.TokenLiteral() not '{}'. got={}",
            name,
            n.token_literal()
        );
        assert_integer_literal(v, value);
    } else {
        panic!("stmt not MutStatement. got={:?}", stmt);
    }
}

fn assert_integer_literal(expr: &Expression, value: i64) {
    if let Expression::IntegerLiteral { value: v, .. } = expr {
        assert_eq!(*v, value, "IntegerLiteral value not {}. got={}", value, v);
    } else {
        panic!("expr not IntegerLiteral. got={:?}", expr);
    }
}

fn assert_boolean_literal(expr: &Expression, value: bool) {
    if let Expression::BooleanLiteral { value: v, .. } = expr {
        assert_eq!(*v, value, "BooleanLiteral value not {}. got={}", value, v);
    } else {
        panic!("expr not BooleanLiteral. got={:?}", expr);
    }
}

fn assert_prefix_expression(expr: &Expression, op: &str, right: bool) {
    if let Expression::PrefixExpression {
        operator, right: r, ..
    } = expr
    {
        assert_eq!(
            operator, op,
            "exp.Operator is not '{}'. got={}",
            op, operator
        );
        assert_boolean_literal(r, right);
    } else {
        panic!("expr is not PrefixExpression. got={:?}", expr);
    }
}

fn assert_identifier(expr: &Expression, value: &str) {
    if let Expression::Identifier(ident) = expr {
        assert_eq!(ident.value, value);
    } else {
        panic!("expr not Identifier. got={:?}", expr);
    }
}

fn assert_literal_expression(expr: &Expression, expected: &dyn Any) {
    if let Some(value) = expected.downcast_ref::<i64>() {
        assert_integer_literal(expr, *value);
    } else if let Some(value) = expected.downcast_ref::<bool>() {
        assert_boolean_literal(expr, *value);
    } else if let Some(value) = expected.downcast_ref::<String>() {
        assert_identifier(expr, value);
    } else {
        panic!("type of expected not handled. got={:?}", expected);
    }
}

fn assert_infix_expression(expr: &Expression, left: &dyn Any, op: &str, right: &dyn Any) {
    if let Expression::InfixExpression {
        left: l,
        operator,
        right: r,
        ..
    } = expr
    {
        assert_literal_expression(l, left);
        assert_eq!(
            operator, op,
            "exp.Operator is not '{}'. got={}",
            op, operator
        );
        assert_literal_expression(r, right);
    } else {
        panic!("expr is not InfixExpression. got={:?}", expr);
    }
}

#[test]
fn test_while_expression() {
    let input = "while (x < y) { x }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(
        program.statements.len(),
        1,
        "program.statements has not enough statements. got={}",
        program.statements.len()
    );

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::WhileExpression { condition, body, .. } = expression {
            assert_infix_expression(condition, &"x".to_string(), "<", &"y".to_string());

            if let Statement::Block(statements) = &**body {
                assert_eq!(statements.len(), 1);
                if let Statement::ExpressionStatement { expression: body_expr } = &statements[0] {
                    assert_identifier(body_expr, "x");
                } else {
                    panic!("statement in body is not ExpressionStatement. got={:?}", statements[0]);
                }
            } else {
                panic!("body is not BlockStatement. got={:?}", body);
            }
        } else {
            panic!("expression is not WhileExpression. got={:?}", expression);
        }
    } else {
        panic!(
            "program.statements[0] is not ExpressionStatement. got={:?}",
            program.statements[0]
        );
    }
}

#[test]
fn test_continue_statement() {
    let input = "while (true) { continue; }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::WhileExpression { body, .. } = expression {
            if let Statement::Block(statements) = &**body {
                assert_eq!(statements.len(), 1);
                if let Statement::Continue(_) = &statements[0] {
                    // success
                } else {
                    panic!("statement is not ContinueStatement. got={:?}", statements[0]);
                }
            } else {
                panic!("body is not BlockStatement. got={:?}", body);
            }
        } else {
            panic!("expression is not WhileExpression. got={:?}", expression);
        }
    } else {
        panic!("program.statements[0] is not ExpressionStatement. got={:?}", program.statements[0]);
    }
}

#[test]
fn test_let_statements_with_type() {
    let input = r#"
        let x: int = 5;
        let y: string = "hello";
    "#;

    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 2);

    let tests = vec![("x", "int"), ("y", "string")];

    for (i, (name, type_val)) in tests.iter().enumerate() {
        let stmt = &program.statements[i];
        if let Statement::Let { name: n, value_type, .. } = stmt {
            assert_eq!(n.value, *name);
            assert!(value_type.is_some());
            assert_eq!(value_type.as_ref().unwrap().value, *type_val);
        } else {
            panic!("stmt not LetStatement. got={:?}", stmt);
        }
    }
}

#[test]
fn test_float_literal_expression() {
    let input = "3.14;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::FloatLiteral { value, .. } = expression {
            assert_eq!(*value, 3.14);
        } else {
            panic!("expression not FloatLiteral. got={:?}", expression);
        }
    } else {
        panic!("program.statements[0] is not ExpressionStatement. got={:?}", program.statements[0]);
    }
}

#[test]
fn test_char_literal_expression() {
    let input = r#"'a';"#;
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::CharLiteral { value, .. } = expression {
            assert_eq!(*value, 'a');
        } else {
            panic!("expression not CharLiteral. got={:?}", expression);
        }
    } else {
        panic!("program.statements[0] is not ExpressionStatement. got={:?}", program.statements[0]);
    }
}

#[test]
fn test_break_statement() {
    let input = "while (true) { break; }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::WhileExpression { body, .. } = expression {
            if let Statement::Block(statements) = &**body {
                assert_eq!(statements.len(), 1);
                if let Statement::Break(_) = &statements[0] {
                    // success
                } else {
                    panic!("statement is not BreakStatement. got={:?}", statements[0]);
                }
            } else {
                panic!("body is not BlockStatement. got={:?}", body);
            }
        } else {
            panic!("expression is not WhileExpression. got={:?}", expression);
        }
    } else {
        panic!("program.statements[0] is not ExpressionStatement. got={:?}", program.statements[0]);
    }
}

#[test]
fn test_string_literal_expression() {
    let input = r#""hello world";"#;
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();

    assert_eq!(program.statements.len(), 1);

    if let Statement::ExpressionStatement { expression } = &program.statements[0] {
        if let Expression::StringLiteral { value, .. } = expression {
            assert_eq!(value, "hello world");
        } else {
            panic!("expression not StringLiteral. got={:?}", expression);
        }
    } else {
        panic!("program.statements[0] is not ExpressionStatement. got={:?}", program.statements[0]);
    }
}
