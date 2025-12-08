#[derive(Debug, PartialEq, Clone)]
pub enum ASTNode {
    Function {
        name: String,
        args: Vec<(String, String)>,
        body: Vec<ASTNode>,
    },
    Comment(String),
}
