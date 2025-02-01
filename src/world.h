#pragma once

#include <vector>
#include <optional>
#include <functional>

#include <renderer/textures.h>
#include <renderer/buffers.h>
#include <renderer/models.h>

enum class BlockType {
	AIR = 0,
	DIRT,
	GRASS,
	STONE,
	WOOD,
	LEAVES,
	COBBLESTONE,
	BEDROCK,
};

const char* BlockTypeToString(BlockType type);

enum class BlockFace {
	FRONT = 0,
	BACK,
	LEFT,
	RIGHT,
	TOP,
	BOTTOM,
};

struct BlockTexture {
	render::TextureCoords coords;
	glm::vec3 color;
};

struct BlockVertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 color;
	glm::vec2 texcoord;
};

struct WorldSettings {
	int chunkWidth;
	int chunkHeight;
	int chunkDepth;
	int renderDistance;
};

BlockTexture GetBlockTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face);
std::optional<BlockTexture> GetBlockOverTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face);	

/* Chunk Generation */
using ChunkGeneratorFn = std::function<std::vector<BlockType>(glm::ivec2, int, int, int)>;
// typedef std::vector<BlockType> (*ChunkGeneratorFn) (glm::ivec2 chunk, int width, int height, int depth);

/* World Getters */
bool InChunkBounds(int x, int y, int z, int width, int height, int depth);
bool InChunkHeightBounds(int x, int y, int z, int width, int height, int depth);
size_t GetBlockIndex(int x, int y, int z, int width, int height, int depth);
BlockType GetBlockType(const std::vector<BlockType>& blocks, int x, int y, int z, int width, int height, int depth);

/* Chunk rendering */
std::pair<std::vector<BlockVertex>, std::vector<unsigned int>> CreateBlockFace(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face, glm::vec3 position, glm::vec3 normal);
render::Mesh CreateChunkMesh(std::shared_ptr<render::Texture> terrain, const std::vector<BlockType>& blocks, glm::ivec2 chunk, int width, int height, int depth);