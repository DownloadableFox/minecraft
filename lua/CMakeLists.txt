cmake_minimum_required(VERSION 3.10)
project(lua VERSION 0.1 LANGUAGES C)

# Locate files
file(GLOB_RECURSE LUA_SOURCE ./lua/*.c)
file(GLOB_RECURSE LUA_HEADER ./lua/*.h)

# Add glad source files
add_library(lua ${LUA_SOURCE} ${LUA_HEADER})

# Include directories
target_include_directories(lua PUBLIC .)

# Link libraries
target_link_libraries(lua ${CMAKE_DL_LIBS})