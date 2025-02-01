#include <iostream>
#include <filesystem>
#include <algorithm>
#include <set>
#include <vector>

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

#include "camera.h"
#include "world.h"
#include "generation.h"

#define WIDTH 960
#define HEIGHT 540

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_HEIGHT = 16;
constexpr int RENDER_DISTANCE = 10;

/* Crosshair */
constexpr int CROSSHAIR_SIZE = 10;
constexpr int CROSSHAIR_X = WIDTH / 2 - CROSSHAIR_SIZE / 2;
constexpr int CROSSHAIR_Y = HEIGHT / 2 - CROSSHAIR_SIZE / 2;

const std::filesystem::path assets_path = std::filesystem::current_path() / "assets";
const std::filesystem::path shaders_path = assets_path / "shaders";
const std::filesystem::path textures_path = assets_path / "textures";
const std::filesystem::path scripts_path = assets_path / "scripts";

/* Create screen vertices */
struct ScreenVertex {
    glm::vec2 position;
    glm::vec2 tex_coords;
};

const ScreenVertex vertices[] = {
    { { -1.0f, -1.0f }, { 0.0f, 0.0f } },
    { {  1.0f, -1.0f }, { 1.0f, 0.0f } },
    { {  1.0f,  1.0f }, { 1.0f, 1.0f } },
    { { -1.0f,  1.0f }, { 0.0f, 1.0f } }
};

const unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};

void parseInputs(GLFWwindow* window, Camera& camera, float deltaTime) {
	float cameraSpeed = 0.025f * deltaTime;

    glm::vec3 front = glm::normalize(camera.GetFront() * glm::vec3(1.0f, 0.0f, 1.0f));
	glm::vec3 right = glm::normalize(camera.GetRight() * glm::vec3(1.0f, 0.0f, 1.0f));

    /* Enable polygons */
    static bool polygons, held;
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS && !held) {
        polygons = !polygons;
        held = true;

        if (polygons) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
    } else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
        held = false;
    }

    /* Add to camera speed */
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        cameraSpeed *= 2.0f;
    }

    /* Move position */
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.SetPosition(camera.GetPosition() + cameraSpeed * front);
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.SetPosition(camera.GetPosition() - cameraSpeed * front);
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.SetPosition(camera.GetPosition() - cameraSpeed * right);
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.SetPosition(camera.GetPosition() + cameraSpeed * right);
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.SetPosition(camera.GetPosition() + glm::vec3(0.0f, cameraSpeed, 0.0f));
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera.SetPosition(camera.GetPosition() - glm::vec3(0.0f, cameraSpeed, 0.0f));
	}

    /* Set rotation */
    const float sensitivity = 0.05f;
	static double last_x = WIDTH / 2.0;
	static double last_y = HEIGHT / 2.0;
	static double yaw = -90.0;
	static double pitch = 0.0;

	/* Get cursor position */
	double pos_x, pos_y;
	glfwGetCursorPos(window, &pos_x, &pos_y);

	/* Calculate offset */
	double x_offset = pos_x - last_x;
	double y_offset = last_y - pos_y;

	/* Update last position */
	last_x = pos_x;
	last_y = pos_y;

	/* Set yaw and pitch */
	yaw += x_offset * sensitivity;
	pitch += y_offset * sensitivity;

	/* Clamp pitch */
	pitch = glm::clamp(pitch, -89.0, 89.0);

	/* Set front */
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	camera.SetFront(glm::normalize(direction));
}

void glfwCallback(int error, const char* description) { 
    std::cerr << "GLFW Error: " << description << std::endl;
}

