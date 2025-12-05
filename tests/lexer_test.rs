use chtholly::lexer::token::Token;
use chtholly::lexer::Lexer;

#[test]
fn test_next_token() {
    let input = "let five = 5;";
    let tests = vec![
        Token::Let,
        Token::Identifier("five".to_string()),
        Token::Assign,
        Token::Int(5),
        Token::Semicolon,
        Token::Eof,
    ];

    let mut lexer = Lexer::new(input);
    for test in tests {
        let tok = lexer.next_token();
        assert_eq!(tok, test);
    }
}
