#pragma once

#include <vector>

#include <renderer/textures.h>
#include <renderer/models.h>

enum class BlockType {
	AIR,
	DIRT,
	GRASS,
	STONE,
	WOOD,
	LEAVES,
	COBBLESTONE,
	BEDROCK,
};

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

BlockTexture GetBlockTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face);

/* Chunk Generation */
bool InChunkBounds(int x, int y, int z, int width, int height, int depth);
size_t GetBlockIndex(int x, int y, int z, int width, int height, int depth);
render::Mesh CreateChunkMesh(std::shared_ptr<render::Texture> terrain, const std::vector<BlockType>& blocks, glm::ivec2 chunk, int width, int height, int depth);