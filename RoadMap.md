# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Project Setup and Core Infrastructure (Completed)

- [x] **Project Initialization:**
  - [x] Initialize Rust project with Cargo.
  - [x] Configure `Cargo.toml` with `inkwell` dependency.
  - [x] Set up `.gitignore`.
- [x] **Environment Setup:**
  - [x] Install Rust and Cargo.
  - [x] Install LLVM 18 and required libraries.
  - [x] Configure environment variables.

## Phase 2: Lexer and Parser

- [x] **Lexer (Tokenizer):**
  - [x] Implement tokenization for basic keywords, operators, and literals.
  - [x] Handle single-line (`//`) and multi-line (`/* ... */`) comments.
- [x] **Parser (AST Generation):**
  - [x] Define Abstract Syntax Tree (AST) nodes for expressions.
  - [x] Implement parsing for basic expressions.
  - [x] Implement parsing for statements (e.g., variable declarations).
  - [x] Implement parsing for variable declarations (`let`, `let mut`).
  - [ ] Implement parsing for function definitions (`fn`).
  - [ ] Implement parsing for control flow (`if`, `else`, `while`, `for`).

## Phase 3: Semantic Analysis and Type Checking

- [x] **Symbol Table:**
  - [x] Implement a symbol table to track variables.
- [ ] **Type Checking:**
  - [ ] Implement type inference and checking for expressions and variable declarations.
- [ ] **Ownership and Borrowing:**
  - [ ] Implement semantic analysis for ownership, borrowing, and lifetimes.

## Phase 4: Code Generation (LLVM IR)

- [x] **LLVM Backend Integration:**
  - [x] Set up `inkwell` to generate LLVM IR.
- [x] **Code Generation:**
  - [x] Generate LLVM IR for basic expressions.
  - [x] Generate LLVM IR for variable declarations and assignments.
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

- [x] **Unit Tests:**
  - [x] Write unit tests for the initial lexer, parser, and code generator.
  - [x] Add tests for new features (variables, comments, etc.).
- [ ] **Integration Tests:**
  - [ ] Write integration tests for the entire compiler.
- [ ] **CI/CD:**
  - [ ] Set up a CI/CD pipeline for automated testing and builds.
