# Chtholly Compiler Development Roadmap

This document outlines the development plan for the Chtholly compiler, following the Scrum and TDD methodologies.

## Sprint 1: Project Setup and Lexer

*   **Goal:** Set up the project structure and implement the lexer.
*   **Tasks:**
    *   [ ] Initialize the Rust project using Cargo.
    *   [ ] Set up the `.gitignore` file.
    *   [ ] Implement the lexer to tokenize the source code according to the `Chtholly.md` specification.
    *   [ ] Write unit tests for the lexer to ensure correctness.

## Sprint 2: Parser and AST

*   **Goal:** Implement the parser to build an Abstract Syntax Tree (AST) from the token stream.
*   **Tasks:**
    *   [ ] Define the AST data structures in `ast.rs`.
    *   [ ] Implement the parser in `parser.rs`.
    *   [ ] Write unit tests for the parser to verify that it correctly parses valid code and handles errors for invalid code.

## Sprint 3: Semantic Analysis

*   **Goal:** Implement the semantic analyzer to perform type checking and other static analysis.
*   **Tasks:**
    *   [ ] Implement the semantic analyzer.
    *   [ ] Perform type checking to ensure that all expressions and statements are well-typed.
    *   [ ] Perform other static analysis, such as checking for undefined variables.
    *   [ ] Write unit tests for the semantic analyzer.

## Sprint 4: LLVM IR Generation

*   **Goal:** Implement the code generator to translate the AST into LLVM Intermediate Representation (IR).
*   **Tasks:**
    *   [ ] Set up the LLVM toolchain.
    *   [ ] Implement the code generator to traverse the AST and generate LLVM IR.
    *   [ ] Write unit tests for the code generator to verify that it generates correct LLVM IR.

## Sprint 5: JIT Compilation and Execution

*   **Goal:** Implement a Just-In-Time (JIT) compiler to execute the generated LLVM IR.
*   **Tasks:**
    *   [ ] Implement the JIT compiler.
    *   [ ] Write integration tests to verify that the entire compiler pipeline works correctly.

## Sprint 6: AOT Compilation

*   **Goal:** Implement an Ahead-Of-Time (AOT) compiler to generate executable files.
*   **Tasks:**
    *   [ ] Implement the AOT compiler.
    *   [ ] Write integration tests to verify that the generated executables work correctly.
