# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Project Setup and Core Infrastructure

- [ ] **Project Initialization:**
  - [ ] Initialize Rust project with Cargo.
  - [ ] Configure `Cargo.toml` with `inkwell` dependency.
  - [ ] Set up `.gitignore`.
- [ ] **Environment Setup:**
  - [ ] Install Rust and Cargo.
  - [ ] Install LLVM 18 and required libraries.
  - [ ] Configure environment variables.

## Phase 2: Lexer and Parser

- [ ] **Lexer (Tokenizer):**
  - [ ] Implement tokenization for all language keywords, operators, and literals.
- [ ] **Parser (AST Generation):**
  - [ ] Define Abstract Syntax Tree (AST) nodes.
  - [ ] Implement parsing for basic expressions and statements.
  - [ ] Implement parsing for variable declarations (`let`, `let mut`).
  - [ ] Implement parsing for function definitions (`fn`).
  - [ ] Implement parsing for control flow (`if`, `else`, `while`, `for`).

## Phase 3: Semantic Analysis and Type Checking

- [ ] **Symbol Table:**
  - [ ] Implement a symbol table to track variables, functions, and types.
- [ ] **Type Checking:**
  - [ ] Implement type inference and checking for expressions and variable declarations.
- [ ] **Ownership and Borrowing:**
  - [ ] Implement semantic analysis for ownership, borrowing, and lifetimes.

## Phase 4: Code Generation (LLVM IR)

- [ ] **LLVM Backend Integration:**
  - [ ] Set up `inkwell` to generate LLVM IR.
- [ ] **Code Generation:**
  - [ ] Generate LLVM IR for basic expressions and statements.
  - [ ] Generate LLVM IR for functions.
  - [ ] Generate LLVM IR for control flow.

## Phase 5: Advanced Features

- [ ] **Classes and Structs:**
  - [ ] Implement parsing, semantic analysis, and code generation for classes and structs.
- [ ] **Generics:**
  - [ ] Implement support for generic functions and types.
- [ ] **Error Handling:**
  - [ ] Implement `Result<T, E>` and the `?` operator.
- [ ] **Module System:**
  - [ ] Implement `import` and `package` functionality.

## Phase 6: Standard Library

- [ ] **Core Library:**
  - [ ] Implement basic I/O operations (`println`).
  - [ ] Implement string manipulation functions.
  - [ ] Implement `optional` type.

## Phase 7: Testing and CI/CD

- [ ] **Unit Tests:**
  - [ ] Write unit tests for the lexer, parser, and code generator.
- [ ] **Integration Tests:**
  - [ ] Write integration tests for the entire compiler.
- [ ] **CI/CD:**
  - [ ] Set up a CI/CD pipeline for automated testing and builds.
