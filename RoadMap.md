# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. Our goal is to create a systems programming language that combines the performance of C++ with the memory safety of Rust, guided by the principles of zero-cost abstractions and a minimal runtime.

## Core Philosophy

- **Safety by Default:** Implement a robust ownership and borrow checking system to eliminate memory errors at compile time.
- **Ergonomics:** Design a clean and intuitive syntax that is easy to learn and use.
- **Performance:** Leverage LLVM to generate highly optimized machine code.
- **TDD:** Employ Test-Driven Development to ensure stability and prevent regressions.

---

## Milestone 1: Foundational Infrastructure (The "Bootstrap" Phase)

This initial phase focuses on setting up the basic project structure, build system, and a minimal compiler that can parse and handle the simplest source file.

- **Tasks:**
    - [x] Set up the project directory structure (`src`, `tests`, `docs`).
    - [x] Configure `CMake` as the build system.
    - [x] Integrate LLVM as the backend.
    - [x] Implement a basic lexer to tokenize the source code.
    - [x] Create a simple parser for the main function `fn main() {}`.
    - [x] Set up the initial testing framework.
    - [ ] **Goal:** Compile a `.cns` file with an empty `main` function.

---

## Milestone 2: Core Language Features (The "Core" Phase)

This phase focuses on implementing the fundamental building blocks of the language, including variables, data types, and basic control flow. The ownership and borrow checking system will be a primary focus here.

- **Tasks:**
    - [ ] **Ownership and Borrowing:**
        - [ ] Implement `let` for immutable variables and move semantics.
        - [ ] Implement `let mut` for mutable variables.
        - [ ] Introduce borrow checking for immutable (`&`) and mutable (`&mut`) references.
        - [ ] Implement lifetime analysis to prevent dangling references.
    - [ ] **Data Types:**
        - [ ] Implement primitive types (`i32`, `f64`, `bool`, `char`).
        - [ ] Implement basic array types (`i32[]`, `i32[4]`).
        - [ ] Implement `string` literals.
    - [ ] **Control Flow:**
        - [ ] Implement `if-else` expressions.
        - [ ] Implement `while` and `for` loops.
    - [ ] **Functions:**
        - [ ] Implement function definitions with `fn`.
        - [ ] Implement function calls.
    - [ ] **Goal:** Be able to write simple programs with variables, loops, and functions, all while enforcing strict memory safety.

---

## Milestone 3: Advanced Language Features (The "Abstraction" Phase)

This phase will build on the core language to provide more powerful abstraction mechanisms.

- **Tasks:**
    - [ ] **Structs and Classes:**
        - [ ] Implement `struct` definitions and instantiation.
        - [ ] Implement `class` definitions, including constructors and member functions.
        - [ ] Implement `self`, `&self`, and `&mut self` semantics.
    - [ ] **Modules and Imports:**
        - [ ] Implement the module system with `import`.
        - [ ] Implement `package` support.
    - [ ] **Error Handling:**
        - [ ] Implement the `Result<T, E>` enum for error handling.
        - [ ] Implement the `?` operator for error propagation.
    - [ ] **Generics:**
        - [ ] Implement generic functions.
        - [ ] Implement generic `struct`s and `class`es.
    - [ ] **Goal:** Enable the creation of complex, reusable, and robust code abstractions.

---

## Milestone 4: Compile-Time System (The "Const" Phase)

This phase will focus on implementing Chtholly's compile-time programming capabilities, a key feature for achieving zero-cost abstractions.

- **Tasks:**
    - [ ] **`const` and `const fn`:**
        - [ ] Implement `const` variables for compile-time constants.
        - [ ] Implement `const fn` for functions that can be evaluated at compile time.
    - [ ] **Const Generics:**
        - [ ] Allow constants to be used as generic parameters.
    - [ ] **Compile-Time Error Handling:**
        - [ ] Integrate `Result<T, E>` with the `const` system.
    - [ ] **Goal:** Allow developers to shift computation from runtime to compile time, improving performance and safety.

---

## Milestone 5: Standard Library and Ecosystem (The "Maturity" Phase)

With the language features in place, this phase will focus on building a standard library and the tools needed for a healthy ecosystem.

- **Tasks:**
    - [ ] **Standard Library:**
        - [ ] Develop a core library with essential data structures (`Vec`, `HashMap`).
        - [ ] Implement I/O modules for file and network operations.
        - [ ] Create a robust testing framework.
    - [ ] **Tooling:**
        - [ ] Develop a package manager.
        - [ ] Create a code formatter and linter.
    - [ ] **Goal:** Provide a rich and productive development experience for Chtholly users.
