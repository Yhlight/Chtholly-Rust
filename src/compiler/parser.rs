use chumsky::prelude::*;

use crate::compiler::ast::{BinaryOp, Expr, Literal, Stmt};
use crate::compiler::lexer::Token;

pub fn parser() -> impl Parser<Token, Vec<Stmt>, Error = Simple<Token>> {
    let ident = select! { Token::Ident(ident) => ident };

    let expr = recursive(|expr| {
        let literal = select! {
            Token::Int(s) => s.parse::<i64>().map(Literal::Int).map_err(|e| e.to_string()),
            Token::Float(s) => s.parse::<f64>().map(Literal::Float).map_err(|e| e.to_string()),
        }
        .try_map(|res, span| res.map_err(|e| Simple::custom(span, e)))
        .map(Expr::Literal)
        .or(select! {
            Token::String(s) => Expr::Literal(Literal::String(s)),
            Token::Char(c) => Expr::Literal(Literal::Char(c)),
            Token::True => Expr::Literal(Literal::Bool(true)),
            Token::False => Expr::Literal(Literal::Bool(false)),
        })
        .labelled("literal");

        let atom = literal
            .or(expr.delimited_by(just(Token::LParen), just(Token::RParen)))
            .or(ident.map(Expr::Ident))
            .labelled("atom");

        let op = |op, op_type| just(op).to(op_type);

        let product = atom
            .clone()
            .then(
                op(Token::Star, BinaryOp::Mul)
                    .or(op(Token::Slash, BinaryOp::Div))
                    .or(op(Token::Percent, BinaryOp::Mod))
                    .then(atom)
                    .repeated(),
            )
            .foldl(|lhs, (op, rhs)| Expr::Binary(Box::new(lhs), op, Box::new(rhs)))
            .labelled("product");

        let sum = product
            .clone()
            .then(
                op(Token::Plus, BinaryOp::Add)
                    .or(op(Token::Minus, BinaryOp::Sub))
                    .then(product)
                    .repeated(),
            )
            .foldl(|lhs, (op, rhs)| Expr::Binary(Box::new(lhs), op, Box::new(rhs)))
            .labelled("sum");

        let comparison = sum
            .clone()
            .then(
                op(Token::EqEq, BinaryOp::Eq)
                    .or(op(Token::NotEq, BinaryOp::NotEq))
                    .or(op(Token::Lt, BinaryOp::Lt))
                    .or(op(Token::LtEq, BinaryOp::LtEq))
                    .or(op(Token::Gt, BinaryOp::Gt))
                    .or(op(Token::GtEq, BinaryOp::GtEq))
                    .then(sum)
                    .repeated(),
            )
            .foldl(|lhs, (op, rhs)| Expr::Binary(Box::new(lhs), op, Box::new(rhs)))
            .labelled("comparison");

        comparison
    });

    let let_stmt = just(Token::Let)
        .ignore_then(ident)
        .then_ignore(just(Token::Eq))
        .then(expr.clone())
        .then_ignore(just(Token::Semicolon))
        .map(|(ident, expr)| Stmt::Let(ident, expr));

    let expr_stmt = expr
        .clone()
        .then_ignore(just(Token::Semicolon))
        .map(Stmt::Expr);

    let stmt = let_stmt.or(expr_stmt);

    stmt.repeated().then_ignore(end())
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler::lexer::lexer;
    use chumsky::Stream;

    fn parse_test_helper(input: &str, expected_stmts: Vec<Stmt>) {
        let (tokens, lex_errs) = lexer().parse_recovery(input);
        assert!(lex_errs.is_empty(), "Lexer errors: {:?}", lex_errs);

        let tokens_vec = tokens.unwrap();
        let (stmts, parse_errs) = parser().parse_recovery(Stream::from_iter(
            0..tokens_vec.len(),
            tokens_vec.into_iter(),
        ));

        assert!(
            parse_errs.is_empty(),
            "Parser errors: {:?}",
            parse_errs
        );

        assert_eq!(stmts.unwrap(), expected_stmts);
    }

    #[test]
    fn test_let_statement() {
        parse_test_helper(
            "let x = 5;",
            vec![Stmt::Let(
                "x".to_string(),
                Expr::Literal(Literal::Int(5)),
            )],
        );
    }
}
