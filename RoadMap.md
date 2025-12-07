# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. It is a living document that will be updated as the project progresses.

## Phase 1: Core Language Infrastructure (Milestone 1)

- [ ] **Project Setup**
    - [x] Initialize Rust project with Cargo.
    - [x] Set up `.gitignore` file.
    - [ ] Configure testing and linting frameworks.
- [ ] **Lexer (Tokenizer)**
    - [ ] Implement tokenization for keywords (`fn`, `let`, `mut`, etc.).
    - [ ] Support for identifiers, literals (integers, strings, booleans), and operators.
    - [ ] Handle single-line and multi-line comments.
- [ ] **Parser (AST Generation)**
    - [ ] Implement a basic parser for the main function `fn main()`.
    - [ ] Parse variable declarations (`let`, `mut`).
    - [ ] Support for basic data types (`i32`, `f64`, `bool`, `string`).
- [ ] **Semantic Analysis (Initial)**
    - [ ] Implement basic type checking.
    - [ ] Symbol table for variable tracking.

## Phase 2: Control Flow and Functions (Milestone 2)

- [ ] **Control Flow Statements**
    - [ ] `if-else` expressions.
    - [ ] `switch-case` statements.
    - [ ] `while` and `for` loops.
- [ ] **Functions**
    - [ ] Function definitions with parameters and return types.
    - [ ] Function call expressions.
    - [ ] Support for `void` return type.

## Phase 3: Advanced Data Structures (Milestone 3)

- [ ] **Structs**
    - [ ] Definition and instantiation.
    - [ ] Member access.
    - [ ] Member functions (`&self`, `&mut self`).
- [ ] **Classes**
    - [ ] Class definitions with member variables and methods.
    - [ ] Constructors and destructors.
    - [ ] Public/private access modifiers.

## Phase 4: Memory Management and Generics (Milestone 4)

- [ ] **Ownership and Borrowing**
    - [ ] Implement move semantics.
    - [ ] Enforce borrowing rules (`&`, `&mut`).
- [ ] **Generics**
    - [ ] Generic functions.
    - [ ] Generic classes and structs.

## Phase 5: Error Handling and Modules (Milestone 5)

- [ ] **Error Handling**
    - [ ] `Result<T, E>` type.
    - [ ] `?` operator for error propagation.
- [ ] **Module System**
    - [ ] `import` statements for file-based modules.
    - [ ] Support for standard library modules.

## Phase 6: LLVM Backend Integration (Milestone 6)

- [ ] **Code Generation**
    - [ ] Set up LLVM IR builder.
    - [ ] Generate IR for basic expressions and statements.
    - [ ] Compile to object code and executable.
