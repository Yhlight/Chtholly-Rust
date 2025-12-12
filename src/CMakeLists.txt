# src/CMakeLists.txt
add_executable(chtholly_compiler main.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs core support)

# Link against LLVM libraries
target_link_libraries(chtholly_compiler PRIVATE ${llvm_libs})
