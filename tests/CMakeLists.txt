# tests/CMakeLists.txt
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_executable(chtholly_tests TestRunner.cpp LexerTests.cpp ParserTests.cpp)

target_link_libraries(chtholly_tests PRIVATE gtest_main chtholly_lib)

# Include the src directory for header files
target_include_directories(chtholly_tests PRIVATE ../src)
