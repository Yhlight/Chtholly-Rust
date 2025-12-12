# Chtholly Language Development Roadmap

This document outlines the planned development milestones for the Chtholly programming language. The development is driven by a Test-Driven Development (TDD) philosophy to ensure stability and incremental progress. The core focus is on implementing a robust ownership system, borrow checking, and a powerful compile-time execution model.

---

## Milestone 1: Project Foundation & Basic Syntax

*Goal: Establish the basic infrastructure for the compiler and parse a simple "Hello, World!" program.*

- **[X] Build System:** Configure CMake for the project, including LLVM integration.
- **[X] Lexer:** Implement a lexical analyzer to tokenize the source code (keywords, identifiers, literals, operators).
- **[X] Parser:** Build a parser for the most basic syntax, including `fn main`, variable declarations, and function calls.
- **[X] Abstract Syntax Tree (AST):** Define and construct the AST for the parsed code.
- **[X] Code Generation (LLVM):** Implement a basic code generator that can traverse the AST and produce LLVM IR for a simple program that prints "Hello, World!".
- **[ ] Testing Framework:** Set up the initial testing infrastructure for TDD.

---

## Milestone 2: Core Language Features & Type System

*Goal: Implement fundamental language features and a static type system.*

- **[X] Primitive Types:** Implement built-in types (`i32`, `f64`, `bool`, `char`, `void`).
- **[X] Variables:**
- **[X] `let` bindings:** Implement immutable variable declarations.
    - **[X] `let mut` bindings:** Implement mutable variable declarations.
- **[ ] Type Checking:**
    - **[X] Type Inference:** Implement basic type inference for variables.
    - **[X] Type Annotations:** Support explicit type annotations (`let x: i32`).
- **[X] Operators:** Implement basic arithmetic, logical, and comparison operators.
- **[X] Functions:** Support function definitions with parameters and return types.
- **[X] Control Flow:** Implement `if`/`else` expressions.

---

## Milestone 3: Ownership, Borrowing, and Lifetimes (Core Focus)

*Goal: Implement Chtholly's core memory safety features. This is the most critical milestone.*

- **[X] Ownership System:**
    - **[X] Move Semantics:** Enforce move semantics for assignments and function calls. The original variable becomes inaccessible after a move.
    - **[X] `Copy` Trait:** Implement a `Copy` trait for primitive types that are copied instead of moved.
    - **[ ] Drop Semantics:** Implement automatic resource cleanup (`drop`/destructor calls) when a variable goes out of scope.
- **[X] Borrowing & References:**
    - **[X] Immutable References (`&`):** Allow creating shared references to a resource.
    - **[ ] Mutable References (`&mut`):** Allow creating exclusive, mutable references.
- **[ ] Borrow Checker (Static Analysis):**
    - **[ ] Rule Enforcement:** Implement the core borrow checking rules at compile time:
        - One mutable reference OR multiple immutable references in a scope.
        - No aliasing of mutable references.
    - **[ ] Use-After-Move Detection:** Ensure that moved-from variables cannot be used.
- **[ ] Lifetime Management:**
    - **[ ] Lifetime Analysis:** Implement a static analysis pass to ensure that no reference outlives the data it points to, preventing dangling pointers.
    - **[ ] Lifetime Elision:** Implement the smart lifetime elision rules described in `Chtholly.md` to reduce the need for explicit annotations.

---

## Milestone 4: Composite Data Types

*Goal: Introduce user-defined data structures that integrate with the ownership system.*

- **[X] `struct`:** Implement C-style structs for aggregating data.
- **[ ] `class`:** Implement classes with:
    - **[ ] Member variables** (`let` and `let mut`).
    - **[ ] Constructors** and initialization syntax.
    - **[ ] Destructors** (`~`).
    - **[ ] Member functions** (with `self`, `&self`, `&mut self` semantics).
    - **[ ] Access Modifiers** (`public`, `private`).
- **[ ] `enum`:** Implement enums with associated data and support for pattern matching/destructuring in control flow.
- **[ ] Arrays:**
    - **[ ] Static Arrays (`T[N]`).**
    - **[ ] Dynamic Arrays (`T[]`).**

---

## Milestone 5: Compile-Time Execution System (Core Focus)

*Goal: Implement the `const` system to shift computation from runtime to compile time.*

- **[ ] `const` Declarations:** Implement compile-time constants.
- **[ ] `const fn`:**
    - **[ ] Compile-Time Interpreter:** Build an evaluator that can execute `const fn` at compile time.
    - **[ ] Purity Enforcement:** Ensure `const fn` are pure and have no side effects.
    - **[ ] Const Context:** Define and enforce the rules of the "Const Context" (what is allowed and disallowed in `const` evaluations).
- **[ ] `const struct`:** Implement compile-time-only structs.
- **[ ] Compile-Time Error Handling:** Implement `Result<T, E>` handling within the compile-time interpreter.

---

## Milestone 6: Modularity and Error Handling

*Goal: Implement a module system and robust error handling mechanisms.*

- **[ ] Module System:**
    - **[ ] `import` statement:** Implement file-based module loading.
    - **[ ] `package` statement:** Organize modules into packages.
    - **[ ] Namespacing:** Use file/package names as namespaces to prevent conflicts.
- **[ ] Error Handling:**
    - **[ ] `Result<T, E>` Enum:** Implement the standard `Result` type.
    - **[ ] `?` Operator:** Implement the error propagation operator for cleaner error handling code.
    - **[ ] `switch` on `Result`:** Support pattern matching on `Ok(T)` and `Err(E)`.

---

## Milestone 7: Generics and Advanced Features

*Goal: Add support for generic programming and other modern language features.*

- **[ ] Generics:**
    - **[ ] Generic Functions:** `fn add<T>(a: T, b: T): T`.
    - **[ ] Generic `struct`s and `class`es:** `class Point<T>`.
    - **[ ] Specialization:** Allow providing specialized implementations for generics.
- **[ ] `optional<T>`:** Implement a safe `optional` type.
- **[ ] Advanced Control Flow:**
    - **[ ] `switch-case` statement.**
    - **[ ] `for`, `while`, and `do-while` loops.**

---

## Future Milestones

*Features to be considered after the core language is stable.*

- **[ ] Standard Library:** Develop a comprehensive standard library (`iostream`, `string`, collections, etc.).
- **[ ] Advanced Lifetime Annotations:** Allow for explicit lifetime annotations in complex scenarios where elision is not sufficient.
- **[ ] Concurrency:** Explore and implement safe concurrency primitives.
- **[ ] Tooling:**
    - **[ ] Package Manager.**
    - **[ ] Language Server (for IDE support).**
    - **[ ] Code Formatter.**
- **[ ] Performance Optimizations:** Advanced compiler optimizations.
