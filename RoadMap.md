# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development will be done in phases, with each phase focusing on a specific set of features.

## Phase 1: Basic Compiler Infrastructure

*   [ ] Set up the basic project structure.
*   [ ] Implement the lexer to tokenize the source code.
*   [ ] Implement the parser to build the Abstract Syntax Tree (AST).
*   [ ] Implement the code generator to produce LLVM IR.
*   [ ] Implement basic data types: `i32`, `f64`, `char`, `bool`, `string`.
*   [ ] Implement basic operators: `+`, `-`, `*`, `/`, `%`, `==`, `!=`, `>`, `>=`, `<`, `<=`.
*   [ ] Implement basic control flow: `if-else`.
*   [ ] Implement basic functions.
*   [ ] Add unit tests for all implemented features.

## Phase 2: Advanced Language Features

*   [ ] Implement mutable and immutable variables (`let`, `mut`).
*   [ ] Implement ownership and borrowing (`&`, `&mut`).
*   [ ] Implement classes and structs.
*   [ ] Implement enums.
*   [ ] Implement arrays (static and dynamic).
*   [ ] Implement loops: `while`, `for`, `foreach`, `do-while`.
*   [ ] Implement `switch-case`.
*   [ ] Implement generics.
*   [ ] Implement modules and `import` statements.
*   [ ] Implement error handling with `Result<T, E>` and the `?` operator.

## Phase 3: Standard Library

*   [ ] Implement the `iostream` module for input/output.
*   [ ] Implement the `optional` module.
*   [ ] Implement the `iterator` module.
*   [ ] Implement other necessary standard library modules.

## Phase 4: Tooling and Ecosystem

*   [ ] Implement a package manager.
*   [ ] Implement a code formatter.
*   [ ] Implement a language server for IDE integration.
*   [ ] Write comprehensive documentation.
