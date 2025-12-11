#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // Keywords
    Fn,      // fn
    Let,     // let
    Mut,     // mut
    If,      // if
    Else,    // else
    Switch,  // switch
    Case,    // case
    While,   // while
    For,     // for
    Do,      // do
    Return,  // return
    Class,   // class
    Struct,  // struct
    Enum,    // enum
    Public,  // public
    Private, // private
    Const,   // const

    // Identifiers and Literals
    Identifier(String),
    Integer(i64),
    Float(f64),
    String(String),
    Char(char),

    // Operators
    Plus,          // +
    Minus,         // -
    Asterisk,      // *
    Slash,         // /
    Percent,       // %
    Equal,         // =
    EqualEqual,    // ==
    Bang,          // !
    BangEqual,     // !=
    LessThan,      // <
    LessThanEqual, // <=
    GreaterThan,   // >
    GreaterThanEqual, // >=
    And,           // &
    AndAnd,        // &&
    Or,            // |
    OrOr,          // ||
    Caret,         // ^
    Tilde,         // ~
    LeftShift,     // <<
    RightShift,    // >>

    // Punctuation
    LParen,   // (
    RParen,   // )
    LBrace,   // {
    RBrace,   // }
    LBracket, // [
    RBracket, // ]
    Comma,    // ,
    Colon,    // :
    Semicolon,// ;
    Dot,      // .

    // End of File
    Eof,

    // Illegal token
    Illegal,
}
