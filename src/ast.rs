#[derive(Debug, PartialEq, Clone)]
pub enum ASTNode {
    Function {
        name: String,
        args: Vec<(String, String)>,
        body: Vec<ASTNode>,
    },
    VariableDeclaration {
        is_mutable: bool,
        name: String,
        type_annotation: Option<String>,
        value: Option<Box<ASTNode>>,
    },
    IntegerLiteral(i64),
    Comment(String),
}
