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

impl std::fmt::Display for Token {
    fn fmt(&self, f: &mut std::fmt::Formatter) -> std::fmt::Result {
        match self {
            Token::Illegal => write!(f, "Illegal"),
            Token::Eof => write!(f, "EOF"),
            Token::Ident(s) => write!(f, "{}", s),
            Token::Int(i) => write!(f, "{}", i),
            Token::Float(fl) => write!(f, "{}", fl),
            Token::String(s) => write!(f, "{}", s),
            Token::Char(c) => write!(f, "{}", c),
            Token::Assign => write!(f, "="),
            Token::Plus => write!(f, "+"),
            Token::Minus => write!(f, "-"),
            Token::Bang => write!(f, "!"),
            Token::Asterisk => write!(f, "*"),
            Token::Slash => write!(f, "/"),
            Token::Percent => write!(f, "%"),
            Token::Lt => write!(f, "<"),
            Token::Gt => write!(f, ">"),
            Token::Eq => write!(f, "=="),
            Token::NotEq => write!(f, "!="),
            Token::LtEq => write!(f, "<="),
            Token::GtEq => write!(f, ">="),
            Token::PlusAssign => write!(f, "+="),
            Token::MinusAssign => write!(f, "-="),
            Token::StarAssign => write!(f, "*="),
            Token::SlashAssign => write!(f, "/="),
            Token::PercentAssign => write!(f, "%="),
            Token::Increment => write!(f, "++"),
            Token::Decrement => write!(f, "--"),
            Token::Comma => write!(f, ","),
            Token::Semicolon => write!(f, ";"),
            Token::Colon => write!(f, ":"),
            Token::LParen => write!(f, "("),
            Token::RParen => write!(f, ")"),
            Token::LBrace => write!(f, "{{"),
            Token::RBrace => write!(f, "}}"),
            Token::LBracket => write!(f, "["),
            Token::RBracket => write!(f, "]"),
            Token::Function => write!(f, "fn"),
            Token::Let => write!(f, "let"),
            Token::Mut => write!(f, "mut"),
            Token::True => write!(f, "true"),
            Token::False => write!(f, "false"),
            Token::If => write!(f, "if"),
            Token::Else => write!(f, "else"),
            Token::Return => write!(f, "return"),
            Token::While => write!(f, "while"),
            Token::For => write!(f, "for"),
            Token::Switch => write!(f, "switch"),
            Token::Case => write!(f, "case"),
            Token::Break => write!(f, "break"),
            Token::Fallthrough => write!(f, "fallthrough"),
            Token::Class => write!(f, "class"),
            Token::Struct => write!(f, "struct"),
            Token::Enum => write!(f, "enum"),
            Token::Public => write!(f, "public"),
            Token::Private => write!(f, "private"),
            Token::SelfKw => write!(f, "self"),
            Token::Type => write!(f, "type"),
            Token::Import => write!(f, "import"),
            Token::Use => write!(f, "use"),
            Token::As => write!(f, "as"),
            Token::Request => write!(f, "request"),
            Token::Require => write!(f, "require"),
        }
    }
}
