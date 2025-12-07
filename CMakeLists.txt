cmake_minimum_required(VERSION 3.10)
project(Chtholly)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the src directory
add_subdirectory(src)

# Enable testing
enable_testing()

# Add the tests directory
add_subdirectory(tests)
