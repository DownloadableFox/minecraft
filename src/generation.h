#pragma once

#include <filesystem>

extern "C" {
    #include <lua/lua.hpp>
}

#include "world.h"

/* Flat world generation function */
std::vector<BlockType> FlatWorldGenerator(glm::ivec2 chunk, int width, int height, int depth);

/* Block test generation */
std::vector<BlockType> BlockTestWorldGenerator(glm::ivec2 chunk, int width, int height, int depth);

class LuaWorldGenerator {
private:
    lua_State* L;
public:
    LuaWorldGenerator(const std::filesystem::path& path);
    ~LuaWorldGenerator();

    /* Delete copying */
    LuaWorldGenerator(const LuaWorldGenerator&) = delete;
    LuaWorldGenerator& operator=(const LuaWorldGenerator&) = delete;

    /* Move constructor */
    LuaWorldGenerator(LuaWorldGenerator&& other) noexcept;
    LuaWorldGenerator& operator=(LuaWorldGenerator&& other) noexcept;

    /* Get the chunk generator function */
    std::vector<BlockType> GetChunk(glm::ivec2 chunk, int width, int height, int depth);
};