# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is divided into several phases, each focusing on a specific set of features.

## Phase 1: Core Compiler Infrastructure (In Progress)

- [ ] **Lexer:** Implement a tokenizer to convert Chtholly source code into a stream of tokens.
- [ ] **Parser:** Develop a parser to build an Abstract Syntax Tree (AST) from the token stream.
- [ ] **AST:** Define the structure of the AST to represent the code's syntax.
- [ ] **LLVM Integration:** Set up the basic infrastructure to interact with the LLVM backend.

## Phase 2: Basic Language Features

- [ ] **Variable Declarations:** Implement `let` and `let mut` for immutable and mutable variables.
- [ ] **Data Types:** Support for basic data types (`i32`, `f64`, `char`, `string`, `bool`).
- [ ] **Operators:** Implement standard arithmetic, comparison, and logical operators.
- [ ] **Functions:** Basic function definition and invocation.
- [ ] **Control Flow:** `if-else` and `while` loops.

## Phase 3: Advanced Language Features

- [ ] **Ownership and Borrowing:** Implement the core memory management model.
- [ ] **Structs and Classes:** Support for user-defined data structures.
- [ ] **Enums:** Implement enumeration types with support for associated data.
- [ ] **Error Handling:** `Result<T, E>` type and the `?` operator.
- [ ] **Modules:** `import` and `package` system for code organization.

## Phase 4: Standard Library

- [ ] **I/O:** Basic input/output operations.
- [ ] **Collections:** Standard data structures like arrays, vectors, and maps.
- [ ] **String Manipulation:** A comprehensive string utility library.

## Phase 5: Tooling and Ecosystem

- [ ] **Build System:** A dedicated build tool for Chtholly projects.
- [ ] **Package Manager:** A package manager for sharing and reusing Chtholly code.
- [ ] **Formatter:** An automated code formatter to ensure consistent style.
- [ ] **Linter:** A tool for static analysis to identify potential issues.
