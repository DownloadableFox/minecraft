#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define WIDTH 960
#define HEIGHT 540

void glfw_callback(int error, const char* description) { 
    std::cerr << "GLFW Error: " << description << std::endl;
}

int main(int argc, char* argv[]) {
    /* Set error callback */
    glfwSetErrorCallback(glfw_callback);

    /* Initialize GLFW */
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    /* Hints */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create window */
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Minecraft", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLEW */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Print OpenGL version */
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    /* Set viewport */
    glViewport(0, 0, WIDTH, HEIGHT);

    /* Enable depth testing */
    glEnable(GL_DEPTH_TEST);

    /* Enable face culling */
    glEnable(GL_CULL_FACE);

    /* Enable blending */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    

    /* Main loop */
    while (!glfwWindowShouldClose(window)) {
        /* Poll events */
        glfwPollEvents();

        /* Clear the screen */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Swap buffers */
        glfwSwapBuffers(window);
    }

    /* Terminate GLFW */
    glfwTerminate();
}
