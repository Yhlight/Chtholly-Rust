add_library(Chtholly-lib parser.cpp lexer.cpp)
target_include_directories(Chtholly-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
