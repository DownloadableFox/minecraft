cmake_minimum_required(VERSION 3.10)
project(minecraft VERSION 0.1 LANGUAGES C CXX)

add_executable(minecraft 
    src/main.cpp
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Compile GLFW and set build to Windows
add_subdirectory(glfw)

# Compile GLAD
add_subdirectory(glad)

# Compile GLM
add_subdirectory(glm)

target_link_libraries(minecraft glfw glad glm-header-only ${OPENGL_LIBRARIES})