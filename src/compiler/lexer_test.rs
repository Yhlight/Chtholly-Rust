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
