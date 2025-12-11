# Chtholly Language Development RoadMap

This document outlines the development plan for the Chtholly programming language. The primary goal is to create a safe, performant, and ergonomic systems programming language by focusing on a strong ownership model, a compile-time borrow checker, and a powerful compile-time programming system.

## Phase 1: Core Compiler Infrastructure

This phase focuses on setting up the essential components of the compiler and the basic language features.

*   **1.1: Project Setup**
    *   [x] Initialize CMake project structure.
    *   [ ] Integrate LLVM libraries for backend code generation.
    *   [ ] Set up initial testing framework (e.g., Google Test) for TDD.

*   **1.2: Lexer (Tokenizer)**
    *   [ ] Implement tokenization for all keywords (`fn`, `let`, `mut`, `if`, `else`, `class`, `struct`, etc.).
    *   [ ] Implement tokenization for operators (`+`, `-`, `*`, `/`, `&`, `&&`, etc.).
    *   [ ] Implement tokenization for literals (integers, floats, strings, chars).
    *   [ ] Handle single-line (`//`) and multi-line (`/* */`) comments.
    *   [ ] Add comprehensive unit tests for the lexer.

*   **1.3: Parser (AST Generation)**
    *   [ ] Implement a recursive descent parser to build a well-defined Abstract Syntax Tree (AST).
    *   [ ] Define AST nodes for:
        *   `main` function structure.
        *   Variable declarations (`let`, `let mut`).
        *   Function definitions and calls.
        *   Basic expressions (binary, unary, literals).
    *   [ ] Implement basic error reporting for syntax errors with line and column numbers.
    *   [ ] Add unit tests for parsing valid and invalid syntax.

*   **1.4: Semantic Analysis & Type System**
    *   [ ] Implement a scoped symbol table for tracking variables, functions, and their types.
    *   [ ] Implement a type checker for primitive data types (`i8`-`i64`, `u8`-`u64`, `f32`, `f64`, `bool`, `char`, `void`).
    *   [ ] Enforce type compatibility rules for expressions and assignments.
    *   [ ] Support for `let` and `let mut` variable declarations, including type inference and explicit type annotations.
    *   [ ] Add semantic error reporting (e.g., undeclared variables, type mismatches).

*   **1.5: Code Generation (LLVM IR)**
    *   [ ] Implement a visitor pattern to traverse the AST and generate LLVM IR.
    *   [ ] Implement code generation for:
        *   `main` function.
        *   Basic arithmetic and logical operations.
        *   Variable storage (allocas).
        *   Simple function calls.
    *   [ ] Compile and execute a simple program that performs calculations and prints the result.
    *   [ ] Add end-to-end tests that compile and run a `.cns` file.

## Phase 2: Memory Safety - Ownership & Borrowing

This is the most critical phase of the project, focusing on the core memory safety features of Chtholly. A TDD approach is paramount here.

*   **2.1: Ownership System**
    *   [ ] Implement move semantics for variable assignments and function arguments for all non-`Copy` types.
    *   [ ] Enhance the semantic analyzer to track the ownership state (e.g., `Owned`, `Moved`) of each variable.
    *   [ ] Enforce the rule that a variable becomes invalid after its value has been moved, issuing a compile-time error on use-after-move.
    *   [ ] Differentiate between `Copy` types (e.g., `i32`) and `Move` types (e.g., heap-allocated objects like `string`). `Copy` types will not have their original variables invalidated.
    *   [ ] Add extensive unit tests for move semantics with various data types and scenarios.

*   **2.2: Borrowing & References**
    *   [ ] Add support for parsing immutable (`&`) and mutable (`&mut`) reference types in the parser.
    *   [ ] Implement the core borrow checking rules in the semantic analyzer:
        *   At any given time, you can have either one mutable reference OR any number of immutable references.
        *   References must always be valid.
    *   [ ] Generate clear, user-friendly error messages for borrow checking violations (e.g., "cannot borrow `x` as mutable because it is also borrowed as immutable").
    *   [ ] Add unit tests for all valid and invalid borrowing scenarios.

