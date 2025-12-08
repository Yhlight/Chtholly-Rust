#[derive(Debug, PartialEq, Clone)]
pub enum Type {
    I32,
    F64,
    Bool,
    String,
}

#[derive(Debug, PartialEq, Clone)]
pub enum BinaryOperator {
    Add,
    Subtract,
    Multiply,
    Divide,
    Equal,
    NotEqual,
    LessThan,
    GreaterThan,
    LessThanOrEqual,
    GreaterThanOrEqual,
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
    IfStatement {
        condition: Box<ASTNode>,
        then_block: Vec<ASTNode>,
        else_block: Option<Vec<ASTNode>>,
    },
    WhileStatement {
        condition: Box<ASTNode>,
        body: Vec<ASTNode>,
    },
    BinaryExpression {
        op: BinaryOperator,
        left: Box<ASTNode>,
        right: Box<ASTNode>,
    },
    Identifier(String),
    AssignmentExpression {
        name: String,
        value: Box<ASTNode>,
    },
    Comment(String),
}
