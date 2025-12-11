# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is divided into several milestones, each focusing on a specific set of features. This approach allows for incremental development and testing, ensuring the stability of the language as it grows.

## Milestone 1: Foundational Components

This milestone focuses on setting up the basic infrastructure of the compiler.

*   [ ] **Project Setup:**
    *   [ ] Create the basic directory structure (`src`, `tests`, `cmake`).
    *   [ ] Set up the initial CMake build system.
    *   [ ] Configure LLVM dependencies.
*   [ ] **Lexer:**
    *   [ ] Implement the lexer to tokenize the Chtholly source code.
    *   [ ] Support for all keywords, operators, and literals defined in `Chtholly.md`.
*   [ ] **Parser:**
    *   [ ] Implement the parser to build an Abstract Syntax Tree (AST) from the token stream.
    *   [ ] Define the AST nodes for all language constructs.
*   [ ] **Basic Command-Line Interface:**
    *   [ ] Create a simple command-line tool to invoke the compiler.
    *   [ ] Add options for input files and output files.

## Milestone 2: Semantic Analysis and Type System

This milestone will focus on the static analysis of the code to enforce language rules.

*   [ ] **Symbol Table:**
    *   [ ] Implement a symbol table to store information about variables, functions, and types.
*   [ ] **Type Checking:**
    *   [ ] Implement type checking for primitive types.
    *   [ ] Enforce type compatibility rules for expressions and assignments.
*   [ ] **Variable Scopes:**
    *   [ ] Implement scoping rules for variables (`let` and `let mut`).
    *   [ ] Handle variable declarations and lookups.

## Milestone 3: Code Generation with LLVM

This milestone will focus on generating LLVM Intermediate Representation (IR) from the AST.

*   [ ] **LLVM Integration:**
    *   [ ] Set up the LLVM backend.
    *   [ ] Initialize LLVM modules and contexts.
*   [ ] **Code Generation for Expressions:**
    *   [ ] Generate LLVM IR for arithmetic, logical, and comparison expressions.
*   [ ] **Code Generation for Statements:**
    *   [ ] Generate LLVM IR for `if-else`, `while`, and `for` statements.
*   [ ] **Function Compilation:**
    *   [ ] Generate LLVM IR for function definitions and calls.

## Milestone 4: Core Feature: Ownership and Borrowing

This is a critical milestone that implements the core memory safety features of Chtholly.

*   [ ] **Ownership System:**
    *   [ ] Implement move semantics for variable assignments and function calls.
    *   [ ] Track ownership of resources in the semantic analyzer.
    *   [ ] Automatically generate code for resource cleanup (destructors).
*   [ ] **Borrow Checker:**
    *   [ ] Implement borrow checking for immutable (`&`) and mutable (`&mut`) references.
    *   [ ] Enforce the borrowing rules (one mutable reference or multiple immutable references).
*   [ ] **Lifetime Analysis:**
    *   [ ] Implement basic lifetime analysis to prevent dangling references.

## Milestone 5: Advanced Language Features

This milestone will add support for classes, structs, and the module system.

*   [ ] **Classes and Structs:**
    *   [ ] Implement `class` and `struct` definitions.
    *   [ ] Support for member variables and member functions.
    *   [ ] Implement constructors and destructors.
*   [ ] **Module System:**
    *   [ ] Implement `import` and `package` for code organization.
    *   [ ] Handle module loading and symbol resolution.

## Milestone 6: Core Feature: Compile-Time System

This milestone implements the compile-time programming features of Chtholly.

*   [ ] **`const` Variables:**
    *   [ ] Implement compile-time evaluation of `const` variables.
*   [ ] **`const fn`:**
    *   [ ] Implement `const fn` for functions that can be evaluated at compile time.
    *   [ ] Enforce the rules of the `const` context (no side effects, no runtime dependencies).
*   [ ] **`const struct`:**
    *   [ ] Implement `const struct` for compile-time data structures.

## Milestone 7: Error Handling and Tooling

This milestone focuses on improving the developer experience.

*   [ ] **`Result<T, E>` Type:**
    *   [ ] Implement the `Result<T, E>` type for error handling.
    *   [ ] Implement the `?` operator for error propagation.
*   [ ] **Improved Diagnostics:**
    *   [ ] Provide clear and helpful error messages.
    *   [ ] Add source code location information to errors.
*   [ ] **Basic Language Server (LSP):**
    *   [ ] Implement basic LSP features like code completion and diagnostics.

## Milestone 8: Standard Library

This milestone will focus on building a small but useful standard library.

*   [ ] **IO Library:**
    *   [ ] Implement basic input/output operations (`println`, file I/O).
*   [ ] **String Library:**
    *   [ ] Provide a set of functions for string manipulation.
*   [ ] **Containers:**
    *   [ ] Implement common container types like `optional` and arrays.
