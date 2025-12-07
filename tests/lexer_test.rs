use chtholly::lexer::Lexer;
use chtholly::token::Token;

#[test]
fn test_lexer() {
    let input = r#"
        // this is a single line comment
        /* this is a multi-line comment */
        let x = 5; // another comment
        /*
         * another multi-line comment
         */
        let var1 = 10;
    "#;

    let mut lexer = Lexer::new(input);

    let expected_tokens = vec![
        Token::Let,
        Token::Ident("x".to_string()),
        Token::Assign,
        Token::Int(5),
        Token::Semicolon,
        Token::Let,
        Token::Ident("var1".to_string()),
        Token::Assign,
        Token::Int(10),
        Token::Semicolon,
        Token::Eof,
    ];

    for expected_token in expected_tokens {
        let token = lexer.next_token();
        assert_eq!(token, expected_token);
    }
}
