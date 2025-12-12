# tests/CMakeLists.txt
enable_testing()

add_executable(chtholly_tests main.cpp)

# Link against the compiler library (once it's a library)
# For now, we'll just have a placeholder test

# find_package(GTest REQUIRED)
# target_link_libraries(chtholly_tests PRIVATE GTest::GTest GTest::Main)
