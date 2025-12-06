use chtholly::ast::{Expression, Infix, Statement};
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

#[test]
fn test_index_expression_parsing() {
    let input = "myArray[1 + 1]";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::Index(left, _index) = expression {
            if let Expression::Identifier(name) = &**left {
                assert_eq!(name, "myArray");
            } else {
                panic!("left is not an Identifier");
            }
        } else {
            panic!("expression is not an Index expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_while_expression() {
    let input = "while (x < y) { x }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::While { condition, body } = expression {
            if let Expression::Infix(Infix::LessThan, left, right) = &**condition {
                if let Expression::Identifier(name) = &**left {
                    assert_eq!(name, "x");
                } else {
                    panic!("left is not an Identifier");
                }
                if let Expression::Identifier(name) = &**right {
                    assert_eq!(name, "y");
                } else {
                    panic!("right is not an Identifier");
                }
            } else {
                panic!("condition is not an Infix expression");
            }
            assert_eq!(body.len(), 1);
        } else {
            panic!("expression is not a While expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_array_literal_parsing() {
    let input = "[1, 2 * 2, 3 + 3]";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::Array(elements) = expression {
            assert_eq!(elements.len(), 3);
        } else {
            panic!("expression is not an Array literal");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_mut_statement() {
    let input = "mut x = 5;";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Mut(name, value) = statement {
        assert_eq!(name, "x");
        if let Expression::Int(val) = value {
            assert_eq!(*val, 5);
        } else {
            panic!("value is not an Int");
        }
    } else {
        panic!("statement is not a Mut statement");
    }
}

#[test]
fn test_if_expression() {
    let input = "if (x < y) { x }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::If {
            condition: _condition,
            consequence,
            alternative,
        } = expression
        {
            assert!(alternative.is_none());
            assert_eq!(consequence.len(), 1);
        } else {
            panic!("expression is not an If expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_operator_precedence_parsing() {
    let tests = vec![
        (
            "1 + 2 * 3",
            Expression::Infix(
                Infix::Plus,
                Box::new(Expression::Int(1)),
                Box::new(Expression::Infix(
                    Infix::Star,
                    Box::new(Expression::Int(2)),
                    Box::new(Expression::Int(3)),
                )),
            ),
        ),
        (
            "(1 + 2) * 3",
            Expression::Infix(
                Infix::Star,
                Box::new(Expression::Infix(
                    Infix::Plus,
                    Box::new(Expression::Int(1)),
                    Box::new(Expression::Int(2)),
                )),
                Box::new(Expression::Int(3)),
            ),
        ),
    ];

    for (input, expected) in tests {
        let lexer = Lexer::new(input);
        let mut parser = Parser::new(lexer);
        let program = parser.parse_program();
        assert_eq!(program.len(), 1);

        let statement = &program[0];
        if let Statement::Expression(expression) = statement {
            assert_eq!(*expression, expected);
        } else {
            panic!("statement is not an Expression statement");
        }
    }
}

#[test]
fn test_character_literal_expression() {
    let input = "'a';";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::Char(value) = expression {
            assert_eq!(*value, 'a');
        } else {
            panic!("expression is not a Char literal");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_string_literal_expression() {
    let input = "\"hello world\";";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::String(value) = expression {
            assert_eq!(value, "hello world");
        } else {
            panic!("expression is not a String literal");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_call_expression_parsing() {
    let input = "add(1, 2 * 3, 4 + 5);";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::Call {
            function,
            arguments,
        } = expression
        {
            assert_eq!(arguments.len(), 3);
            if let Expression::Identifier(name) = &**function {
                assert_eq!(name, "add");
            } else {
                panic!("function is not an Identifier");
            }
        } else {
            panic!("expression is not a Call expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_function_literal_parsing() {
    let input = "fn(x, y) { x + y; }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::Function { parameters, body } = expression {
            assert_eq!(parameters.len(), 2);
            assert_eq!(parameters[0], "x");
            assert_eq!(parameters[1], "y");
            assert_eq!(body.len(), 1);
        } else {
            panic!("expression is not a Function literal");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}

#[test]
fn test_if_else_expression() {
    let input = "if (x < y) { x } else { y }";
    let lexer = Lexer::new(input);
    let mut parser = Parser::new(lexer);
    let program = parser.parse_program();
    assert_eq!(program.len(), 1);

    let statement = &program[0];
    if let Statement::Expression(expression) = statement {
        if let Expression::If {
            condition: _condition,
            consequence,
            alternative,
        } = expression
        {
            assert!(alternative.is_some());
            assert_eq!(consequence.len(), 1);
            assert_eq!(alternative.as_ref().unwrap().len(), 1);
        } else {
            panic!("expression is not an If expression");
        }
    } else {
        panic!("statement is not an Expression statement");
    }
}
