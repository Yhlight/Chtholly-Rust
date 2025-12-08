pub mod ast;
pub mod lexer;
pub mod parser;
pub mod compiler;

#[cfg(test)]
mod tests {
    use crate::parser::Parser;
    use crate::ast::ASTNode;

    #[test]
    fn test_parse_comment_and_function() {
        let code = r#"
            // This is a comment.
            fn main() {}
        "#;
        let parser = Parser::new(code);
        let ast = parser.parse();

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
        let parser = Parser::new(code);
        let ast = parser.parse();

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
                            type_annotation: Some("i32".to_string()),
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
}
