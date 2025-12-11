# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is broken down into milestones, each focusing on a specific set of features. This is a living document and will be updated as the project progresses.

---

## Milestone 1: Foundational Language Features (In Progress)

This milestone focuses on setting up the basic infrastructure of the compiler.

- [ ] **Lexer:** Implement the tokenizer to convert Chtholly source code into a stream of tokens.
    - [ ] Comments (`//`, `/* */`)
    - [ ] Keywords (`fn`, `let`, `mut`, `if`, etc.)
    - [ ] Identifiers
    - [ ] Literals
        - [x] Integers
        - [ ] Floats
        - [ ] Strings
        - [ ] Chars
        - [x] Booleans
    - [ ] Operators
        - [x] Prefix Operators (`!`, `-`)
        - [x] Infix Operators
    - [ ] Punctuation
- [x] **Parser:** Implement the parser to build an Abstract Syntax Tree (AST) from the token stream.
    - [x] Basic AST structure
    - [ ] Parse main function (`fn main(...)`)
    - [x] Parse variable declarations (`let`, `let mut`)
- [ ] **Code Generation (LLVM IR):** Set up the basic infrastructure for generating LLVM IR from the AST.
    - [ ] LLVM context and module setup
    - [ ] JIT (Just-In-Time) compilation for running code directly
    - [ ] Basic `main` function generation
- [ ] **Basic Types:** Implement support for primitive data types.
    - [ ] `i32`, `f64`, `bool`, `char`
    - [ ] Type inference for `let` declarations
- [ ] **TDD Setup:** Establish the testing framework for all components.

---

## Milestone 2: Expressions and Operators

- [x] **Arithmetic Operators:** `+`, `-`, `*`, `/`, `%`
- [x] **Comparison Operators:** `==`, `!=`, `<`, `>`, `<=`, `>=`
- [ ] **Logical Operators:** `&&`, `||`, `!`
- [x] **Operator Precedence and Associativity:** Ensure expressions are evaluated correctly.
- [ ] **Type Checking:** Basic type checking for expressions.

---

## Milestone 3: Control Flow

- [ ] **`if-else` Statements**
- [ ] **`while` Loops**
- [ ] **`for` Loops** (C-style)
- [ ] **`do-while` Loops**
- [ ] **`break` and `continue`**
- [ ] **`switch-case` Statements**

---

## Milestone 4: Functions

- [ ] **Function Definitions and Calls**
- [ ] **Parameters and Return Values** (`void` support)
- [ ] **Scope Management:** Implement symbol tables for managing variables and functions.
- [ ] **Lambda Expressions**

---

## Milestone 5: Ownership and Borrowing

- [ ] **Move Semantics:** Implement ownership transfer.
- [ ] **Borrowing:**
    - [ ] Shared references (`&`)
    - [ ] Mutable references (`&mut`)
- [ ] **Compile-Time Checks:** Enforce borrowing rules at compile time.
- [ ] **Lifetimes:** Initial implementation of lifetime analysis (focusing on elision rules).

---

## Milestone 6: Compound Data Types

- [ ] **Structs:**
    - [ ] Definition and instantiation
    - [ ] Member access
    - [ ] Member functions
- [ ] **Classes (Initial):**
    - [ ] Definition and instantiation
    - [ ] Member variables (`let`, `let mut`)
    - [ ] Member access
- [ ] **Enums:**
    - [ ] Simple enums
    - [ ] Enums with associated data
    - [ ] Destructuring in `if` and `switch`
- [ ] **Arrays:**
    - [ ] Static arrays (`i32[4]`)
    - [ ] Dynamic arrays (`i32[]`)

---

## Milestone 7: Advanced Class Features

- [ ] **Constructors**
- [ ] **Destructors (`~`)**
- [ ] **Member Functions (`self`, `&self`, `&mut self`)**
- [ ] **Access Modifiers (`public`, `private`)**

---

## Milestone 8: Modules and Imports

- [ ] **`import "filepath.cns"`:** File-based modules.
- [ ] **`package` directive:** For namespacing.
- [ ] **`use` statement:** To bring symbols into the current scope.
- [ ] **Module Aliasing (`import "..." as ...`)**

---

## Milestone 9: Error Handling

- [ ] **`Result<T, E>` Enum:** Implement the standard `Result` type.
- [ ] **`?` Operator:** For error propagation.
- [ ] **Pattern Matching:** Enhance `switch-case` to fully support `Result` destructuring.

---

## Milestone 10: Generics

- [ ] **Generic Functions**
- [ ] **Generic Structs and Classes**
- [ ] **Type Inference for Generics**
- [ ] **Specialization**

---

## Milestone 11: Compile-Time Execution

- [ ] **`const` Constants**
- [ ] **`const fn` Functions**
- [ ] **Const Context:**
    - [ ] Implement checks for allowed/disallowed operations in `const fn`.
    - [ ] Compile-time evaluation of `const` expressions.
- [ ] **`const struct`**

---

## Milestone 12: Standard Library

- [ ] **`iostream`:** Basic input/output functions (`print`, `println`).
- [ ] **`string`:** String manipulation functions.
- [ ] **`optional`:** Implement the `optional<T>` type.
- [ ] **Basic File I/O**
