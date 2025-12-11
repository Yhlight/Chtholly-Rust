# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development will be done in phases, following a Test-Driven Development (TDD) approach to ensure stability and incremental progress. The core focus will be on **Ownership, Borrow Checking, Automatic Lifetime Management, and the Compile-time System**.

## Phase 1: Project Setup & Core Infrastructure

The goal of this phase is to establish the foundational infrastructure for the compiler.

1.  **CMake Project Setup:**
    *   Configure `CMakeLists.txt` for the project.
    *   Set up C++17 as the standard.
    *   Define project structure (e.g., `src`, `include`, `tests`, `examples`).

2.  **LLVM Integration:**
    *   Integrate LLVM libraries using CMake.
    *   Create a basic LLVM module and JIT engine.

3.  **Command-Line Interface (CLI):**
    *   Implement a basic CLI to take a `.cns` file as input.
    *   The CLI will eventually support different flags (e.g., for emitting LLVM IR, object files).

4.  **Lexer and Parser:**
    *   Implement a lexer to tokenize the Chtholly source code.
    *   Implement a parser to build an Abstract Syntax Tree (AST) from the tokens.
    *   Define the AST node structures.

## Phase 2: Fundamental Language Features

This phase focuses on implementing the most basic features of the language.

1.  **Comments:**
    *   Support for single-line (`//`) and multi-line (`/* */`) comments.

2.  **Main Function:**
    *   Recognize `fn main(...)` as the entry point of the program.

3.  **Basic Data Types:**
    *   Implement `i32`, `f64`, `bool`, `char`.
    *   Type inference and explicit type annotations (`let a: i32 = 10;`).

4.  **Immutable Variables:**
    *   Implement `let` bindings for immutable variables.

5.  **Basic Operators:**
    *   Implement arithmetic (`+`, `-`, `*`, `/`, `%`), comparison (`==`, `!=`, `<`, `>`), and logical (`&&`, `||`, `!`) operators.

## Phase 3: Ownership and Memory Management (Core Feature)

This is a critical phase that implements the core memory safety features of Chtholly.

1.  **Ownership and Move Semantics:**
    *   Implement move semantics for variable assignments.
    *   When a variable is assigned to another, the ownership is transferred, and the original variable becomes inaccessible.
    *   This will be enforced at compile time.

2.  **Borrowing:**
    *   Implement immutable references (`&`). Multiple immutable references can exist simultaneously.
    *   Implement mutable references (`&mut`). Only one mutable reference can exist at a time.
    *   Enforce the rule that mutable and immutable references cannot coexist.

3.  **Lifetime Management:**
    *   Implement a borrow checker to ensure that references do not outlive the data they point to.
    *   Implement lifetime elision rules to simplify common cases.

4.  **Mutable Variables:**
    *   Implement the `mut` keyword to allow variables to be modified.

## Phase 4: Control Flow & Functions

This phase adds more complex control flow structures and functions.

1.  **Control Flow:**
    *   `if-else` expressions.
    *   `while`, `for`, and `do-while` loops.
    *   `switch-case` statements with `fallthrough`.

2.  **Functions:**
    *   `fn` definitions and calls.
    *   Function parameters and return types.
    *   Pass-by-value (move) and pass-by-reference (borrow).

3.  **Lambda Expressions:**
    *   Implement lambda expressions with capture clauses (`[]`, `[&]`, `[&mut]`).

## Phase 5: Composite Data Types

This phase introduces user-defined data types.

1.  **Structs:**
    *   `struct` definitions with named fields.
    *   Struct instantiation and field access.
    *   Member functions with `self` receiver.

2.  **Classes:**
    *   `class` definitions.
    *   Member variables (`let`, `let mut`).
    *   Constructors and destructors.
    *   Access modifiers (`public`, `private`).

3.  **Enums:**
    *   `enum` definitions with variants.
    *   Enums with associated data (`RGB(r, g, b)`).
    *   Pattern matching on enums.

4.  **Arrays:**
    *   Static arrays (`i32[4]`).
    *   Dynamic arrays (`i32[]`).

## Phase 6: Advanced Features

This phase adds features for code organization and advanced programming.

1.  **Modules:**
    *   `import` statements to load code from other files.
    *   `package` declaration.
    *   Module aliasing (`import "..." as ...`).

2.  **Generics:**
    *   Generic functions (`fn add<T>(a: T, b: T): T`).
    *   Generic structs and classes.

3.  **Error Handling:**
    *   `Result<T, E>` type.
    *   `?` operator for error propagation.

4.  **Optional Type:**
    *   `optional<T>` type and its methods (`unwrap`, `unwrap_or`).

## Phase 7: Compile-Time System (Core Feature)

This phase implements the compile-time programming capabilities of Chtholly.

1.  **`const` Constants:**
    *   Define and evaluate `const` values at compile time.

2.  **`const fn`:**
    *   Define and evaluate `const fn` at compile time.
    *   Enforce restrictions on `const fn` (e.g., no side effects).

3.  **`const struct`:**
    *   Implement `const struct` for compile-time data structures.

4.  **Compile-Time Evaluation:**
    *   Implement a compile-time interpreter to evaluate `const` expressions.
    *   Compile-time error handling.

## Phase 8: Standard Library & Tooling

The final phase focuses on building out the ecosystem around the language.

1.  **Standard Library:**
    *   Develop a basic standard library with modules for I/O, strings, collections, etc.

2.  **Build System and Package Manager:**
    *   Design and implement a build system and package manager for Chtholly projects.

3.  **Tooling:**
    *   Develop a code formatter (`chtholly fmt`).
    *   Develop a linter (`chtholly lint`).
