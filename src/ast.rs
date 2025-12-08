#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    I32,
    F64,
    Bool,
    String,
}

#[derive(Debug, PartialEq, Clone)]
pub enum ASTNode {
    Function {
        name: String,
        args: Vec<(String, Type)>,
        body: Vec<ASTNode>,
    },
    VariableDeclaration {
        is_mutable: bool,
        name: String,
        type_annotation: Option<Type>,
        value: Option<Box<ASTNode>>,
    },
    IntegerLiteral(i64),
    FloatLiteral(f64),
    BoolLiteral(bool),
    StringLiteral(String),
    Comment(String),
}
