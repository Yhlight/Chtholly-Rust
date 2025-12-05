//! Parser for the Chtholly language.

use crate::ast::{self, BinaryOp, Expression, Function, If, LetDeclaration, Literal, Parameter, Statement, Type};
use chumsky::prelude::*;

fn type_parser() -> impl Parser<char, Type, Error = Simple<char>> + Clone {
    let ident = text::ident().padded();

    recursive(|type_parser| {
        let generic_args = type_parser
            .clone()
            .separated_by(just(',').padded())
            .allow_trailing()
            .delimited_by(just('<'), just('>'));

        let base_type = ident
            .then(generic_args.or_not())
            .map(|(name, args)| match args {
                Some(args) => Type::Generic(name, args),
                None => Type::Simple(name),
            });

        base_type
            .then(just("[]").repeated())
            .foldl(|ty, _brackets| Type::Array(Box::new(ty)))
    })
}

/// Parses the Chtholly source code.
pub fn parse(source: &str) -> Result<Vec<ast::Node>, Vec<Simple<char>>> {
    let mut expr = Recursive::declare();
    let mut stmt = Recursive::declare();

    let block = stmt
        .clone()
        .repeated()
        .delimited_by(just('{').padded(), just('}').padded())
        .map(Expression::Block);

    expr.define({
        let integer = text::int(10).map(|s: String| Literal::Int(s.parse().unwrap()));
        let float = text::int(10)
            .then_ignore(just('.'))
            .then(text::digits(10))
            .map(|(a, b): (String, String)| Literal::Float(format!("{}.{}", a, b).parse().unwrap()));
        let char_literal = none_of('\'')
            .delimited_by(just('\''), just('\''))
            .map(Literal::Char);
        let bool_literal = just("true")
            .to(Literal::Bool(true))
            .or(just("false").to(Literal::Bool(false)));
        let string_literal = none_of('"')
            .repeated()
            .delimited_by(just('"'), just('"'))
            .collect()
            .map(Literal::String);

        let literal = choice((
            float,
            integer,
            char_literal,
            bool_literal,
            string_literal,
        ))
        .map(Expression::Literal);

        let if_expr = just("if")
            .padded()
            .ignore_then(expr.clone().delimited_by(just('('), just(')')))
            .then(block.clone())
            .then(just("else").padded().ignore_then(block).or_not())
            .map(|((condition, then_block), else_block)| {
                Expression::If(If {
                    condition: Box::new(condition),
                    then_block: Box::new(then_block),
                    else_block: else_block.map(Box::new),
                })
            });

        let atom = literal
            .or(if_expr)
            .or(expr.clone().delimited_by(just('('), just(')')))
            .padded();

        let mul_div = just('*').to(BinaryOp::Mul).or(just('/').to(BinaryOp::Div)).padded();
        let add_sub = just('+').to(BinaryOp::Add).or(just('-').to(BinaryOp::Sub)).padded();
        let cmp = just("==")
            .to(BinaryOp::Eq)
            .or(just("!=").to(BinaryOp::NotEq))
            .or(just(">=").to(BinaryOp::GtEq))
            .or(just("<=").to(BinaryOp::LtEq))
            .or(just('>').to(BinaryOp::Gt))
            .or(just('<').to(BinaryOp::Lt))
            .padded();
        let logical_and = just("&&").to(BinaryOp::And).padded();
        let logical_or = just("||").to(BinaryOp::Or).padded();

        let product = atom
            .clone()
            .then(mul_div.then(atom).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        let sum = product
            .clone()
            .then(add_sub.then(product).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        let comparison = sum
            .clone()
            .then(cmp.then(sum).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        let and = comparison
            .clone()
            .then(logical_and.then(comparison).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        let or = and
            .clone()
            .then(logical_or.then(and).repeated())
            .foldl(|lhs, (op, rhs)| Expression::Binary(Box::new(lhs), op, Box::new(rhs)));

        or
    });

    stmt.define({
        let let_declaration = just("let")
            .to(false)
            .or(just("mut").to(true))
            .padded()
            .then(text::ident().padded())
            .then(just(':').padded().ignore_then(type_parser()).or_not())
            .then_ignore(just('=').padded())
            .then(expr.clone())
            .then_ignore(just(';'))
            .map(|(((mutable, name), type_info), value)| {
                Statement::Let(LetDeclaration {
                    name,
                    mutable,
                    type_info,
                    value,
                })
            });

        let expression_statement = expr.clone().then_ignore(just(';')).map(Statement::Expression);

        let_declaration.or(expression_statement)
    });

    let function_parser = {
        let ident = text::ident().padded();

        let param = ident
            .then_ignore(just(':').padded())
            .then(type_parser())
            .map(|(name, type_info)| Parameter { name, type_info });

        let params = param
            .separated_by(just(',').padded())
            .allow_trailing()
            .delimited_by(just('('), just(')'));

        let body = stmt
            .repeated()
            .delimited_by(just('{').padded(), just('}').padded());

        just("fn")
            .padded()
            .ignore_then(ident)
            .then(params)
            .then_ignore(just(':').padded())
            .then(type_parser())
            .then(body)
            .map(|(((name, parameters), return_type), body)| {
                ast::Node::Function(Function {
                    name,
                    parameters,
                    return_type,
                    body,
                })
            })
    };

    let single_line_comment = just("//").then(take_until(just('\n'))).padded().ignored();
    let multi_line_comment = just("/*").then(take_until(just("*/"))).padded().ignored();
    let comment = single_line_comment.or(multi_line_comment);

    let top_level = function_parser.map(Some).or(comment.to(None));

    let parser = top_level
        .repeated()
        .map(|nodes| nodes.into_iter().flatten().collect())
        .then_ignore(end());

    parser.parse(source)
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::ast::{Expression, Function, If, LetDeclaration, Literal, Node, Statement, Type};

    #[test]
    fn test_parse_if_else_statement() {
        let source = r#"
            fn main(): void {
                if (true) {
                    let a = 1;
                };
            }
        "#;
        let expected_ast = vec![Node::Function(Function {
            name: "main".to_string(),
            parameters: vec![],
            return_type: Type::Simple("void".to_string()),
            body: vec![Statement::Expression(Expression::If(If {
                condition: Box::new(Expression::Literal(Literal::Bool(true))),
                then_block: Box::new(Expression::Block(vec![Statement::Let(LetDeclaration {
                    name: "a".to_string(),
                    mutable: false,
                    type_info: None,
                    value: Expression::Literal(Literal::Int(1)),
                })])),
                else_block: None,
            }))],
        })];
        assert_eq!(parse(source).unwrap(), expected_ast);
    }
}
