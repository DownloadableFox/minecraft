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
	int chunk_width;
	int chunk_height;
	int chunk_depth;
	int render_distance;
};

struct RaycastResult {
	glm::vec3 position;
	glm::vec3 normal;
	BlockType type;
};

class Chunk {
private:
    glm::ivec2 m_Position;
    render::Mesh m_Mesh;
    std::vector<BlockType> m_Blocks;

public:
    Chunk(glm::ivec2 position, const std::vector<BlockType>& blocks, render::Mesh&& mesh) :
		m_Position(position), m_Blocks(blocks), m_Mesh(std::move(mesh)) {}

	/* Delete copying */
	Chunk(const Chunk&) = delete;
	Chunk& operator=(const Chunk&) = delete;

	/* Allow moving */
	Chunk(Chunk&& other) noexcept : m_Position(other.m_Position), m_Mesh(std::move(other.m_Mesh)), m_Blocks(std::move(other.m_Blocks)) {}
	Chunk& operator=(Chunk&& other) noexcept {
		if (this != &other) {
			m_Position = other.m_Position;
			m_Mesh = std::move(other.m_Mesh);
			m_Blocks = std::move(other.m_Blocks);
		}

		return *this;
	}

    const glm::ivec2& GetPosition() const { return m_Position; }
    const render::Mesh& GetMesh() const { return m_Mesh; }
    const std::vector<BlockType>& GetBlocks() const { return m_Blocks; }

    void SetMesh(render::Mesh&& mesh) { m_Mesh = std::move(mesh); }
};

/* Chunk Generation */
using ChunkGeneratorFn = std::function<std::vector<BlockType>(glm::ivec2, int, int, int)>;

/* Chunk helpers */
BlockTexture GetBlockTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face);
std::optional<BlockTexture> GetBlockOverTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face);	

/* World Getters */
bool InChunkBounds(int x, int y, int z, int width, int height, int depth);
bool InChunkHeightBounds(int x, int y, int z, int width, int height, int depth);
size_t GetBlockIndex(int x, int y, int z, int width, int height, int depth);
BlockType GetBlockType(const std::vector<BlockType>& blocks, int x, int y, int z, int width, int height, int depth);
std::pair<glm::ivec2, glm::ivec3> GlobalToChunkPosition(glm::vec3 position, int width, int height, int depth);

/* Chunk rendering */
std::pair<std::vector<BlockVertex>, std::vector<unsigned int>> CreateBlockFace(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face, glm::vec3 position, glm::vec3 normal);
render::Mesh CreateChunkMesh(std::shared_ptr<render::Texture> terrain, const std::vector<BlockType>& blocks, glm::ivec2 chunk, int width, int height, int depth);

/* Raycast result */
bool Raycast(const WorldSettings& settings, const std::vector<std::tuple<glm::ivec2, Chunk>>& chunks, glm::vec3 position, glm::vec3 direction, float distance, RaycastResult& result);