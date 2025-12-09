# Chtholly Language Development Roadmap

This document outlines the development plan for the Chtholly programming language. The development is guided by Scrum and TDD principles, focusing on incremental implementation and stability.

## Phase 0: Project Setup & Foundation

-   [x] **Repository Initialization:**
    -   [x] Create `Chtholly.md` syntax specification.
    -   [x] Create `README.md` and `LICENSE`.
    -   [x] Configure `.gitignore`.
-   [x] **Roadmap Creation:**
    -   [x] Create this `RoadMap.md` file.
-   [ ] **Build System Setup:**
    -   [ ] Configure `CMake` for the project.
    -   [ ] Set up dependency management for LLVM 18.
-   [ ] **Initial Directory Structure:**
    -   [ ] Create `src/` for the compiler source code.
    -   [ ] Create `tests/` for unit and integration tests.
    -   [ ] Create `modules/` for the standard library.
-   [ ] **Continuous Integration:**
    -   [ ] Set up a CI pipeline (e.g., GitHub Actions) to build and test the compiler on every commit.

## Phase 1: Lexer, Parser, and AST

-   [ ] **Lexer (Tokenizer):**
    -   [ ] Implement the lexer to convert source code into a stream of tokens.
    -   [ ] Support for keywords, identifiers, literals (integers, floats, strings, chars), operators, and symbols.
    -   [ ] Handle comments (single-line and multi-line).
-   [ ] **AST (Abstract Syntax Tree):**
    -   [ ] Define the data structures for the AST nodes.
-   [ ] **Parser:**
    -   [ ] Implement a parser (e.g., recursive descent) that consumes tokens from the lexer and builds the AST.
    -   [ ] Initial parsing support for:
        -   [ ] `fn main()` function.
        -   [ ] Variable declarations (`let`, `let mut`).
        -   [ ] Basic data types (`i32`, `f64`, `bool`, `string`, `char`).
        -   [ ] Simple expressions and statements.

## Phase 2: Semantic Analysis & Type System

-   [ ] **Symbol Table:**
    -   [ ] Implement a symbol table to store information about identifiers (variables, functions, types) and their scopes.
-   [ ] **Semantic Analyzer:**
    -   [ ] Traverse the AST to perform semantic checks.
    -   [ ] Check for variable declaration and initialization.
    -   [ ] Resolve identifier lookups using the symbol table.
-   [ ] **Type System:**
    -   [ ] Implement type checking for expressions and statements.
    -   [ ] Implement basic type inference.
    -   [ ] Handle explicit type annotations.

## Phase 3: Code Generation (LLVM)

-   [ ] **LLVM Integration:**
    -   [ ] Link the compiler with the LLVM libraries.
    -   [ ] Set up the LLVM context, module, and IR builder.
-   [ ] **Code Generator:**
    -   [ ] Implement a code generator that traverses the AST and emits LLVM IR.
    -   [ ] Generate code for a simple "Hello, World!" program.
    -   [ ] Compile the generated LLVM IR into an executable binary.

## Phase 4: Core Language Features - Ownership and Borrowing

This is the most critical phase, defining the core value proposition of Chtholly.

-   [ ] **Ownership System:**
    -   [ ] **Move Semantics:** Implement move semantics for variable assignments and function call arguments. The original variable must be invalidated after a move.
    -   [ ] **Ownership Tracking:** The semantic analyzer must track the ownership of each value.
    -   [ ] **Drop Logic:** Implement destructors (`~ClassName`) and automatic calls to `drop` when a value's owner goes out of scope.
    -   [ ] **Copy Trait:** Implement the `Copy` trait for simple types (like integers) where assignment is a bitwise copy instead of a move.
-   [ ] **Borrowing and References:**
    -   [ ] **Shared References (`&`):** Implement immutable references.
    -   [ ] **Mutable References (`&mut`):** Implement mutable references.
    -   [ ] **Borrow Checker:** This is a key component of the semantic analyzer. It must enforce the core borrowing rules:
        -   At any given time, you can have either one mutable reference or any number of immutable references.
        -   References must always be valid.
-   [ ] **Lifetime Management:**
    -   [ ] **Lifetime Analysis:** Implement lifetime analysis to prevent dangling references.
    -   [ ] **Lifetime Elision:** Implement the smart lifetime elision rules described in `Chtholly.md` to minimize the need for explicit lifetime annotations.

## Phase 5: Expanding the Language

-   [ ] **Control Flow:**
    -   [ ] `if-else` expressions.
    -   [ ] `switch-case` with fallthrough control.
    -   [ ] Loops: `while`, `for`, `foreach`, `do-while`.
-   [ ] **Functions:**
    -   [ ] Function declarations, calls, parameters, and return types.
    -   [ ] Lambda expressions with captures.
-   [ ] **Data Structures:**
    -   [ ] `struct` definitions and instantiation (aggregate and designated initialization).
    -   [ ] `class` definitions:
        -   [ ] Member variables (`let`, `let mut`).
        -   [ ] Constructors and destructors.
        -   [ ] Member functions (with `self`, `&self`, `&mut self`).
        -   [ ] Access modifiers (`public`, `private`).
    -   [ ] Enums with associated data and automatic destructuring.
-   [ ] **Arrays:**
    -   [ ] Static arrays (`i32[4]`).
    -   [ ] Dynamic arrays (`i32[]`).
-   [ ] **Generics:**
    -   [ ] Generic functions.
    -   [ ] Generic classes and structs.
    -   [ ] Specialization and default generic parameters.

## Phase 6: Standard Library & Modules

-   [ ] **Module System:**
    -   [ ] Implement the `import` and `use` statements.
    -   [ ] Handle module resolution (file-based and standard library).
    -   [ ] Implement `package` scoping.
-   [ ] **Standard Library (`modules/`):**
    -   [ ] **`iostream`:** For basic input/output (`print`, `println`).
    -   [ ] **`optional`:** For safe handling of optional values.
    -   [ ] **`Result`:** For robust error handling.
    -   [ ] **`string`:** For dynamic string manipulation.
-   [ ] **Contracts (Traits):**
    -   [ ] Implement the `contract` and `require` keywords.
    -   [ ] Support for associated types (`type item`).
    -   [ ] Implement built-in contracts, especially for iterators (`iterator_move`, `iterator_let`, `iterator_mut`, etc.).

## Phase 7: Advanced Features & Tooling

-   [ ] **Error Handling:**
    -   [ ] Implement the `Result<T, E>` enum in the standard library.
    -   [ ] Implement the `?` operator for error propagation.
-   [ ] **Tooling:**
    -   [ ] **Package Manager:** A tool to manage dependencies and build projects.
    -   [ ] **Formatter:** An automated code formatter to ensure consistent style.
    -   [ ] **LSP (Language Server Protocol):** Support for IDE features like autocompletion, go-to-definition, and diagnostics.
