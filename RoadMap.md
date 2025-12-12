# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The goal is to incrementally build the language, ensuring stability and avoiding regressions through a test-driven development (TDD) approach.

## Phase 1: Core Language Features

### Milestone 1.1: Basic Syntax and Scaffolding
- [x] Set up the development environment.
- [ ] Initialize the Cargo project.
- [ ] Implement the command-line interface (CLI) for file processing.
- [ ] Implement a basic lexer for tokenizing the source code.
- [ ] Implement a basic parser to construct an Abstract Syntax Tree (AST).
- [ ] Define the core AST data structures.

### Milestone 1.2: Variables and Data Types
- [ ] Implement `let` and `let mut` for variable declarations.
- [ ] Support basic data types: `i32`, `f64`, `bool`, `char`.
- [ ] Implement type inference and type annotations.
- [ ] Implement move semantics for variable assignments.
- [ ] Write unit tests for variable declarations and assignments.

### Milestone 1.3: Functions and Control Flow
- [ ] Implement function definitions with `fn`.
- [ ] Implement `if-else` expressions.
- [ ] Implement `while` and `for` loops.
- [ ] Implement `do-while` loops.
- [ ] Implement `switch-case` statements.
- [ ] Write unit tests for control flow structures.

### Milestone 1.4: Ownership and Borrowing
- [ ] Implement the ownership system.
- [ ] Implement borrowing with `&` and `&mut`.
- [ ] Implement lifetime analysis.
- [ ] Write unit tests for ownership and borrowing rules.

## Phase 2: Advanced Language Features

### Milestone 2.1: Structs and Classes
- [ ] Implement `struct` definitions.
- [ ] Implement `class` definitions.
- [ ] Implement member variables and methods.
- [ ] Implement constructors and destructors.
- [ ] Implement access modifiers (`public`, `private`).
- [ ] Write unit tests for structs and classes.

### Milestone 2.2: Generics and Enums
- [ ] Implement generic functions.
- [ ] Implement generic structs and classes.
- [ ] Implement `enum` definitions.
- [ ] Implement pattern matching for enums.
- [ ] Write unit tests for generics and enums.

### Milestone 2.3: Modules and Error Handling
- [ ] Implement the module system with `import` and `use`.
- [ ] Implement the `Result<T, E>` type for error handling.
- [ ] Implement the `?` operator for error propagation.
- [ ] Write unit tests for modules and error handling.

## Phase 3: Standard Library and Tooling

### Milestone 3.1: Standard Library
- [ ] Implement a basic I/O library (`iostream`).
- [ ] Implement a string manipulation library.
- [ ] Implement a collection library (e.g., `optional`).

### Milestone 3.2: Tooling and Ecosystem
- [ ] Develop a package manager.
- [ ] Create a code formatter.
- [ ] Write comprehensive documentation.

This roadmap will be updated as the project progresses. Each feature will be implemented with corresponding tests to ensure the stability and correctness of the language.
