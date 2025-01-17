cmake_minimum_required(VERSION 3.10)
project(glad VERSION 0.1 LANGUAGES C)

# Add glad source files
add_library(glad src/glad.c)

# Include directories
target_include_directories(glad PUBLIC include)

# Link libraries
target_link_libraries(glad ${CMAKE_DL_LIBS})