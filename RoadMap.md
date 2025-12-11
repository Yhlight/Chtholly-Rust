# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language, a compiled, general-purpose systems programming language designed to offer a balance of C++'s performance and Rust's memory safety.

## Core Principles

- **Ownership and Borrowing:** Eliminate memory errors at compile time.
- **Zero-Cost Abstractions:** Ensure that high-level language features do not introduce runtime overhead.
- **Compile-Time Programming:** Allow for complex computations and code generation to be performed at compile time.

## Milestone 1: Foundational Infrastructure (Completed)

This milestone focuses on setting up the basic project structure and build system.

- [x] **Project Structure:** Create the `src`, `tests`, and `cmake` directories.
- [x] **Build System:** Create a root `CMakeLists.txt` file to define the project and its dependencies.
- [x] **Entry Point:** Create a `main.cpp` file to serve as the entry point for the Chtholly compiler.
- [x] **Version Control:** Create a `.gitignore` file to exclude build artifacts.

## Milestone 2: Lexer and Parser (In Progress)

This milestone focuses on implementing the front-end of the compiler, which is responsible for parsing Chtholly source code.

- [ ] **Lexer:** Implement a lexer to tokenize the Chtholly source code.
- [ ] **Parser:** Implement a parser to build an Abstract Syntax Tree (AST) from the tokens.
- [ ] **AST:** Define the AST nodes for all the language constructs specified in `Chtholly.md`.
- [ ] **Testing:** Write unit tests for the lexer and parser.

## Milestone 3: Semantic Analysis and Type Checking

This milestone focuses on implementing the semantic analysis phase of the compiler, which is responsible for enforcing the language's rules.

- [ ] **Semantic Analyzer:** Implement a semantic analyzer to traverse the AST and perform type checking.
- [ ] **Symbol Tables:** Implement symbol tables to store information about variables, functions, and types.
- [ ] **Ownership and Borrowing:** Enforce the ownership and borrowing rules at the semantic analysis stage.
- [ ] **Testing:** Write unit tests for the semantic analyzer.

## Milestone 4: LLVM IR Generation

This milestone focuses on implementing the back-end of the compiler, which is responsible for generating LLVM Intermediate Representation (IR).

- [ ] **Code Generator:** Implement a code generator that traverses the AST and generates LLVM IR.
- [ ] **Type Mapping:** Map Chtholly types to LLVM types.
- [ ] **IR Generation:** Generate LLVM IR for all the language constructs.
- [ ] **Testing:** Write unit tests for the code generator.

## Milestone 5: Compile-Time System

This milestone focuses on implementing the compile-time programming features of Chtholly.

- [ ] **`const` System:** Implement the `const` system, including `const` variables, `const fn`, and `const struct`.
- [ ] **Compile-Time Evaluation:** Implement the compile-time evaluation of `const` expressions.
- [ ] **Rules and Restrictions:** Enforce the rules and restrictions of the `const` system.
- [ ] **Testing:** Write unit tests for the compile-time system.

## Milestone 6: Standard Library

This milestone focuses on implementing the Chtholly standard library.

- [ ] **`iostream` Module:** Implement the `iostream` module with `print` and `println` functions.
- [ ] **`optional` Module:** Implement the `optional` module.
- [ ] **`Result<T, E>` Type:** Implement the `Result<T, E>` type and the `?` operator.
- [ ] **Module and Package System:** Implement the module and package system.

## Milestone 7: Advanced Features

This milestone focuses on implementing advanced language features.

- [ ] **Generics:** Implement generics for functions and classes.
- [ ] **Lambda Expressions:** Implement lambda expressions.
- [ ] **Error Handling:** Implement error handling with `Result<T, E>`.
