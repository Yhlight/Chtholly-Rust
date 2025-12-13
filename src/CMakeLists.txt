llvm_map_components_to_libnames(llvm_libs Core Support Analysis IRReader MCJIT)

add_library(chtholly_lib STATIC lib.cpp Lexer.cpp Parser.cpp AST.cpp SemanticAnalyzer.cpp Type.cpp Token.cpp CodeGenerator.cpp)

target_include_directories(chtholly_lib 
    PUBLIC 
    ${LLVM_INCLUDE_DIRS}
    PRIVATE
)
target_compile_definitions(chtholly_lib PUBLIC ${LLVM_DEFINITIONS})

target_link_libraries(chtholly_lib PRIVATE ${llvm_libs})

add_executable(chtholly_compiler driver.cpp)

target_link_libraries(chtholly_compiler PRIVATE chtholly_lib)
