# src/CMakeLists.txt
add_library(chtholly_lib lib.cpp Lexer.cpp Parser.cpp AST.cpp SemanticAnalyzer.cpp Type.cpp Token.cpp)

# Add LLVM include dirs and definitions
target_include_directories(chtholly_lib PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(chtholly_lib PRIVATE ${LLVM_DEFINITIONS})

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs core support analysis irreader mcjit)

# Link against LLVM libraries
target_link_libraries(chtholly_lib PRIVATE ${llvm_libs})

add_executable(chtholly_compiler driver.cpp)
target_link_libraries(chtholly_compiler PRIVATE chtholly_lib)
