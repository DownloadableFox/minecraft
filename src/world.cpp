#include "world.h"

#include <iostream>

#include "renderer/arrays.h"

const char* BlockTypeToString(BlockType type) {
	switch (type) {
	case BlockType::AIR:
		return "AIR";
	case BlockType::DIRT:
		return "DIRT";
	case BlockType::GRASS:
		return "GRASS";
	case BlockType::STONE:
		return "STONE";
	case BlockType::WOOD:
		return "WOOD";
	case BlockType::LEAVES:
		return "LEAVES";
	case BlockType::COBBLESTONE:
		return "COBBLESTONE";
	case BlockType::BEDROCK:
		return "BEDROCK";
	default:
		return "UNKNOWN";
	}
}

BlockTexture GetBlockTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face) {
	const glm::vec3 defaultColor = glm::vec3(1.0f, 1.0f, 1.0f);
	const int atlasCount = 16;

	switch (type) {
	case BlockType::GRASS:
		switch (face) {
		case BlockFace::TOP:
			return { terrain->GetTextureCoords(0, atlasCount), glm::vec3(0.7f, 1.0f, 0.4f)};
		case BlockFace::BOTTOM:
			return { terrain->GetTextureCoords(2, atlasCount), defaultColor};
		default:
			return { terrain->GetTextureCoords(3, atlasCount), defaultColor };
		}
	case BlockType::DIRT:
		return { terrain->GetTextureCoords(2, atlasCount), defaultColor };
	case BlockType::STONE:
		return { terrain->GetTextureCoords(1, atlasCount), defaultColor };
	case BlockType::WOOD:
		switch (face) {
		case BlockFace::TOP:
		case BlockFace::BOTTOM:
			return { terrain->GetTextureCoords(21, atlasCount), defaultColor };
		default:
			return { terrain->GetTextureCoords(20, atlasCount), defaultColor };
		}
	case BlockType::LEAVES:
		return { terrain->GetTextureCoords(53, atlasCount), glm::vec3(0.7f, 1.0f, 0.4f) };
	case BlockType::COBBLESTONE:
		return { terrain->GetTextureCoords(16, atlasCount), defaultColor };
	case BlockType::BEDROCK:
		return { terrain->GetTextureCoords(17, atlasCount), defaultColor };
	default:
		return { terrain->GetTextureCoords(31, atlasCount), defaultColor };
	}
}

std::optional<BlockTexture> GetBlockOverTexture(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face) {
	const glm::vec3 defaultColor = glm::vec3(1.0f, 1.0f, 1.0f);
	const int atlasCount = 16;

	switch (type) {
		case BlockType::GRASS:
			if (face != BlockFace::TOP && face != BlockFace::BOTTOM) {
				return BlockTexture{ terrain->GetTextureCoords(38, atlasCount), glm::vec3(1.0f, 0.4f, 0.7f) /* glm::vec3(0.7f, 1.0f, 0.4f) */ };
			}
	}
	
	return std::nullopt;
}

bool InChunkBounds(int x, int y, int z, int width, int height, int depth) {
	return x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth;
}

bool InChunkHeightBounds(int x, int y, int z, int width, int height, int depth) {
	return (z >= 0 && z < depth) && (x >= 0 && x < width);
}

size_t GetBlockIndex(int x, int y, int z, int width, int height, int depth) {
	return static_cast<size_t>(x) + (static_cast<size_t>(y) * width) + (static_cast<size_t>(z) * width * height);
}

BlockType GetBlockType(const std::vector<BlockType>& blocks, int x, int y, int z, int width, int height, int depth) {
	if (!InChunkBounds(x, y, z, width, height, depth)) {
		return BlockType::AIR;
	}

	size_t index = GetBlockIndex(x, y, z, width, height, depth);
	return blocks[index];
}

