# Chtholly Language Development Roadmap

## Phase 1: Core Language and Compiler Infrastructure

*   **1.1 Project Setup:**
    *   [x] Initial repository setup.
    *   [x] Create `RoadMap.md`.
    *   [x] Set up `CMakeLists.txt` for the project.
    *   [x] Configure `.gitignore`.
    *   [x] Set up basic directory structure (`src`, `tests`, `docs`).
*   **1.2 Lexical Analysis (Lexer):**
    *   [x] Implement a lexer to tokenize the source code.
    *   [x] Support for all keywords, operators, and literals defined in `Chtholly.md`.
    *   [x] Unit tests for the lexer.
*   **1.3 Syntactic Analysis (Parser):**
    *   [x] Implement a parser to build an Abstract Syntax Tree (AST).
    *   [x] Support for basic language constructs (variables, functions, control flow).
    *   [x] Unit tests for the parser.
*   **1.4 Semantic Analysis:**
    *   [x] Implement a semantic analyzer to perform type checking.
    *   [x] Symbol table implementation.
    *   [x] Basic error reporting for semantic errors.
    *   [x] Unit tests for the semantic analyzer.
*   **1.5 Code Generation (LLVM):**
    *   [x] Set up LLVM backend.
    *   [x] Generate LLVM IR for basic language constructs.
    *   [x] Compile a simple "Hello, World!" program.
    *   [x] Unit tests for code generation.

## Phase 2: Ownership, Borrowing, and Lifetime Management

*   **2.1 Ownership System:**
    *   [ ] Implement move semantics for variable assignments and function calls.
    *   [ ] Implement the `Copy` trait for basic types.
    *   [ ] Implement the `Drop` trait and destructor calls.
    *   [ ] Unit tests for ownership rules.
*   **2.2 Borrowing and References:**
    *   [ ] Implement immutable (`&`) and mutable (`&mut`) borrows.
    *   [ ] Enforce the borrowing rules (one mutable or multiple immutable references).
    *   [ ] Unit tests for borrowing rules.
*   **2.3 Lifetime Analysis:**
    *   [ ] Implement lifetime elision rules.
    *   [ ] Implement lifetime analysis to prevent dangling references.
    *   [ ] Unit tests for lifetime analysis.

## Phase 3: Advanced Language Features

*   **3.1 Data Types:**
    *   [ ] Implement support for `struct`s and `class`es.
    *   [ ] Implement support for `enum`s.
    *   [ ] Implement support for arrays (static and dynamic).
*   **3.2 Functions and Generics:**
    *   [ ] Implement support for generic functions.
    *   [ ] Implement support for generic `struct`s and `class`es.
    *   [ ] Implement support for lambda expressions.
*   **3.3 Modules and Packages:**
    *   [ ] Implement the module system with `import` and `use` keywords.
    *   [ ] Implement the package system.
*   **3.4 Error Handling:**
    *   [ ] Implement the `Result<T, E>` type.
    *   [ ] Implement the `?` operator for error propagation.

## Phase 4: Compile-Time System

*   **4.1 `const` and `const fn`:**
    *   [ ] Implement support for `const` variables.
    *   [ ] Implement support for `const fn` functions.
    *   [ ] Implement compile-time evaluation of `const` expressions.
*   **4.2 `const struct`:**
    *   [ ] Implement support for `const struct`.
*   **4.3 Compile-Time Error Handling:**
    *   [ ] Implement compile-time `Result<T, E>` handling.

## Phase 5: Standard Library and Tooling

*   **5.1 Standard Library:**
    *   [ ] Develop a basic standard library with I/O, string manipulation, and collections.
*   **5.2 Build System:**
    *   [ ] Improve the CMake and Python build scripts.
*   **5.3 Documentation:**
    *   [ ] Write comprehensive documentation for the language and standard library.
