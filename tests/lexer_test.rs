use chtholly::lexer::token::Token;
use chtholly::lexer::Lexer;

#[test]
fn test_next_token() {
    let input = "let five = 5; let ten = 10; let add = fn(x, y) { x + y; }; let result = add(five, ten); 'a';";
    let tests = vec![
        Token::Let,
        Token::Identifier("five".to_string()),
        Token::Assign,
        Token::Int(5),
        Token::Semicolon,
        Token::Let,
        Token::Identifier("ten".to_string()),
        Token::Assign,
        Token::Int(10),
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
        Token::Char('a'),
        Token::Semicolon,
        Token::Eof,
    ];

    let mut lexer = Lexer::new(input);
    for test in tests {
        let tok = lexer.next_token();
        assert_eq!(tok, test);
    }
}
