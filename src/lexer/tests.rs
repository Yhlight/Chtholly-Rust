use crate::lexer::lexer::Lexer;
use crate::lexer::token::Token;

#[test]
fn test_next_token_single_char() {
    let input = "=+(){},-*/;";
    let expected_tokens = vec![
        Token::Assign,
        Token::Plus,
        Token::LParen,
        Token::RParen,
        Token::LBrace,
        Token::RBrace,
        Token::Comma,
        Token::Minus,
        Token::Asterisk,
        Token::Slash,
        Token::Semicolon,
        Token::Eof,
    ];

    let mut lexer = Lexer::new(input.to_string());

    for expected_token in expected_tokens {
        let token = lexer.next_token();
        assert_eq!(token, expected_token);
    }
}

#[test]
fn test_identifiers_with_numbers() {
    let input = "let var1 = 5;";
    let expected_tokens = vec![
        Token::Let,
        Token::Identifier("var1".to_string()),
        Token::Assign,
        Token::Integer(5),
        Token::Semicolon,
        Token::Eof,
    ];

    let mut lexer = Lexer::new(input.to_string());

    for expected_token in expected_tokens {
        let token = lexer.next_token();
        assert_eq!(token, expected_token);
    }
}

#[test]
fn test_literals_and_comments() {
    let input = r#"
        "hello world";
        'a';
        // this is a comment
        let x = 5; /* another comment */
        "#;

    let expected_tokens = vec![
        Token::String("hello world".to_string()),
        Token::Semicolon,
        Token::Char('a'),
        Token::Semicolon,
        Token::Let,
        Token::Identifier("x".to_string()),
        Token::Assign,
        Token::Integer(5),
        Token::Semicolon,
        Token::Eof,
    ];

    let mut lexer = Lexer::new(input.to_string());

    for expected_token in expected_tokens {
        let token = lexer.next_token();
        assert_eq!(token, expected_token);
    }
}

#[test]
fn test_next_token_multi_char() {
    let input = r#"
        let five = 5;
        let ten = 10;
        let add = fn(x, y) {
            x + y;
        };
        let result = add(five, ten);
        ! - / * 5;
        5 < 10 > 5;
        if (5 < 10) {
            return true;
        } else {
            return false;
        }
        10 == 10;
        10 != 9;
        "#;

    let expected_tokens = vec![
        Token::Let,
        Token::Identifier("five".to_string()),
        Token::Assign,
        Token::Integer(5),
        Token::Semicolon,
        Token::Let,
        Token::Identifier("ten".to_string()),
        Token::Assign,
        Token::Integer(10),
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
        Token::Bang,
        Token::Minus,
        Token::Slash,
        Token::Asterisk,
        Token::Integer(5),
        Token::Semicolon,
        Token::Integer(5),
        Token::Lt,
        Token::Integer(10),
        Token::Gt,
        Token::Integer(5),
        Token::Semicolon,
        Token::If,
        Token::LParen,
        Token::Integer(5),
        Token::Lt,
        Token::Integer(10),
        Token::RParen,
        Token::LBrace,
        Token::Return,
        Token::True,
        Token::Semicolon,
        Token::RBrace,
        Token::Else,
        Token::LBrace,
        Token::Return,
        Token::False,
        Token::Semicolon,
        Token::RBrace,
        Token::Integer(10),
        Token::Eq,
        Token::Integer(10),
        Token::Semicolon,
        Token::Integer(10),
        Token::NotEq,
        Token::Integer(9),
        Token::Semicolon,
        Token::Eof,
    ];

    let mut lexer = Lexer::new(input.to_string());

    for expected_token in expected_tokens {
        let token = lexer.next_token();
        assert_eq!(token, expected_token);
    }
}
