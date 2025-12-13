# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is divided into several phases, each with specific milestones. This roadmap is a living document and may be updated as the project progresses.

The core focus of Chtholly is to provide a safe and performant systems programming language by implementing modern features like ownership, borrowing, and a powerful compile-time execution system.

---

## Phase 1: Core Language and Compiler Infrastructure

This phase focuses on setting up the basic infrastructure for the compiler, including lexical analysis, parsing, and basic code generation.

*   **Milestone 1.1: Project Setup & Lexer**
    *   [x] Set up the development environment with CMake, C++17, and LLVM 18.
    *   [x] Implement the lexer to tokenize the Chtholly source code (`.cns` files).
    *   [x] Define all tokens based on `Chtholly.md` (keywords, identifiers, literals, operators, etc.).
    *   [x] Write unit tests for the lexer to cover all token types and edge cases.

*   **Milestone 1.2: Parser & Abstract Syntax Tree (AST)**
    *   [x] Define the structure of the Abstract Syntax Tree (AST).
    *   [x] Implement a parser that consumes tokens from the lexer and builds an AST.
    *   [x] Start with parsing basic constructs like variable declarations (`let`, `let mut`).
    *   [x] Write unit tests for the parser to verify the correctness of the generated AST for simple expressions.

*   **Milestone 1.3: Basic Semantic Analysis**
    *   [x] Implement a semantic analyzer that traverses the AST.
    *   [x] Create a symbol table to track variable declarations and their types.
    *   [x] Implement basic type checking for variable assignments and simple expressions.
    *   [x] Write unit tests to check for semantic errors like undeclared variables or type mismatches.

*   **Milestone 1.4: LLVM IR Generation**
    *   [ ] Set up the LLVM backend.
    *   [ ] Implement a code generator that traverses the AST and generates LLVM Intermediate Representation (IR).
    *   [ ] Focus on generating code for simple variable declarations and assignments.
    *   [ ] Write integration tests that compile a simple Chtholly program to LLVM IR.

*   **Milestone 1.5: End-to-End "Hello, World!"**
    *   [ ] Extend the lexer, parser, and code generator to support function calls (e.g., a built-in `println` function).
    *   [ ] Implement the `main` function entry point.
    *   [ ] Achieve the first successful compilation and execution of a "Hello, World!" program.

---

## Phase 2: Ownership, Borrowing, and Memory Management

This is the most critical phase, focusing on Chtholly's core memory safety features.

*   **Milestone 2.1: Implementing Ownership and Move Semantics**
    *   [ ] Enhance the semantic analyzer to track the ownership of variables.
    *   [ ] Implement move semantics for assignments and function arguments.
    *   [ ] The compiler should invalidate a variable after its value has been moved.
    *   [ ] Write tests to ensure that using a moved variable results in a compile-time error.

*   **Milestone 2.2: Implementing Borrowing and References**
    *   [ ] Add support for immutable (`&`) and mutable (`&mut`) references to the parser and AST.
    *   [ ] Implement the borrowing rules in the semantic analyzer:
        *   Multiple immutable references are allowed.
        *   Only one mutable reference is allowed at a time.
        *   Mutable and immutable references cannot coexist.
    *   [ ] Write extensive tests to enforce the borrowing rules at compile time.

*   **Milestone 2.3: Lifetime Analysis**
    *   [ ] Implement lifetime analysis to prevent dangling references.
    *   [ ] Start with basic lexical lifetime analysis.
    *   [ ] Implement the smart lifetime elision rules described in `Chtholly.md`.
    *   [ ] Write tests that attempt to create dangling references and ensure they are caught at compile time.

*   **Milestone 2.4: Destructors and Resource Management**
    *   [ ] Implement support for destructors (`~ClassName`).
    *   [ ] The compiler should automatically insert calls to destructors when a variable goes out of scope.
    *   [ ] Write tests to verify that destructors are called correctly, ensuring automatic resource management.

---

## Phase 3: Expanding Language Features

This phase will add more features to make Chtholly a more expressive and useful language.

*   **Milestone 3.1: Functions, Classes, and Structs**
    *   [ ] Full implementation of function definitions, including parameters and return types.
    *   [ ] Implement classes with member variables, constructors, and member functions.
    *   [ ] Implement structs as pure data aggregates.
    *   [ ] Add support for `self`, `&self`, and `&mut self` in member functions.

*   **Milestone 3.2: Control Flow**
    *   [x] Implement `if-else` expressions.
    *   [ ] Implement `while`, `for`, and `do-while` loops.
    *   [ ] Implement `switch-case` statements with the improved semantics (`fallthrough`).

*   **Milestone 3.3: Advanced Types**
    *   [ ] Implement static (`T[N]`) and dynamic (`T[]`) arrays.
    *   [ ] Implement `enum` types, including support for associated data and pattern matching/destructuring.
    *   [ ] Implement generics for functions and classes.

*   **Milestone 3.4: Modules and Error Handling**
    *   [ ] Implement the module system with `import` and `package`.
    *   [ ] Implement the `Result<T, E>` type for error handling.
    *   [ ] Implement the `?` operator for error propagation.

---

## Phase 4: Compile-Time System (Const System)

This phase focuses on building the compile-time programming capabilities of Chtholly.

*   **Milestone 4.1: `const` Variables**
    *   [ ] Implement support for `const` variables that are evaluated at compile time.
    *   [ ] Ensure `const` values are embedded directly into the generated code.

*   **Milestone 4.2: `const fn`**
    *   [ ] Implement `const fn` for functions that can be executed at compile time.
    *   [ ] Enforce the rules for `const fn` (no side effects, only calling other `const fn`).

*   **Milestone 4.3: `const struct` and Compile-Time Data Structures**
    *   [ ] Implement `const struct` to create compile-time data structures.
    *   [ ] Explore the capabilities of manipulating these structures at compile time.

*   **Milestone 4.4: Compile-Time Error Handling**
    *   [ ] Allow `const fn` to return `Result<T, E>`.
    *   [ ] Force the handling of `Err` results from `const fn` at compile time.

---

## Phase 5: Standard Library and Tooling

The final phase will focus on building a usable ecosystem around the language.

*   **Milestone 5.1: Standard Library**
    *   [ ] Create a basic `iostream` module.
    *   [ ] Implement a `string` manipulation module.
    *   [ ] Develop a container library (e.g., `optional`, dynamic arrays).

*   **Milestone 5.2: Build System and Tooling**
    *   [ ] Improve the build system with Python helper scripts.
    *   [ ] Develop a code formatter and a linter.

*   **Milestone 5.3: Documentation**
    *   [ ] Write comprehensive documentation for the language and the standard library.
    *   [ ] Create a tool to generate documentation from source code comments.
