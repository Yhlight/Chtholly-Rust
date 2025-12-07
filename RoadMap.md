# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language.

## Phase 1: Core Language Features

*   [x] Project Setup
*   [ ] **Lexer and Parser:**
    *   [ ] Comments (single-line and multi-line)
    *   [ ] Keywords and Identifiers
    *   [ ] Literals (integers, floats, strings, chars, booleans)
    *   [ ] Operators
    *   [ ] Basic AST structure
*   [ ] **Variables and Data Types:**
    *   [ ] `let` and `mut` bindings
    *   [ ] Primitive types (`i32`, `f64`, `char`, `bool`, `string`)
    *   [ ] Type inference and annotations
    *   [ ] Ownership and Move Semantics
*   [ ] **Functions:**
    *   [ ] Function definition and calls
    *   [ ] `main` function as entry point
    *   [ ] Return values (`void` and other types)
    *   [ ] Parameters
*   [ ] **Control Flow:**
    *   [ ] `if-else` statements
    *   [ ] `while` loops
    *   [ ] `for` loops (C-style)
    *   [ ] `do-while` loops
    *   [ ] `switch-case` statements
*   [ ] **Basic LLVM Integration:**
    *   [ ] Generate LLVM IR for basic arithmetic operations
    *   [ ] Compile and execute a simple "Hello, World!" program

## Phase 2: Advanced Language Features

*   [ ] **Structs and Classes:**
    *   [ ] Struct definitions and instantiation
    *   [ ] Class definitions, constructors, and destructors
    *   [ ] Member variables (`let`, `mut`)
    *   [ ] Member functions (`self`, `&self`, `&mut self`)
    *   [ ] Access modifiers (`public`, `private`)
*   [ ] **Enums:**
    *   [ ] Simple enums
    *   [ ] Enums with associated data
    *   [ ] Pattern matching with `switch`
*   [ ] **Arrays:**
    *   [ ] Static arrays (`i32[4]`)
    *   [ ] Dynamic arrays (`i32[]`)
    *   [ ] Array access
*   [ ] **Borrowing and References:**
    *   [ ] Immutable references (`&`)
    *   [ ] Mutable references (`&mut`)
    *   [ ] Borrowing rules enforcement in the compiler
*   [ ] **Modules and Imports:**
    *   [ ] `import` statement for files and standard library
    *   [ ] `use` statement for bringing symbols into scope
    *   [ ] `package` declaration

## Phase 3: Generics, Error Handling, and Standard Library

*   [ ] **Generics:**
    *   [ ] Generic functions
    *   [ ] Generic structs and classes
    *   [ ] Generic member functions
*   [ ] **Error Handling:**
    *   [ ] `Result<T, E>` type
    *   [ ] `?` operator for error propagation
*   [ ] **Traits/Requests:**
    *   [ ] `request` keyword for defining interfaces
    *   [ ] `require` keyword for implementing interfaces
    *   [ ] Associated types
*   [ ] **Standard Library:**
    *   [ ] `iostream` module (for `print`, `println`)
    *   [ ] `optional` module
    *   [ ] `iterator` module and `foreach` loop

## Phase 4: Tooling and Ecosystem

*   [ ] **Build System:**
    *   [ ] A command-line tool to compile Chtholly code
*   [ ] **Package Manager:**
    *   [ ] A tool to manage dependencies and projects
*   [ ] **Documentation:**
    *   [ ] Comprehensive documentation for the language and standard library.
