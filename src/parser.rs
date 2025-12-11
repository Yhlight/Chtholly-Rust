use crate::ast::{Expression, LiteralValue, Statement, Type};
use nom::{
    branch::alt,
    bytes::complete::{tag, take_until},
    character::complete::{alpha1, char, digit1, multispace0},
    combinator::{map, map_res, opt, recognize},
    sequence::{delimited, preceded, tuple},
    IResult,
};

// Parse single-line comments
fn parse_single_line_comment(input: &str) -> IResult<&str, &str> {
    let (input, _) = tag("//")(input)?;
    let (input, comment) = take_until("\n")(input)?;
    Ok((input, comment))
}

// Parse multi-line comments
fn parse_multi_line_comment(input: &str) -> IResult<&str, &str> {
    let (input, _) = tag("/*")(input)?;
    let (input, comment) = take_until("*/")(input)?;
    let (input, _) = tag("*/")(input)?;
    Ok((input, comment))
}

// Parse either a single-line or multi-line comment
pub fn parse_comment(input: &str) -> IResult<&str, &str> {
    alt((parse_single_line_comment, parse_multi_line_comment))(input)
}

// Parse the main function
pub fn parse_main_function(input: &str) -> IResult<&str, &str> {
    let (input, _) = tuple((
        multispace0,
        tag("fn"),
        multispace0,
        tag("main"),
        multispace0,
        char('('),
        multispace0,
        tag("args"),
        multispace0,
        char(':'),
        multispace0,
        tag("string[]"),
        multispace0,
        char(')'),
        multispace0,
        char(':'),
        multispace0,
        tag("Result<i32, SystemError>"),
        multispace0,
    ))(input)?;

    let (input, body) = delimited(
        tuple((char('{'), multispace0)),
        take_until("}"),
        tuple((multispace0, char('}'))),
    )(input)?;

    Ok((input, body))
}

/// Parses a primitive type into a `Type` enum.
pub fn parse_type(input: &str) -> IResult<&str, Type> {
    alt((
        map(tag("i32"), |_| Type::I32),
        map(tag("bool"), |_| Type::Bool),
    ))(input)
}

/// Parses a boolean literal (`true` or `false`).
fn parse_bool(input: &str) -> IResult<&str, LiteralValue> {
    alt((
        map(tag("true"), |_| LiteralValue::Boolean(true)),
        map(tag("false"), |_| LiteralValue::Boolean(false)),
    ))(input)
}

/// Parses an integer literal.
fn parse_integer(input: &str) -> IResult<&str, LiteralValue> {
    map(map_res(recognize(digit1), str::parse), |val| {
        LiteralValue::Integer(val)
    })(input)
}

/// Parses any literal value into an `Expression`.
pub fn parse_literal(input: &str) -> IResult<&str, Expression> {
    map(alt((parse_integer, parse_bool)), |literal| {
        Expression::Literal(literal)
    })(input)
}

/// Parses a `let` or `let mut` statement.
pub fn parse_let_statement(input: &str) -> IResult<&str, Statement> {
    let (input, _) = tag("let")(input)?;
    let (input, _) = multispace0(input)?;
    let (input, is_mutable) = map(opt(tag("mut")), |m| m.is_some())(input)?;
    let (input, _) = multispace0(input)?;
    let (input, name) = alpha1(input)?;
    let (input, _) = multispace0(input)?;
    let (input, type_annotation) = opt(preceded(
        tuple((char(':'), multispace0)),
        parse_type,
    ))(input)?;
    let (input, _) = multispace0(input)?;
    let (input, _) = char('=')(input)?;
    let (input, _) = multispace0(input)?;
    let (input, value) = parse_literal(input)?;
    let (input, _) = char(';')(input)?;

    Ok((
        input,
        Statement::Let {
            name: name.to_string(),
            is_mutable,
            type_annotation,
            value,
        },
    ))
}