*   **2.3: Lifetime Analysis**
    *   [ ] Introduce lifetime analysis to prevent dangling references, starting with lexical lifetimes (lifetimes tied to scopes).
    *   [ ] Implement the "smart lifetime omission" rules described in `Chtholly.md` to automatically infer lifetimes in common cases.
    *   [ ] Add tests to ensure that references do not outlive the data they point to.

*   **2.4: Automatic Memory Management**
    *   [ ] Implement `Drop` semantics (destructors) for resource cleanup when a variable goes out of scope.
    *   [ ] Integrate `Drop` calls into the code generation phase, ensuring they are called exactly once when ownership is lost.
    *   [ ] Test that resources are correctly deallocated for complex ownership transfer scenarios.

## Phase 3: Advanced Language Features

This phase expands the language with more complex features to improve expressiveness and utility.

*   **3.1: Control Flow**
    *   [ ] Implement `if-else` expressions.
    *   [ ] Implement `while` and `for` loops.
    *   [ ] Implement `switch-case` with fallthrough control.

*   **3.2: Compound Data Types**
    *   [ ] Implement `struct` definitions and instantiation.
    *   [ ] Implement `class` definitions with member variables and methods.
    *   [ ] Implement constructors and destructors.

*   **3.3: Functions & Lambdas**
    *   [ ] Support for function overloading.
    *   [ ] Implement lambda expressions with captures.

*   **3.4: Modules & Packages**
    *   [ ] Implement the `import` and `use` statements.
    *   [ ] Develop the package management system.

## Phase 4: Compile-Time Programming System

This phase focuses on implementing Chtholly's compile-time execution features to adhere to the "zero-cost abstraction" and "runtime minimalism" principles.

*   **4.1: `const` Evaluation Engine**
    *   [ ] Design and implement a compile-time evaluation engine (e.g., a tree-walking interpreter) that operates on the AST.
    *   [ ] Ensure the engine can handle basic arithmetic, control flow, and function calls within a `const` context.

*   **4.2: `const` and `const fn`**
    *   [ ] Implement `const` variables, ensuring their values are computed at compile time and substituted as constants during code generation.
    *   [ ] Implement `const fn` for functions that can be executed at compile time.
    *   [ ] Enforce the strict rules of the `const` context, prohibiting side effects like I/O, memory allocation, or calls to non-`const` functions.
    *   [ ] Add comprehensive tests for valid compile-time computations and for triggering errors on invalid operations.

*   **4.3: `Const Struct`**
    *   [ ] Implement `const struct`s as compile-time-only data structures.
    *   [ ] Ensure their members are `const` and can be evaluated and used in a `const` context.
    *   [ ] Test the creation and use of `const struct`s in `const fn` and `const` variable definitions.

*   **4.4: Compile-Time Error Handling**
    *   [ ] Extend the `Result<T, E>` type to be fully usable within a `const` context.
    *   [ ] Implement compile-time error handling, where a `const fn` returning a `Result` must be handled.
    *   [ ] A `const` variable assigned an `Err` result must trigger a clear compilation error.

*   **4.5: Generics**
    *   [ ] Implement generic functions.
    *   [ ] Implement generic `struct`s and `class`es.
    *   [ ] Support for template specialization.

## Phase 5: Standard Library & Tooling

This final phase focuses on building a usable ecosystem around the Chtholly language.

*   **5.1: Standard Library**
    *   [ ] Develop a core library with basic data structures (e.g., `string`, `Array`).
    *   [ ] Mark parts of the standard library as `const` where applicable (e.g., math functions).
    *   [ ] Implement I/O and file system modules.
    *   [ ] Provide a testing framework within the standard library.

*   **5.2: Tooling**
    *   [ ] Create a command-line interface (CLI) for the compiler.
    *   [ ] Develop a code formatter and a language server for IDE integration.
