# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Language Features

- [x] **Lexer:** Tokenize the source code.
- [x] **Parser:** Build the Abstract Syntax Tree (AST).
- [x] **Compiler:**
    - [x] Basic LLVM integration.
    - [x] Main function compilation.
    - [x] Variable declarations (let, let mut).
    - [x] Basic types (i32, f64, bool, string).
- [x] **Basic Syntax:**
    - [x] Comments (single-line and multi-line).
    - [x] Main function definition.

## Phase 2: Advanced Language Features

- [ ] **Control Flow:**
    - [x] if-else statements.
    - [ ] while loops.
    - [ ] for loops.
- [ ] **Functions:**
    - [ ] Function definitions and calls.
    - [ ] Return values.
    - [ ] Function arguments.
- [ ] **Classes and Structs:**
    - [ ] Class and struct definitions.
    - [ ] Member variables.
    - [ ] Member functions.
    - [ ] Constructors and destructors.
- [ ] **Ownership and Borrowing:**
    - [ ] Implement ownership and move semantics.
    - [ ] Implement borrowing and references.
    - [ ] Lifetime analysis.

## Phase 3: Standard Library

- [ ] **IO:** Basic input/output operations.
- [ ] **Collections:**
    - [ ] Dynamic arrays (vectors).
    - [ ] Hash maps.
- [ ] **String Manipulation:**
    - [ ] String concatenation.
    - [ ] Substring operations.

## Phase 4: Tooling and Ecosystem

- [ ] **Build System:**
    - [ ] Package manager.
    - [ ] Dependency management.
- [ ] **Testing Framework:**
    - [ ] Unit testing support.
    - [ ] Integration testing support.
- [ ] **Documentation:**
    - [ ] Language reference.
    - [ ] Standard library documentation.
