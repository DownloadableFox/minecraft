cmake_minimum_required(VERSION 3.10)
project(minecraft VERSION 0.1 LANGUAGES C CXX)

# Set CXX standard to 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set C standard to 11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)
include_directories(src)

# Add executable
add_executable(minecraft 
    src/main.cpp
    src/world.cpp
    
    # Renderer
    src/renderer/buffers.cpp
    src/renderer/arrays.cpp
    src/renderer/shaders.cpp
    src/renderer/textures.cpp
    src/renderer/models.cpp
)

# Add compile flags
# if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
#     set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=address")
# endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

# Compile GLFW and set build to Windows
add_subdirectory(glfw)

# Compile GLAD
add_subdirectory(glad)

# Compile GLM
add_subdirectory(glm)

# Link libraries
target_link_libraries(minecraft glfw glad glm-header-only ${OPENGL_LIBRARIES})