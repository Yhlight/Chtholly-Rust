
use chtholly::lexer::Lexer;

#[test]
fn test_comments() {
    let input = r#"
        // This is a single-line comment.
        /*
            This is a multi-line comment.
        */
    "#;
    let mut lexer = Lexer::new(input);
    let tokens = lexer.lex();
    assert_eq!(tokens.len(), 1);
    assert_eq!(tokens[0].kind, chtholly::token::TokenKind::Eof);
}
