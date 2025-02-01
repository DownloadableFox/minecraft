#include "generation.h"

#include <iostream>
#include <unordered_map>

std::vector<BlockType> FlatWorldGenerator(glm::ivec2 chunk, int width, int height, int depth) {
    static std::vector<BlockType> blocks(width * height * depth, BlockType::AIR);

    /* Only generate once  */
    static bool cached = false;
    if (cached) {
        return blocks;
    }

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            for (int z = 0; z < depth; z++) {
                if (y == 0) {
                    blocks[GetBlockIndex(x, y, z, width, height, depth)] = BlockType::BEDROCK;
                } else if (y < 4) {
                    blocks[GetBlockIndex(x, y, z, width, height, depth)] = BlockType::STONE;
                } else if (y < 8) {
                    blocks[GetBlockIndex(x, y, z, width, height, depth)] = BlockType::DIRT;
                } else if (y < 9) {
                    blocks[GetBlockIndex(x, y, z, width, height, depth)] = BlockType::GRASS;
                }
            }
        }
    }

    cached = true;
    return blocks;
}

std::vector<BlockType> BlockTestWorldGenerator(glm::ivec2 chunk, int width, int height, int depth) {
    static std::unordered_map<BlockType, std::vector<BlockType>> block_map;

    /* Create block type */
    BlockType block = static_cast<BlockType>((static_cast<int>(BlockType::BEDROCK) * 1000 + chunk.x + chunk.y) % (static_cast<int>(BlockType::BEDROCK) + 1) + 1);

    /* Cache the value */
    if (!block_map.count(block)) {
        std::vector<BlockType> blocks(width * height * depth, BlockType::AIR);

        for (int x = 2; x < width - 2; x++) {
            for (int y = 0; y < 5; y++) {
                for (int z = 2; z < depth - 2; z++) {
                    blocks[GetBlockIndex(x, y, z, width, height, depth)] = block;
                }
            }
        }

        block_map[block] = std::move(blocks);
    }

    return block_map[block];
}

LuaWorldGenerator::LuaWorldGenerator(const std::filesystem::path& path) {
    L = luaL_newstate();
    luaL_openlibs(L);

    /* Register blocks */
    lua_newtable(L);
    for (int i = 0; i < static_cast<int>(BlockType::BEDROCK); i++) {
        lua_pushnumber(L, i);
        lua_pushstring(L, BlockTypeToString(static_cast<BlockType>(i)));
        lua_settable(L, -3);
    }
    lua_setglobal(L, "BlockType");

    /* Load files */
    if (luaL_dofile(L, path.string().c_str()) != LUA_OK) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
    }
}

LuaWorldGenerator::~LuaWorldGenerator() {
    if (L != nullptr) {
        lua_close(L);
    }
}

LuaWorldGenerator::LuaWorldGenerator(LuaWorldGenerator&& other) noexcept {
    L = other.L;
    other.L = nullptr;
}

LuaWorldGenerator& LuaWorldGenerator::operator=(LuaWorldGenerator&& other) noexcept {
    if (this != &other) {
        if (L != nullptr) {
            lua_close(L);
        }

        L = other.L;
        other.L = nullptr;
    }

    return *this;
}

std::vector<BlockType> LuaWorldGenerator::GetChunk(glm::ivec2 chunk, int width, int height, int depth) {
    static std::vector<BlockType> defaultChunk(width * height * depth, BlockType::AIR);

    /* Return void if lua was moved */
    if (L == nullptr) {
        return defaultChunk;
    }

    /* Get the function */
    if (lua_getglobal(L, "generate_chunk") != LUA_OK) {
        std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
        lua_pop(L, 1);
        return defaultChunk;
    }
    
    if (!lua_isfunction(L, -1)) {
        std::cerr << "Error: expected 'generate_chunk' to be a function, it's " << lua_typename(L, lua_type(L, -1)) << std::endl;
        lua_pop(L, 1);
        return defaultChunk;
    }

    return defaultChunk;
}