void updateChunks(ChunkGeneratorFn generator, std::shared_ptr<render::Texture> terrain, std::vector<std::tuple<glm::ivec2, render::Mesh>>& chunks, glm::vec3 player_position) {    
    /* Calculate chunk position */
    glm::ivec2 player_chunk = glm::ivec2(player_position.x, player_position.z) / 16;
 
    /* Add meshes that are needed */
    std::set<std::pair<int, int>> keep_alive;
    for (int offset_x = -RENDER_DISTANCE; offset_x < RENDER_DISTANCE; offset_x++) {
        for (int offset_z = -RENDER_DISTANCE; offset_z < RENDER_DISTANCE; offset_z++) {
            /* Calculate chunk position and distance */
            glm::ivec2 chunk = player_chunk + glm::ivec2(offset_x, offset_z);

            /* Skip if distance is too long */
            float distance = glm::sqrt(static_cast<double>(offset_x * offset_x + offset_z * offset_z));
            if (distance < RENDER_DISTANCE) {
                keep_alive.insert({ chunk.x, chunk.y });
            }
        }
    }

    /* Render new chunks */
    for (const auto& to_render : keep_alive) {
        /* Check if chunk is already in chunks list */
        bool is_cached = false;
        for (const auto& alive_chunk : chunks) {
            const auto& [position, _] = alive_chunk;

            if (to_render.first == position.x && to_render.second == position.y) {
                is_cached = true;
                break;
            }
        }

        /* If chunk isn't cached, render */
        if (!is_cached) {
            auto blocks = generator(glm::ivec2(to_render.first, to_render.second), CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE);
            auto mesh = CreateChunkMesh(terrain, blocks, glm::ivec2(to_render.first, to_render.second), CHUNK_SIZE, CHUNK_HEIGHT, CHUNK_SIZE);
            chunks.push_back(std::make_tuple(glm::ivec2(to_render.first, to_render.second), std::move(mesh)));
        }
    }

    /* Remove any chunks that shouldn't be alive */
    for (auto it = chunks.begin(); it != chunks.end(); it++) {
        const auto& [position, _] = *it;

        if (keep_alive.find({ position.x, position.y }) == keep_alive.end()) {
            chunks.erase(it);            
        }
    }
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

    /* Disable cursor */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    ShaderCollection world_collection;
    world_collection.AddShader(shaders_path / "world.vert", ShaderType::VERTEX);
    world_collection.AddShader(shaders_path / "world.frag", ShaderType::FRAGMENT);

    /* Create shader program */
    ShaderProgram world_program(world_collection);

    /* Create texture */
    std::shared_ptr<Texture> terrain = std::make_shared<Texture>(textures_path / "terrain.png");

    /* Camera */
    Camera camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    /* Set texture parameters */
    terrain->GenerateMipmaps();
    terrain->SetWrapMode(WrapMode::CLAMP_TO_EDGE);
    terrain->SetFilterMode(FilterMode::NEAREST_MIPMAP_LINEAR, FilterMode::NEAREST);

    /* Chunk array */
    std::vector<std::tuple<glm::ivec2, render::Mesh>> chunks;

    /* Chunks generator */    
    LuaWorldGenerator generator(scripts_path / "world.lua");
    auto chunk_generator = [&generator](glm::ivec2 chunk, int width, int height, int depth) {
        return generator.GetChunk(chunk, width, height, depth);
    };

    /* Main loop */
    std::chrono::time_point<std::chrono::high_resolution_clock> last_time = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::high_resolution_clock> current_time;
    float delta_time = 0.0;

    /* Crosshair */
    std::vector<unsigned char> crosshair(CROSSHAIR_SIZE * CROSSHAIR_SIZE * 4, 255);

    /* Handle mouse */
    while (!glfwWindowShouldClose(window)) {
        /* Update chunks */
        updateChunks(BlockTestWorldGenerator, terrain, chunks, camera.GetPosition());

        /* Poll events */
        glfwPollEvents();

        /* Calculate delta time in ms */
		current_time = std::chrono::high_resolution_clock::now();
		delta_time = std::chrono::duration<float, std::milli>(current_time - last_time).count();
		last_time = current_time;        

		/* Parse inputs */
		parseInputs(window, camera, delta_time);

        /* Set projection, view, model, and normal matrices */
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 250.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 normal = glm::transpose(glm::inverse(model));

        /* Set uniform */
        world_program.SetUniformMat4f("u_Projection", proj);
        world_program.SetUniformMat4f("u_View", view);
        world_program.SetUniformMat4f("u_Model", model);
        world_program.SetUniformMat4f("u_NormalMatrix", normal);

        /* Clear the screen */
        glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw */
        for (const auto& chunk : chunks) {
            const auto& [_, mesh] = chunk;
            mesh.Draw(world_program);
        }

        /* Obtain crosshair */
        glReadPixels(CROSSHAIR_X, CROSSHAIR_Y, CROSSHAIR_SIZE, CROSSHAIR_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, crosshair.data());

        /* Invert colors */
        for (size_t i = 0; i < crosshair.size(); i += 4) {
            crosshair[i] = 255 - crosshair[i];
            crosshair[i + 1] = 255 - crosshair[i + 1];
            crosshair[i + 2] = 255 - crosshair[i + 2];
        }

        /* Draw crosshair */
        glBegin(GL_QUADS);
        

        /* Swap buffers */
        glfwSwapBuffers(window);
    }

    /* Terminate GLFW */
    glfwTerminate();
}