std::pair<std::vector<BlockVertex>, std::vector<unsigned int>> CreateBlockFace(std::shared_ptr<render::Texture> terrain, BlockType type, BlockFace face, glm::vec3 position, glm::vec3 normal) {
	std::vector<BlockVertex> vertices;
	std::vector<unsigned int> indices;

	/* Get block texture */
	BlockTexture texture = GetBlockTexture(terrain, type, face);
	std::optional<BlockTexture> overTexture = GetBlockOverTexture(terrain, type, face);

	{
		/* Get color */
		glm::vec3 color = texture.color;

		/* Get texture coordinates */
		glm::vec2 bottomLeft = glm::vec2(texture.coords.min_x, texture.coords.min_y);
		glm::vec2 bottomRight = glm::vec2(texture.coords.max_x, texture.coords.min_y);
		glm::vec2 topRight = glm::vec2(texture.coords.max_x, texture.coords.max_y);
		glm::vec2 topLeft = glm::vec2(texture.coords.min_x, texture.coords.max_y);

		/* Add vertices */
		switch (face) {
		case BlockFace::FRONT:
			vertices.push_back({ position + glm::vec3(0, 0, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(1, 0, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(1, 1, 1), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(0, 1, 1), normal, color, topLeft });
			break;
		case BlockFace::BACK:
			vertices.push_back({ position + glm::vec3(1, 0, 0), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(0, 0, 0), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(0, 1, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(1, 1, 0), normal, color, topLeft });
			break;
		case BlockFace::LEFT:
			vertices.push_back({ position + glm::vec3(0, 0, 0), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(0, 0, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(0, 1, 1), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(0, 1, 0), normal, color, topLeft });
			break;
		case BlockFace::RIGHT:
			vertices.push_back({ position + glm::vec3(1, 0, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(1, 0, 0), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(1, 1, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(1, 1, 1), normal, color, topLeft });
			break;
		case BlockFace::TOP:
			vertices.push_back({ position + glm::vec3(0, 1, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(1, 1, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(1, 1, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(0, 1, 0), normal, color, topLeft });
			break;
		case BlockFace::BOTTOM:
			vertices.push_back({ position + glm::vec3(1, 0, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(0, 0, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(0, 0, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(1, 0, 0), normal, color, topLeft });
			break;
		}

		/* Adding indices */
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(3);
		indices.push_back(0);
	}

	if (overTexture.has_value()) {
		BlockTexture texture = overTexture.value();

		/* Get color */
		glm::vec3 color = texture.color;

		/* Get texture coordinates */
		glm::vec2 bottomLeft = glm::vec2(texture.coords.min_x, texture.coords.min_y);
		glm::vec2 bottomRight = glm::vec2(texture.coords.max_x, texture.coords.min_y);
		glm::vec2 topRight = glm::vec2(texture.coords.max_x, texture.coords.max_y);
		glm::vec2 topLeft = glm::vec2(texture.coords.min_x, texture.coords.max_y);

		/* Add vertices */
		switch (face) {
		case BlockFace::FRONT:
			vertices.push_back({ position + glm::vec3(0, 0, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(1, 0, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(1, 1, 1), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(0, 1, 1), normal, color, topLeft });
			break;
		case BlockFace::BACK:
			vertices.push_back({ position + glm::vec3(1, 0, 0), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(0, 0, 0), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(0, 1, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(1, 1, 0), normal, color, topLeft });
			break;
		case BlockFace::LEFT:
			vertices.push_back({ position + glm::vec3(0, 0, 0), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(0, 0, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(0, 1, 1), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(0, 1, 0), normal, color, topLeft });
			break;
		case BlockFace::RIGHT:	
			vertices.push_back({ position + glm::vec3(1, 0, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(1, 0, 0), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(1, 1, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(1, 1, 1), normal, color, topLeft });
			break;
		case BlockFace::TOP:
			vertices.push_back({ position + glm::vec3(0, 1, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(1, 1, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(1, 1, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(0, 1, 0), normal, color, topLeft });
			break;
		case BlockFace::BOTTOM:
			vertices.push_back({ position + glm::vec3(1, 0, 1), normal, color, bottomLeft });
			vertices.push_back({ position + glm::vec3(0, 0, 1), normal, color, bottomRight });
			vertices.push_back({ position + glm::vec3(0, 0, 0), normal, color, topRight });
			vertices.push_back({ position + glm::vec3(1, 0, 0), normal, color, topLeft });
			break;
		}

		/* Adding indices */
		int offset = vertices.size() - 4;
		indices.push_back(0 + offset);
		indices.push_back(1 + offset);
		indices.push_back(2 + offset);
		indices.push_back(2 + offset);
		indices.push_back(3 + offset);
		indices.push_back(0 + offset);
	}
	
	return { std::move(vertices), std::move(indices) };
}

render::Mesh CreateChunkMesh(std::shared_ptr<render::Texture> terrain, const std::vector<BlockType>& blocks, glm::ivec2 chunk, int width, int height, int depth) {
	constexpr int directions[][3] = {
		{  0,  0,  1 },
		{  0,  0, -1 },
		{ -1,  0,  0 },
		{  1,  0,  0 },
		{  0,  1,  0 },
		{  0, -1,  0 },
	};
	
	std::vector<BlockVertex> vertices;
	std::vector<unsigned int> indices;
	glm::vec3 inChunk = glm::vec3(chunk.x * width, 0, chunk.y * depth);

	for (int y = 0; y < height; y++) {
		for (int z = 0; z < depth; z++) {
			for (int x = 0; x < width; x++) {
				/* Get block type */
				BlockType type = GetBlockType(blocks, x, y, z, width, height, depth);
				if (type == BlockType::AIR) {
					continue;
				}

				/* Check for faces */
				for (int direction = 0; direction < 6; direction++) {
					BlockFace face = static_cast<BlockFace>(direction);

					int nx = x + directions[direction][0];
					int ny = y + directions[direction][1];
					int nz = z + directions[direction][2];

					if (!InChunkHeightBounds(nx, ny, nz, width, height, depth)) {
						continue;
					}

					BlockType neighbor = GetBlockType(blocks, nx, ny, nz, width, height, depth);
					if (neighbor == BlockType::AIR) {
						glm::vec3 position = glm::vec3(x, y, z) + inChunk;
						glm::vec3 normal = glm::vec3(directions[direction][0], directions[direction][1], directions[direction][2]);

						auto [faceVertices, faceIndices] = CreateBlockFace(terrain, type, face, position, normal);
						for (const auto& vertex : faceVertices) {
							vertices.push_back(vertex);
						}

						int offset = vertices.size() - faceVertices.size();
						for (const auto& index : faceIndices) {
							indices.push_back(index + offset);
						}
					}
				}
			}
		}
	}

	/* Create layout */
	render::VertexBufferLayout layout;
	layout.Push<float>(3); // Position
	layout.Push<float>(3); // Normal
	layout.Push<float>(3); // Color
	layout.Push<float>(2); // Texcoord

	// std::cerr << "Creating mesh with " << vertices.size() << " vertices and " << indices.size() << " indices" << std::endl;

	/* Create mesh */
	return render::Mesh(layout, vertices, indices, { terrain });
}