use chumsky::prelude::*;

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
pub enum Token {
    // Keywords
    Fn,
    Let,
    Mut,
    If,
    Else,
    Switch,
    Case,
    While,
    For,
    Do,
    Class,
    Struct,
    Enum,
    Public,
    Private,
    Import,
    Use,
    As,
    Return,
    True,
    False,
    SelfKw,  // Self type
    SelfVal, // self value

    // Literals
    Int(String),
    Float(String),
    String(String),
    Char(char),

    // Identifiers
    Ident(String),

    // Operators
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    Eq,
    EqEq,
    Not,
    NotEq,
    Lt,
    LtEq,
    Gt,
    GtEq,
    And,
    AndAnd,
    Or,
    OrOr,
    Xor,
    Shl,
    Shr,
    PlusEq,
    MinusEq,
    StarEq,
    SlashEq,
    PercentEq,
    PlusPlus,
    MinusMinus,

    // Punctuation
    LParen,
    RParen,
    LBrace,
    RBrace,
    LBracket,
    RBracket,
    Comma,
    Colon,
    Semicolon,
    Dot,
}

pub fn lexer() -> impl Parser<char, Vec<(Token, <Simple<char> as chumsky::Error<char>>::Span)>, Error = Simple<char>> {
    let float = text::int(10)
        .then(just('.').ignore_then(text::digits(10)))
        .map(|(integer, fractional)| Token::Float(format!("{}.{}", integer, fractional)))
        .labelled("float");

    let int = text::int(10).map(Token::Int);

    let string = just('"')
        .ignore_then(none_of("\"").repeated())
        .then_ignore(just('"'))
        .collect::<String>()
        .map(Token::String)
        .labelled("string");

    let char_literal = just('\'')
        .ignore_then(none_of("'"))
        .then_ignore(just('\''))
        .map(Token::Char)
        .labelled("char");

    let ident = text::ident().map(|s: String| match s.as_str() {
        "fn" => Token::Fn,
        "let" => Token::Let,
        "mut" => Token::Mut,
        "if" => Token::If,
        "else" => Token::Else,
        "switch" => Token::Switch,
        "case" => Token::Case,
        "while" => Token::While,
        "for" => Token::For,
        "do" => Token::Do,
        "class" => Token::Class,
        "struct" => Token::Struct,
        "enum" => Token::Enum,
        "public" => Token::Public,
        "private" => Token::Private,
        "import" => Token::Import,
        "use" => Token::Use,
        "as" => Token::As,
        "return" => Token::Return,
        "true" => Token::True,
        "false" => Token::False,
        "Self" => Token::SelfKw,
        "self" => Token::SelfVal,
        _ => Token::Ident(s),
    });

    let token = float
        .or(int)
        .or(string)
        .or(char_literal)
        .or(just("++").to(Token::PlusPlus))
        .or(just("--").to(Token::MinusMinus))
        .or(just("+=").to(Token::PlusEq))
        .or(just("-=").to(Token::MinusEq))
        .or(just("*=").to(Token::StarEq))
        .or(just("/=").to(Token::SlashEq))
        .or(just("%=").to(Token::PercentEq))
        .or(just("==").to(Token::EqEq))
        .or(just("!=").to(Token::NotEq))
        .or(just("<=").to(Token::LtEq))
        .or(just(">=").to(Token::GtEq))
        .or(just("&&").to(Token::AndAnd))
        .or(just("||").to(Token::OrOr))
        .or(just("<<").to(Token::Shl))
        .or(just(">>").to(Token::Shr))
        .or(just('+').to(Token::Plus))
        .or(just('-').to(Token::Minus))
        .or(just('*').to(Token::Star))
        .or(just('/').to(Token::Slash))
        .or(just('%').to(Token::Percent))
        .or(just('=').to(Token::Eq))
        .or(just('!').to(Token::Not))
        .or(just('<').to(Token::Lt))
        .or(just('>').to(Token::Gt))
        .or(just('&').to(Token::And))
        .or(just('|').to(Token::Or))
        .or(just('^').to(Token::Xor))
        .or(just('(').to(Token::LParen))
        .or(just(')').to(Token::RParen))
        .or(just('{').to(Token::LBrace))
        .or(just('}').to(Token::RBrace))
        .or(just('[').to(Token::LBracket))
        .or(just(']').to(Token::RBracket))
        .or(just(',').to(Token::Comma))
        .or(just(':').to(Token::Colon))
        .or(just(';').to(Token::Semicolon))
        .or(just('.').to(Token::Dot))
        .or(ident);

    let comment = just("//").then(take_until(just('\n'))).padded();

    token
        .map_with_span(|tok, span| (tok, span))
        .padded_by(comment.repeated())
        .padded()
        .repeated()
        .then_ignore(end())
}

#[cfg(test)]
mod tests {
    use super::*;

    fn lex_test_helper(input: &str, expected_tokens: Vec<Token>) {
        let (tokens, errs) = lexer().parse_recovery(input);
        assert!(errs.is_empty(), "Lexer errors: {:?}", errs);
        let tokens: Vec<Token> = tokens.unwrap().into_iter().map(|(t, _)| t).collect();
        assert_eq!(tokens, expected_tokens);
    }

    #[test]
    fn test_keywords() {
        lex_test_helper("fn let mut", vec![Token::Fn, Token::Let, Token::Mut]);
    }

    #[test]
    fn test_literals() {
        lex_test_helper(
            "123 123.456 \"hello\" 'a'",
            vec![
                Token::Int("123".to_string()),
                Token::Float("123.456".to_string()),
                Token::String("hello".to_string()),
                Token::Char('a'),
            ],
        );
    }

    #[test]
    fn test_operators() {
        lex_test_helper(
            "+ - * / % = == != < <= > >= ++ --",
            vec![
                Token::Plus,
                Token::Minus,
                Token::Star,
                Token::Slash,
                Token::Percent,
                Token::Eq,
                Token::EqEq,
                Token::NotEq,
                Token::Lt,
                Token::LtEq,
                Token::Gt,
                Token::GtEq,
                Token::PlusPlus,
                Token::MinusMinus,
            ],
        );
    }

    #[test]
    fn test_let_statement() {
        lex_test_helper(
            "let x = 5;",
            vec![
                Token::Let,
                Token::Ident("x".to_string()),
                Token::Eq,
                Token::Int("5".to_string()),
                Token::Semicolon,
            ],
        );
    }

    #[test]
    fn test_comment() {
        lex_test_helper(
            "// this is a comment\nlet x = 5;",
            vec![
                Token::Let,
                Token::Ident("x".to_string()),
                Token::Eq,
                Token::Int("5".to_string()),
                Token::Semicolon,
            ],
        );
    }
}
