# tests/CMakeLists.txt
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests TestRunner.cpp LexerTests.cpp ParserTests.cpp SemanticAnalyzerTests.cpp)
target_link_libraries(chtholly_tests PRIVATE gtest_main chtholly_lib)
target_include_directories(chtholly_tests PRIVATE ../src)

add_executable(chtholly_codegen_tests CodeGenTestRunner.cpp)
target_link_libraries(chtholly_codegen_tests PRIVATE chtholly_lib ${LLVM_LIBRARIES})
target_include_directories(chtholly_codegen_tests PRIVATE ../src ${LLVM_INCLUDE_DIRS})
