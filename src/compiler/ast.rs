#[derive(Debug, PartialEq, Clone)]
pub enum Stmt {
    Let(String, bool, Expr), // name, is_mutable, value
    Expr(Expr),
    If(Box<Expr>, Vec<Stmt>, Option<Vec<Stmt>>), // condition, then_block, else_block
    While(Box<Expr>, Vec<Stmt>), // condition, body
    For(Box<Stmt>, Box<Expr>, Box<Expr>, Vec<Stmt>), // init, cond, incr, body
}

#[derive(Debug, PartialEq, Clone)]
pub enum Expr {
    Literal(Literal),
    Ident(String),
    Assignment(String, Box<Expr>),
    Binary(Box<Expr>, BinaryOp, Box<Expr>),
}

#[derive(Debug, PartialEq, Clone)]
pub enum Literal {
    Int(i64),
    Float(f64),
    String(String),
    Char(char),
    Bool(bool),
}

#[derive(Debug, PartialEq, Clone, Copy)]
pub enum BinaryOp {
    // Arithmetic
    Add,
    Sub,
    Mul,
    Div,
    Mod,

    // Comparison
    Eq,
    NotEq,
    Lt,
    LtEq,
    Gt,
    GtEq,

    // Logical
    And,
    Or,
}
