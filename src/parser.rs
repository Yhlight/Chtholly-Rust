use nom::{
    branch::alt,
    bytes::complete::{tag, take_until},
    character::complete::{char, multispace0},
    sequence::{delimited, tuple},
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
