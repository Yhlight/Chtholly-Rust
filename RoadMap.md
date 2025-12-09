# Chtholly Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development process will be incremental, following Scrum and TDD principles to ensure stability and avoid regressions. The core focus of Chtholly is to provide a safe and ergonomic system programming experience by leveraging ownership, borrow checking, and automatic lifetime management.

## Phase 1: Core Language Features

This phase focuses on implementing the fundamental features of the language, including the lexer, parser, and basic semantic analysis.

*   **Lexer:** Implemented.
*   **Parser:** In-progress. Implemented variable declarations and expression parsing (unary, binary, grouping).
*   **AST:** In-progress. Expanded to include nodes for various expression types.
*   **Basic Semantic Analysis:**
    *   **Type System:** Implement the basic type system, including primitive types, arrays, and strings.
    *   **Variable Declarations:** Support for `let` and `let mut` variable declarations.
    *   **Scope Management:** Implement symbol tables to manage scopes and variable visibility.

## Phase 2: Ownership and Borrowing

This is the most critical phase of the project, as it involves implementing the core memory safety features of Chtholly.

*   **Ownership:**
    *   Implement move semantics for variable assignments and function calls.
    *   Track ownership of resources and ensure that each resource has a single owner.
    *   Implement the `Copy` trait for primitive types.
*   **Borrowing:**
    *   Implement immutable (`&`) and mutable (`&mut`) borrows.
    *   Enforce the borrowing rules:
        *   Multiple immutable borrows are allowed.
        *   Only one mutable borrow is allowed at a time.
        *   A mutable borrow cannot coexist with any other borrows.
*   **Lifetime Management:**
    *   Implement lifetime analysis to ensure that all borrows are valid and do not outlive the resources they point to.
    *   Implement smart lifetime elision to reduce the need for explicit lifetime annotations.

## Phase 3: Control Flow and Functions

This phase will introduce control flow structures and functions to the language.

*   **Control Flow:**
    *   Implement `if-else` expressions.
    *   Implement `while`, `for`, and `do-while` loops.
    *   Implement `switch-case` statements with fallthrough.
*   **Functions:**
    *   Implement function definitions and calls.
    *   Support for function parameters and return types.
    *   Implement the `main` function as the entry point of the program.

## Phase 4: Data Structures

This phase will add support for more complex data structures.

*   **Structs:** Implement `struct` definitions and instantiation.
*   **Classes:** Implement `class` definitions, including member variables, constructors, and destructors.
*   **Enums:** Implement `enum` definitions with support for associated data.

## Phase 5: Advanced Features

This phase will introduce more advanced language features.

*   **Generics:** Implement generic functions, structs, and classes.
*   **Contracts (Traits):** Implement contracts to define shared behavior.
*   **Modules:** Implement a module system for code organization.
*   **Error Handling:** Implement the `Result<T, E>` type and the `?` operator for error propagation.

## Phase 6: LLVM Backend

This phase will focus on integrating LLVM to generate machine code.

*   **LLVM IR Generation:** Implement a code generator that traverses the AST and generates LLVM Intermediate Representation (IR).
*   **Code Generation:** Compile the LLVM IR into machine code for the target platform.
*   **Standard Library:** Begin development of the Chtholly standard library.

This roadmap will be updated as the project progresses. Each feature will be implemented with a corresponding set of tests to ensure correctness and prevent regressions.