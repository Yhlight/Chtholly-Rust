# Chtholly Development Roadmap

## Phase 1: Core Language Features

- [x] **Lexical Analysis:** Implement a lexer to tokenize the source code.
- [x] **Parsing:** Implement a parser to build an Abstract Syntax Tree (AST) from the tokens.
- [x] **AST Definition:** Define the data structures for the AST.
- [x] **Basic Types:** Implement support for basic data types (`int`, `double`, `char`, `string`, `bool`).
- [x] **Variable Declarations:** Implement `let` and `mut` variable declarations.
- [x] **Operators:** Implement basic arithmetic and logical operators.
- [x] **Control Flow:** Implement `if`/`else` statements.
- [x] **Functions:** Implement function definitions and calls.

## Phase 2: Advanced Language Features

- [ ] **Ownership and Borrowing:** Implement the ownership and borrowing system.
- [ ] **Classes and Structs:** Implement class and struct definitions.
- [ ] **Methods:** Implement method calls.
- [ ] **Enums:** Implement enums.
- [x] **Arrays:** Implement static and dynamic arrays.
- [ ] **Loops:**
  - [x] `while`
  - [ ] `for`
  - [ ] `foreach`
  - [ ] `do-while`
- [ ] **Generics:** Implement generic functions and classes.
- [ ] **Modules:** Implement the module system with `import` and `use`.
- [ ] **Error Handling:** Implement `Result<T, E>` and the `?` operator.

## Phase 3: LLVM Backend

- [ ] **LLVM IR Generation:** Implement code generation to produce LLVM IR.
- [ ] **JIT Compilation:** Implement a Just-In-Time (JIT) compiler.
- [ ] **AOT Compilation:** Implement an Ahead-Of-Time (AOT) compiler to produce executables.

## Phase 4: Standard Library

- [ ] **IO:** Implement a basic I/O library.
- [ ] **Collections:** Implement common data structures like vectors and maps.
- [ ] **String Manipulation:** Implement a comprehensive string manipulation library.
