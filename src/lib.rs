pub mod ast;
pub mod lexer;
pub mod parser;

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
}
