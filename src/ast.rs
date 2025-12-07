//! Abstract Syntax Tree (AST) for the Chtholly language.

/// Represents a Chtholly program.
#[derive(Debug, PartialEq)]
pub struct Program {
    pub body: Vec<Statement>,
}

/// Represents a statement in a Chtholly program.
#[derive(Debug, PartialEq)]
pub enum Statement {
    MainFunction(MainFunction),
}

/// Represents the main function.
#[derive(Debug, PartialEq)]
pub struct MainFunction;
