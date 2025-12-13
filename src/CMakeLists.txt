add_library(chtholly_lib STATIC lib.cpp Lexer.cpp Parser.cpp AST.cpp SemanticAnalyzer.cpp Type.cpp Token.cpp CodeGenerator.cpp)

target_include_directories(chtholly_lib PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(chtholly_lib PRIVATE ${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs Core Support Analysis IRReader MCJIT)

target_link_libraries(chtholly_lib PRIVATE 
    ${llvm_libs}
    c++
    c++abi
    pthread
)

add_executable(chtholly_compiler driver.cpp)
target_include_directories(chtholly_compiler PRIVATE ${LLVM_INCLUDE_DIRS})

target_link_libraries(chtholly_compiler 
    PRIVATE 
    chtholly_lib
)
