# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Language Features

- [ ] **Lexer & Parser:** Implement the lexical and syntactical analysis of the Chtholly language.
  - [x] **Lexer:** Implemented.
  - [ ] **Parser:** In progress.
- [x] **AST (Abstract Syntax Tree):** Define and build the AST to represent the code's structure.
- [ ] **Basic Type System:** Implement support for basic types such as `i32`, `f64`, `bool`, `char`, and `string`.
- [ ] **Variable Declarations:** Implement `let` (immutable) and `mut` (mutable) variable declarations.
- [ ] **Basic Operators:** Implement arithmetic, comparison, and logical operators.
- [ ] **Function Definitions and Calls:** Implement function definitions and call expressions.
- [x] **Control Flow:** Implement `if/else`, `while`, and `for` loops.
- [ ] **LLVM IR Generation:** Generate LLVM Intermediate Representation for the core language features.
- [ ] **Main Function:** Implement the `fn main` function as the entry point of the program.

## Phase 2: Advanced Language Features

- [ ] **Ownership and Borrowing:** Implement the ownership and borrowing system with `&` and `&mut`.
- [ ] **Structs and Classes:** Implement `struct` and `class` definitions, including member variables and methods.
- [ ] **Enums:** Implement `enum` types with support for associated data.
- [ ] **Arrays:** Implement static and dynamic arrays.
- [ ] **Modules and Imports:** Implement the module system with `import` statements.

## Phase 3: Standard Library

- [ ] **Basic I/O:** Implement a basic I/O library with functions like `println`.
- [ ] **String Manipulation:** Implement a string manipulation library.
- [ ] **Collections:** Implement common data structures like `Vector` and `HashMap`.

## Phase 4: Tooling and Ecosystem

- [ ] **Build System Integration:** Improve and expand the CMake build system.
- [ ] **Package Manager:** Design and implement a package manager for Chtholly.
- [ ] **Language Server Protocol (LSP):** Implement an LSP for IDE support, providing features like code completion and diagnostics.
