#include "world.h"

#include "renderer/arrays.h"

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
		return { terrain->GetTextureCoords(53, atlasCount), defaultColor };
	case BlockType::COBBLESTONE:
		return { terrain->GetTextureCoords(16, atlasCount), defaultColor };
	case BlockType::BEDROCK:
		return { terrain->GetTextureCoords(17, atlasCount), defaultColor };
	default:
		return { terrain->GetTextureCoords(31, atlasCount), defaultColor };
	}
}

bool InChunkBounds(int x, int y, int z, int width, int height, int depth) {
	return x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth;
}

size_t GetBlockIndex(int x, int y, int z, int width, int height, int depth) {
	return static_cast<size_t>(x) + (static_cast<size_t>(y) * width) + (static_cast<size_t>(z) * width * height);
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
	glm::vec3 inChunk = glm::vec3(chunk.x, 0, chunk.y);

	for (int y = 0; y < height; y++) {
		for (int z = 0; z < depth; z++) {
			for (int x = 0; x < width; x++) {
				/* Calculate index */
				size_t index = GetBlockIndex(x, y, z, width, height, depth);

				/* Get block type */
				BlockType type = blocks[index];
				if (type == BlockType::AIR) {
					continue;
				}

				/* Check for faces */
				for (int direction = 0; direction < 6; direction++) {
					BlockFace face = static_cast<BlockFace>(direction);

					int nx = x + directions[direction][0];
					int ny = y + directions[direction][1];
					int nz = z + directions[direction][2];

					if (!InChunkBounds(nx, ny, nz, width, height, depth)) {
						continue;
					}

					size_t neighborIndex = GetBlockIndex(nx, ny, nz, width, height, depth);
					if (blocks[neighborIndex] == BlockType::AIR) {
						/* Get block texture */
						BlockTexture texture = GetBlockTexture(terrain, type, face);

						glm::vec3 position = glm::vec3(x, y, z);
						glm::vec3 normal = glm::vec3(directions[direction][0], directions[direction][1], directions[direction][2]);
						glm::vec3 color = texture.color;

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

						/* Add indices */
						unsigned int base = vertices.size() - 4;
						indices.push_back(base + 0);
						indices.push_back(base + 1);
						indices.push_back(base + 2);
						indices.push_back(base + 2);
						indices.push_back(base + 3);
						indices.push_back(base + 0);
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

	/* Create mesh */
	return render::Mesh(layout, vertices, indices, { terrain });
}