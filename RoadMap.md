# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. It is a living document and will be updated as the project progresses.

## Milestone 1: Project Setup and Basic Syntax

- [ ] Initialize Rust project with Cargo.
- [ ] Configure `inkwell` dependency for LLVM integration.
- [ ] Create `.gitignore` to exclude build artifacts.
- [ ] Implement the parser for basic syntax elements:
    - [ ] Comments (single-line and multi-line).
    - [ ] `main` function definition.
- [ ] Implement basic AST (Abstract Syntax Tree) nodes for the parsed elements.
- [ ] Implement a basic code generator that can produce an empty LLVM module for a `main` function.
- [ ] Write unit tests for the parser.

## Milestone 2: Variables and Data Types

- [ ] Implement parsing for `let` and `mut` variable declarations.
- [ ] Implement parsing for type annotations.
- [ ] Support all built-in data types (`i32`, `f64`, `char`, `string`, etc.).
- [ ] Implement AST nodes for variable declarations and data types.
- [ ] Implement code generation for variable allocation and initialization.
- [ ] Write unit tests for variable declarations and data types.

## Milestone 3: Operators and Expressions

- [ ] Implement parsing for all arithmetic, comparison, logical, and bitwise operators.
- [ ] Implement operator precedence and associativity rules.
- [ ] Implement AST nodes for expressions.
- [ ] Implement code generation for expressions.
- [ ] Write unit tests for all operators and various expression combinations.

## Milestone 4: Control Flow

- [ ] Implement parsing for `if-else` statements.
- [ ] Implement parsing for `switch-case` statements.
- [ ] Implement parsing for `while`, `for`, `foreach`, and `do-while` loops.
- [ ] Implement AST nodes for control flow statements.
- [ ] Implement code generation for control flow, including branching and loops.
- [ ] Write unit tests for all control flow structures.

## Milestone 5: Functions

- [ ] Implement parsing for function definitions and calls.
- [ ] Implement parsing for function parameters and return types.
- [ ] Implement parsing for lambda expressions.
- [ ] Implement AST nodes for functions and lambdas.
- [ ] Implement code generation for function definitions and calls.
- [ ] Write unit tests for functions and lambdas.

## Milestone 6: Structs and Classes

- [ ] Implement parsing for `struct` definitions.
- [ ] Implement parsing for `class` definitions, including member variables, constructors, and destructors.
- [ ] Implement parsing for member functions and the `self` keyword.
- [ ] Implement parsing for `public` and `private` access modifiers.
- [ ] Implement AST nodes for structs and classes.
- [ ] Implement code generation for structs and classes.
- [ ] Write unit tests for structs and classes.

## Milestone 7: Advanced Features

- [ ] Implement parsing for enums.
- [ ] Implement parsing for arrays (static and dynamic).
- [ ] Implement parsing for generics in functions and classes.
- [ ] Implement parsing for `request` (traits/interfaces).
- [ ] Implement parsing for `optional` type.
- [ ] Implement parsing for modules (`import` and `package`).
- [ ] Implement parsing for error handling (`Result<T, E>` and `?` operator).
- [ ] Implement AST nodes for all advanced features.
- [ ] Implement code generation for all advanced features.
- [ ] Write extensive unit and integration tests for all advanced features.
