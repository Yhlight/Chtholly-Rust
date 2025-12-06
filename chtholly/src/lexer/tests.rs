use crate::lexer::Lexer;
use crate::token::Token;

#[test]
fn test_next_token() {
    let input = r#"
        let five = 5;
        let ten = 10.0;
        let add = fn(x, y) {
            x + y;
        };
        let result = add(five, ten);
        "hello"
    "#
    .to_string();

    let mut lexer = Lexer::new(input);

    let tokens = vec![
        Token::Let,
        Token::Identifier("five".to_string()),
        Token::Assign,
        Token::Int(5),
        Token::Semicolon,
        Token::Let,
        Token::Identifier("ten".to_string()),
        Token::Assign,
        Token::Float(10.0),
        Token::Semicolon,
        Token::Let,
        Token::Identifier("add".to_string()),
        Token::Assign,
        Token::Fn,
        Token::LParen,
        Token::Identifier("x".to_string()),
        Token::Comma,
        Token::Identifier("y".to_string()),
        Token::RParen,
        Token::LBrace,
        Token::Identifier("x".to_string()),
        Token::Plus,
        Token::Identifier("y".to_string()),
        Token::Semicolon,
        Token::RBrace,
        Token::Semicolon,
        Token::Let,
        Token::Identifier("result".to_string()),
        Token::Assign,
        Token::Identifier("add".to_string()),
        Token::LParen,
        Token::Identifier("five".to_string()),
        Token::Comma,
        Token::Identifier("ten".to_string()),
        Token::RParen,
        Token::Semicolon,
        Token::String("hello".to_string()),
        Token::Eof,
    ];

    for token in tokens {
        assert_eq!(token, lexer.next_token());
    }
}

#[test]
fn test_identifiers_with_numbers() {
    let input = "let var1 = 5;".to_string();
    let mut lexer = Lexer::new(input);

    let tokens = vec![
        Token::Let,
        Token::Identifier("var1".to_string()),
        Token::Assign,
        Token::Int(5),
        Token::Semicolon,
        Token::Eof,
    ];

    for token in tokens {
        assert_eq!(token, lexer.next_token());
    }
}
