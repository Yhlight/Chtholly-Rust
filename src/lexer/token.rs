/// Represents a single token in the Chtholly language.
#[derive(Debug, PartialEq, Clone)]
pub enum Token {
    // -- Special Tokens --
    /// An illegal or unknown token.
    Illegal,
    /// End of file marker.
    Eof,

    // -- Identifiers & Literals --
    /// An identifier, e.g., `my_variable`.
    Ident(String),
    /// An integer literal, e.g., `123`.
    Int(i64),
    /// A float literal, e.g., `12.34`.
    Float(f64),
    /// A string literal, e.g., `"hello"`.
    String(String),
    /// A character literal, e.g., `'a'`.
    Char(char),

    // -- Operators --
    Assign,      // =
    Plus,        // +
    Minus,       // -
    Bang,        // !
    Asterisk,    // *
    Slash,       // /
    Percent,     // %

    Lt,          // <
    Gt,          // >
    Eq,          // ==
    NotEq,       // !=
    LtEq,        // <=
    GtEq,        // >=

    PlusAssign,  // +=
    MinusAssign, // -=
    StarAssign,  // *=
    SlashAssign, // /=
    PercentAssign,// %=

    Increment,   // ++
    Decrement,   // --

    // -- Delimiters --
    Comma,       // ,
    Semicolon,   // ;
    Colon,       // :
    LParen,      // (
    RParen,      // )
    LBrace,      // {
    RBrace,      // }
    LBracket,    // [
    RBracket,    // ]

    // -- Keywords --
    Function,    // fn
    Let,         // let
    Mut,         // mut
    True,        // true
    False,       // false
    If,          // if
    Else,        // else
    Return,      // return
    While,       // while
    For,         // for
    Switch,      // switch
    Case,        // case
    Break,       // break
    Fallthrough, // fallthrough
    Class,       // class
    Struct,      // struct
    Enum,        // enum
    Public,      // public
    Private,     // private
    SelfKw,      // self
    Type,        // type
    Import,      // import
    Use,         // use
    As,          // as
    Request,     // request
    Require,     // require
}
