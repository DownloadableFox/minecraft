cmake_minimum_required(VERSION 3.13)
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

# Enable AddressSanitizer for the "sanitize" target
if (CMAKE_BUILD_TYPE STREQUAL "Sanitize")
    message(STATUS "Building with AddressSanitizer enabled")
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

# Set debug flags
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in debug mode")
    add_compile_options(-g)
endif()

# Add executable
add_executable(minecraft 
    src/main.cpp
    src/world.cpp
    src/generation.cpp
    
    # Renderer
    src/renderer/buffers.cpp
    src/renderer/arrays.cpp
    src/renderer/shaders.cpp
    src/renderer/textures.cpp
    src/renderer/models.cpp
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Detect if the system is Linux
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    message(STATUS "Building on Linux - Enabling X11 and disabling Wayland")

    # Disable Wayland and enable X11
    set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "Disable Wayland support" FORCE)
    set(GLFW_BUILD_X11 ON CACHE BOOL "Enable X11 support" FORCE)

    # Ensure X11 dependencies are installed
    find_package(X11 REQUIRED)
    if (NOT X11_FOUND)
        message(FATAL_ERROR "X11 libraries not found! Please install X11 development packages.")
    endif()
else()
    message(STATUS "Not building on Linux - Skipping X11/Wayland configuration")
endif()

# Compile GLFW and set build to Windows
add_subdirectory(glfw)

# Compile GLAD
add_subdirectory(glad)

# Compile GLM
add_subdirectory(glm)

# Add lua 
add_subdirectory(lua)

# Link libraries
target_link_libraries(minecraft glfw glad glm-header-only lua ${OPENGL_LIBRARIES})