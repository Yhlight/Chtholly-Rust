use chumsky::prelude::*;

use crate::compiler::ast::{BinaryOp, Expr, Literal};
use crate::compiler::lexer::Token;

pub fn parser() -> impl Parser<Token, Expr, Error = Simple<Token>> {
    let expr = recursive(|expr| {
        let literal = select! {
            Token::Int(s) => Expr::Literal(Literal::Int(s.parse().unwrap())),
            Token::Float(s) => Expr::Literal(Literal::Float(s.parse().unwrap())),
            Token::String(s) => Expr::Literal(Literal::String(s)),
            Token::Char(c) => Expr::Literal(Literal::Char(c)),
            Token::True => Expr::Literal(Literal::Bool(true)),
            Token::False => Expr::Literal(Literal::Bool(false)),
        }
        .labelled("literal");

        let atom = literal
            .or(expr.delimited_by(just(Token::LParen), just(Token::RParen)))
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

    expr.then_ignore(end())
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::compiler::lexer::lexer;
    use chumsky::Stream;

    fn parse_test_helper(input: &str, expected_expr: Expr) {
        let (tokens, lex_errs) = lexer().parse_recovery(input);
        assert!(lex_errs.is_empty(), "Lexer errors: {:?}", lex_errs);

        let tokens_vec = tokens.unwrap();
        let (expr, parse_errs) = parser().parse_recovery(Stream::from_iter(
            tokens_vec.len()..tokens_vec.len(),
            tokens_vec.into_iter(),
        ));

        assert!(
            parse_errs.is_empty(),
            "Parser errors: {:?}",
            parse_errs
        );

        assert_eq!(expr.unwrap(), expected_expr);
    }

    #[test]
    fn test_literals() {
        parse_test_helper("123", Expr::Literal(Literal::Int(123)));
        parse_test_helper("123.456", Expr::Literal(Literal::Float(123.456)));
        parse_test_helper(
            "\"hello\"",
            Expr::Literal(Literal::String("hello".to_string())),
        );
        parse_test_helper("'a'", Expr::Literal(Literal::Char('a')));
        parse_test_helper("true", Expr::Literal(Literal::Bool(true)));
        parse_test_helper("false", Expr::Literal(Literal::Bool(false)));
    }

    #[test]
    fn test_arithmetic() {
        parse_test_helper(
            "1 + 2 * 3",
            Expr::Binary(
                Box::new(Expr::Literal(Literal::Int(1))),
                BinaryOp::Add,
                Box::new(Expr::Binary(
                    Box::new(Expr::Literal(Literal::Int(2))),
                    BinaryOp::Mul,
                    Box::new(Expr::Literal(Literal::Int(3))),
                )),
            ),
        );
    }
}
