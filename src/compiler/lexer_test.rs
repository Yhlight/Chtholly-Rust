use super::lexer::{Lexer, Token};

#[test]
fn test_next_token() {
    let input = "let five = 5;";
    let mut lexer = Lexer::new(input);

    let tokens = vec![
        Token::Let,
        Token::Identifier("five".to_string()),
        Token::Equal,
        Token::Integer(5),
        Token::Semicolon,
        Token::Eof,
    ];

    for token in tokens {
        assert_eq!(lexer.next_token(), token);
    }
}

#[test]
fn test_operators() {
    let input = "+ - * / % == != < > <= >= && || ! & | ^ ~ << >> += -= *= /= %= ++ --";
    let mut lexer = Lexer::new(input);

    let tokens = vec![
        Token::Plus,
        Token::Minus,
        Token::Asterisk,
        Token::Slash,
        Token::Percent,
        Token::EqualEqual,
        Token::NotEqual,
        Token::LessThan,
        Token::GreaterThan,
        Token::LessThanOrEqual,
        Token::GreaterThanOrEqual,
        Token::AmpersandAmpersand,
        Token::PipePipe,
        Token::Bang,
        Token::Ampersand,
        Token::Pipe,
        Token::Caret,
        Token::Tilde,
        Token::LeftShift,
        Token::RightShift,
        Token::PlusEqual,
        Token::MinusEqual,
        Token::AsteriskEqual,
        Token::SlashEqual,
        Token::PercentEqual,
        Token::PlusPlus,
        Token::MinusMinus,
        Token::Eof,
    ];

    for token in tokens {
        assert_eq!(lexer.next_token(), token);
    }
}
