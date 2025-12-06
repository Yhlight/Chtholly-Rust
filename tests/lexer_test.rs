use chtholly::lexer::Lexer;
use chtholly::token::TokenKind;

#[test]
fn test_next_token() {
    let input = "let five = 5;
let ten = 10;

let add = fn(x, y) {
  x + y;
};

let result = add(five, ten);
!*-/5;
5 < 10 > 5;

if (5 < 10) {
    return true;
} else {
    return false;
}

10 == 10;
10 != 9;
let var1 = 5;
";

    let tests = vec![
        (TokenKind::Let, "let"),
        (TokenKind::Ident, "five"),
        (TokenKind::Assign, "="),
        (TokenKind::Int, "5"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Let, "let"),
        (TokenKind::Ident, "ten"),
        (TokenKind::Assign, "="),
        (TokenKind::Int, "10"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Let, "let"),
        (TokenKind::Ident, "add"),
        (TokenKind::Assign, "="),
        (TokenKind::Function, "fn"),
        (TokenKind::LParen, "("),
        (TokenKind::Ident, "x"),
        (TokenKind::Comma, ","),
        (TokenKind::Ident, "y"),
        (TokenKind::RParen, ")"),
        (TokenKind::LBrace, "{"),
        (TokenKind::Ident, "x"),
        (TokenKind::Plus, "+"),
        (TokenKind::Ident, "y"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::RBrace, "}"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Let, "let"),
        (TokenKind::Ident, "result"),
        (TokenKind::Assign, "="),
        (TokenKind::Ident, "add"),
        (TokenKind::LParen, "("),
        (TokenKind::Ident, "five"),
        (TokenKind::Comma, ","),
        (TokenKind::Ident, "ten"),
        (TokenKind::RParen, ")"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Bang, "!"),
        (TokenKind::Asterisk, "*"),
        (TokenKind::Minus, "-"),
        (TokenKind::Slash, "/"),
        (TokenKind::Int, "5"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Int, "5"),
        (TokenKind::Lt, "<"),
        (TokenKind::Int, "10"),
        (TokenKind::Gt, ">"),
        (TokenKind::Int, "5"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::If, "if"),
        (TokenKind::LParen, "("),
        (TokenKind::Int, "5"),
        (TokenKind::Lt, "<"),
        (TokenKind::Int, "10"),
        (TokenKind::RParen, ")"),
        (TokenKind::LBrace, "{"),
        (TokenKind::Ident, "return"), // return is not a keyword yet
        (TokenKind::True, "true"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::RBrace, "}"),
        (TokenKind::Else, "else"),
        (TokenKind::LBrace, "{"),
        (TokenKind::Ident, "return"),
        (TokenKind::False, "false"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::RBrace, "}"),
        (TokenKind::Int, "10"),
        (TokenKind::Eq, "=="),
        (TokenKind::Int, "10"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Int, "10"),
        (TokenKind::NotEq, "!="),
        (TokenKind::Int, "9"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Let, "let"),
        (TokenKind::Ident, "var1"),
        (TokenKind::Assign, "="),
        (TokenKind::Int, "5"),
        (TokenKind::Semicolon, ";"),
        (TokenKind::Eof, ""),
    ];

    let mut lexer = Lexer::new(input);

    for (expected_kind, expected_literal) in tests {
        let tok = lexer.next_token();
        assert_eq!(tok.kind, expected_kind);
        assert_eq!(tok.literal, expected_literal);
    }
}
