# Chtholly Compiler Development Roadmap

This document outlines the development plan for the Chtholly compiler.

## Milestone 1: Core Compiler Infrastructure

- [x] **Project Setup:** Initialize the Rust project using Cargo.
- [x] **Roadmap:** Create this `RoadMap.md` file.
- [x] **Lexer (Tokenizer):** Implement a lexer to convert source code into a stream of tokens.
- [x] **AST (Abstract Syntax Tree):** Define the data structures for the AST.
- [x] **Parser:** Implement a parser to build the AST from the token stream.

## Milestone 2: Basic Language Features

- [x] **Variable Declarations:** Support for `let` and `mut`.
- [ ] **Data Types:** Implement basic data types like `int`, `double`, `char`, `string`, `bool` (In Progress).
- [x] **Operators:** Implement arithmetic, comparison, and logical operators.
- [x] **Control Flow:** Implement `if-else`.

## Milestone 3: Functions and Scope

- [x] **Function Definitions:** Support for `fn` declarations.
- [x] **Function Calls:** Implement function call expressions.
- [ ] **Scope Management:** Implement a symbol table to manage variable scopes.

## Milestone 4: Advanced Language Features

- [ ] **Classes and Structs:** Implement `class` and `struct` definitions.
- [ ] **Ownership and Borrowing:** Implement the ownership and borrowing system.
- [ ] **Error Handling:** Implement `Result<T, E>` and the `?` operator.

## Milestone 5: LLVM Backend and Code Generation

- [ ] **LLVM Integration:** Set up the LLVM backend.
- [ ] **Code Generation:** Implement a code generator to translate the AST to LLVM IR.
- [ ] **Executable Generation:** Generate executable files from the LLVM IR.
