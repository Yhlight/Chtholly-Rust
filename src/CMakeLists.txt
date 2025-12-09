add_library(chtholly_lib Lexer.cpp Parser.cpp)

target_include_directories(chtholly_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(chtholly main.cpp)
target_link_libraries(chtholly chtholly_lib)
