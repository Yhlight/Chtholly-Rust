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

fn assert_literal_expression(expr: &Expression, expected: &dyn Any) {
    if let Some(value) = expected.downcast_ref::<i64>() {
        assert_integer_literal(expr, *value);
    } else if let Some(value) = expected.downcast_ref::<bool>() {
        assert_boolean_literal(expr, *value);
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
