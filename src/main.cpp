#include <iostream>
#include <filesystem>

/* OpenGL */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/* GLM */
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* STB */
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "renderer/buffers.h"
#include "renderer/arrays.h"
#include "renderer/shaders.h"
#include "renderer/textures.h"
#include "renderer/models.h"

#define WIDTH 960
#define HEIGHT 540

const std::filesystem::path assets_path = std::filesystem::current_path() / "assets";
const std::filesystem::path shaders_path = assets_path / "shaders";
const std::filesystem::path textures_path = assets_path / "textures";

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texcoord;
};

void glfwCallback(int error, const char* description) { 
    std::cerr << "GLFW Error: " << description << std::endl;
}

int main(int argc, char* argv[]) {
    /* Set error callback */
    glfwSetErrorCallback(glfwCallback);

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

    /* Swap interval */
    glfwSwapInterval(1);

    /* Initialize GLEW */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Print OpenGL version */
	std::cout << "Working directory: " << std::filesystem::current_path() << std::endl;
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

    using namespace render;

    /* Create shader collection */
    ShaderCollection collection;
    collection.AddShader(shaders_path / "vertex.glsl", ShaderType::VERTEX);
    collection.AddShader(shaders_path / "fragment.glsl", ShaderType::FRAGMENT);

    /* Create shader program */
    ShaderProgram program(collection);

    /* Create texture */
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(textures_path / "terrain.png");

    /* Set texture parameters */
    texture->GenerateMipmaps();
    texture->SetWrapMode(WrapMode::CLAMP_TO_EDGE);
    texture->SetFilterMode(FilterMode::NEAREST_MIPMAP_NEAREST, FilterMode::NEAREST);

    /* Obtain texture coordinates */
	TextureCoords sides = texture->GetTextureCoords(3, 16);
	TextureCoords top = texture->GetTextureCoords(0, 16);
	TextureCoords bottom = texture->GetTextureCoords(2, 16);

    Vertex vertices[] = {
        /* Front */
        { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.min_y) },
        { glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.min_y) },
        { glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.max_y) },
        { glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.max_y) },

        /* Back */
        { glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.min_y) },
        { glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.min_y) },
        { glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.max_y) },
        { glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.max_y) },

        /* Left */
		{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.min_y) },
		{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.min_y) },
		{ glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.max_y) },
		{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.max_y) },

		/* Right */
		{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.min_y) },
		{ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.min_y) },
		{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.max_x, sides.max_y) },
		{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(sides.min_x, sides.max_y) },

		/* Top */
		{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.7f, 1.0f, 0.4f), glm::vec2(top.min_x, top.min_y) },
		{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.7f, 1.0f, 0.4f), glm::vec2(top.max_x, top.min_y) },
		{ glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.7f, 1.0f, 0.4f), glm::vec2(top.max_x, top.max_y) },
		{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.7f, 1.0f, 0.4f), glm::vec2(top.min_x, top.max_y) },

		/* Bottom */
		{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(bottom.min_x, bottom.min_y) },
		{ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(bottom.max_x, bottom.min_y) },
		{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(bottom.max_x, bottom.max_y) },
		{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(bottom.min_x, bottom.max_y) },
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
    };

    /* Create vertex array object */
    // VertexArray vao;

    /* Create vertex buffer object */
	// VertexBuffer vbo(vertices, sizeof(vertices));

    /* Create element buffer object */
	// ElementBuffer ebo(indices, sizeof(indices) / sizeof(unsigned int));

    /* Set vertex attributes */
    VertexBufferLayout layout;
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(3);
    layout.Push<float>(2);

    /* Bind layout to buffer */
    // vao.AddBuffer(vbo, layout);

	/* Mesh */
	Mesh mesh(layout, vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(unsigned int), { texture });

    /* Main loop */
    float angle = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        /* Increment angle */
        angle += 0.05f;
        angle = fmod(angle, 360.0f);

        /* Poll events */
        glfwPollEvents();

        /* Set projection, view, model, and normal matrices */
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 model = glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.0f, -5.5f)), angle, glm::vec3(1.0f, 0.0f, 1.0f));
        glm::mat4 normal = glm::transpose(glm::inverse(model));

        /* Set uniform */
		program.SetUniformMat4f("u_Projection", proj);
		program.SetUniformMat4f("u_View", view);
		program.SetUniformMat4f("u_Model", model);
		program.SetUniformMat4f("u_NormalMatrix", normal);

        /* Clear the screen */
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw */
		mesh.Draw(program);

        /* Swap buffers */
        glfwSwapBuffers(window);
    }

    /* Terminate GLFW */
    glfwTerminate();
}
