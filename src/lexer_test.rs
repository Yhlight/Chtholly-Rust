use crate::lexer::{Lexer, Token};

#[test]
fn test_next_token() {
    let input = r#"
        fn main() {
            let mut x = 5;
            let y = 10.5;
            if x > y {
                return 1;
            } else {
                return 0;
            }
        }
    "#;

    let mut lexer = Lexer::new(input);

    let tokens = vec![
        Token::Fn,
        Token::Identifier("main".to_string()),
        Token::LParen,
        Token::RParen,
        Token::LBrace,
        Token::Let,
        Token::Mut,
        Token::Identifier("x".to_string()),
        Token::Equal,
        Token::Integer(5),
        Token::Semicolon,
        Token::Let,
        Token::Identifier("y".to_string()),
        Token::Equal,
        Token::Float(10.5),
        Token::Semicolon,
        Token::If,
        Token::Identifier("x".to_string()),
        Token::GreaterThan,
        Token::Identifier("y".to_string()),
        Token::LBrace,
        Token::Return,
        Token::Integer(1),
        Token::Semicolon,
        Token::RBrace,
        Token::Else,
        Token::LBrace,
        Token::Return,
        Token::Integer(0),
        Token::Semicolon,
        Token::RBrace,
        Token::RBrace,
        Token::Eof,
    ];

    for expected_token in tokens {
        let token = lexer.next_token();
        assert_eq!(token, expected_token);
    }
}
