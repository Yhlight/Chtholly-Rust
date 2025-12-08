pub mod ast;
pub mod lexer;
pub mod parser;
pub mod compiler;

#[cfg(test)]
mod tests {
    use crate::parser::Parser;
    use crate::compiler::Compiler;
    use crate::ast::{ASTNode, Type, BinaryOperator};
    use inkwell::context::Context;

    #[test]
    fn test_parse_comment_and_function() {
        let code = r#"
            // This is a comment.
            fn main() {}
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Comment(" This is a comment.".to_string()),
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![],
                },
            ]
        );
    }

    #[test]
    fn test_parse_variable_declarations() {
        let code = r#"
            fn main() {
                let a: i32 = 10;
                let mut b = 20;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![
                        ASTNode::VariableDeclaration {
                            is_mutable: false,
                            name: "a".to_string(),
                            type_annotation: Some(Type::I32),
                            value: Some(Box::new(ASTNode::IntegerLiteral(10))),
                        },
                        ASTNode::VariableDeclaration {
                            is_mutable: true,
                            name: "b".to_string(),
                            type_annotation: None,
                            value: Some(Box::new(ASTNode::IntegerLiteral(20))),
                        },
                    ],
                },
            ]
        );
    }

    #[test]
    fn test_parse_new_literal_types() {
        let code = r#"
            fn main() {
                let a: f64 = 3.14;
                let b: bool = true;
                let c: string = "hello";
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![
                        ASTNode::VariableDeclaration {
                            is_mutable: false,
                            name: "a".to_string(),
                            type_annotation: Some(Type::F64),
                            value: Some(Box::new(ASTNode::FloatLiteral(3.14))),
                        },
                        ASTNode::VariableDeclaration {
                            is_mutable: false,
                            name: "b".to_string(),
                            type_annotation: Some(Type::Bool),
                            value: Some(Box::new(ASTNode::BoolLiteral(true))),
                        },
                        ASTNode::VariableDeclaration {
                            is_mutable: false,
                            name: "c".to_string(),
                            type_annotation: Some(Type::String),
                            value: Some(Box::new(ASTNode::StringLiteral("hello".to_string()))),
                        },
                    ],
                },
            ]
        );
    }

    #[test]
    fn test_compile_variable_declaration_with_type_inference() {
        let code = r#"
            fn main() {
                let a = 10;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        let result = compiler.compile(&ast);

        assert!(result.is_ok());
    }

    #[test]
    fn test_parse_if_else_statement() {
        let code = r#"
            fn main() {
                if (true) {
                    let a = 10;
                } else {
                    let b = 20;
                }
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![
                        ASTNode::IfStatement {
                            condition: Box::new(ASTNode::BoolLiteral(true)),
                            then_block: vec![
                                ASTNode::VariableDeclaration {
                                    is_mutable: false,
                                    name: "a".to_string(),
                                    type_annotation: None,
                                    value: Some(Box::new(ASTNode::IntegerLiteral(10))),
                                },
                            ],
                            else_block: Some(vec![
                                ASTNode::VariableDeclaration {
                                    is_mutable: false,
                                    name: "b".to_string(),
                                    type_annotation: None,
                                    value: Some(Box::new(ASTNode::IntegerLiteral(20))),
                                },
                            ]),
                        },
                    ],
                },
            ]
        );
    }

    #[test]
    fn test_compile_if_else_statement() {
        let code = r#"
            fn main() {
                if (true) {
                    let a = 10;
                } else {
                    let b = 20;
                }
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        let result = compiler.compile(&ast);

        assert!(result.is_ok());
    }

    #[test]
    fn test_parse_while_statement() {
        let code = r#"
            fn main() {
                while (true) {
                    let a = 10;
                }
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![
                        ASTNode::WhileStatement {
                            condition: Box::new(ASTNode::BoolLiteral(true)),
                            body: vec![
                                ASTNode::VariableDeclaration {
                                    is_mutable: false,
                                    name: "a".to_string(),
                                    type_annotation: None,
                                    value: Some(Box::new(ASTNode::IntegerLiteral(10))),
                                },
                            ],
                        },
                    ],
                },
            ]
        );
    }

    #[test]
    fn test_compile_while_statement() {
        let code = r#"
            fn main() {
                while (true) {
                    let a = 10;
                }
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        let result = compiler.compile(&ast);

        assert!(result.is_ok());
    }

    #[test]
    fn test_parse_binary_expression() {
        let code = r#"
            fn main() {
                1 + 2 * 3;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![
                        ASTNode::BinaryExpression {
                            op: BinaryOperator::Add,
                            left: Box::new(ASTNode::IntegerLiteral(1)),
                            right: Box::new(ASTNode::BinaryExpression {
                                op: BinaryOperator::Multiply,
                                left: Box::new(ASTNode::IntegerLiteral(2)),
                                right: Box::new(ASTNode::IntegerLiteral(3)),
                            }),
                        },
                    ],
                },
            ]
        );
    }

    #[test]
    fn test_compile_binary_expression() {
        let code = r#"
            fn main() {
                let a = 10 + 20;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile(&ast).unwrap();

        insta::assert_snapshot!(compiler.to_string());
    }

    #[test]
    fn test_parse_assignment_expression() {
        let code = r#"
            fn main() {
                a = 10;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        assert_eq!(
            ast,
            vec![
                ASTNode::Function {
                    name: "main".to_string(),
                    args: vec![],
                    body: vec![
                        ASTNode::AssignmentExpression {
                            name: "a".to_string(),
                            value: Box::new(ASTNode::IntegerLiteral(10)),
                        },
                    ],
                },
            ]
        );
    }

    #[test]
    fn test_compile_assignment_expression() {
        let code = r#"
            fn main() {
                let mut a = 10;
                a = 20;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        compiler.compile(&ast).unwrap();

        insta::assert_snapshot!(compiler.to_string());
    }

    #[test]
    fn test_compile_assignment_to_immutable_variable() {
        let code = r#"
            fn main() {
                let a = 10;
                a = 20;
            }
        "#;
        let parser = Parser::new(code).unwrap();
        let ast = parser.parse().unwrap();

        let context = Context::create();
        let mut compiler = Compiler::new(&context);
        let result = compiler.compile(&ast);

        assert!(result.is_err());
    }
}
