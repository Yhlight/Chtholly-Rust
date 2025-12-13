cmake_minimum_required(VERSION 3.16)
project(ChthollyLanguage CXX)

find_package(LLVM 18 REQUIRED CONFIG 
    COMPONENTS Core Support Demangle Analysis IRReader MCJIT 
)

add_subdirectory(src)
add_subdirectory(tests)
