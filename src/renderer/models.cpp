#include "models.h"

#include <glad/glad.h>

namespace render {
	Mesh::Mesh(
		const VertexBufferLayout& layout,
		const void* vertices,
		size_t vertices_size,
		const unsigned int* indices,
		size_t indices_count,
		const std::vector<std::shared_ptr<Texture>>& textures) :
		m_VAO(),
		m_VBO(vertices, vertices_size, BufferHint::STATIC_DRAW),
		m_EBO(indices, indices_count, BufferHint::STATIC_DRAW),
		m_Textures(textures)
	{
		/* Bind buffers to vertex array */
		m_VAO.Bind();
		m_VBO.Bind();
		m_EBO.Bind();

		/* Set vertex buffer layout */
		m_VAO.AddBuffer(m_VBO, layout);
	}


	Mesh::Mesh(Mesh&& other) noexcept :
		m_VAO(std::move(other.m_VAO)), 
		m_VBO(std::move(other.m_VBO)), 
		m_EBO(std::move(other.m_EBO)), 
		m_Textures(std::move(other.m_Textures)) {}

	Mesh& Mesh::operator=(Mesh&& other) noexcept {
		if (this != &other) {
			m_VAO = std::move(other.m_VAO);
			m_VBO = std::move(other.m_VBO);
			m_EBO = std::move(other.m_EBO);
			m_Textures = std::move(other.m_Textures);
		}

		return *this;
	}

	void Mesh::SetTextures(const std::vector<std::shared_ptr<Texture>>& textures) {
		m_Textures = textures;
	}

	void Mesh::Draw(ShaderProgram& program) const {
		const static int textures[] = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 
			9, 10, 11, 12, 13, 15, 16
		};

		/* Bind textures */
		for (size_t i = 0; i < m_Textures.size(); i++) {
			m_Textures[i]->Bind(static_cast<unsigned int>(i));
		}

		/* Bind VAO */
		m_VAO.Bind();

		/* Bind program */
		program.Bind();
		program.SetUniform1iv("u_Texture", m_Textures.size(), textures);

		/* Draw */
		glDrawElements(GL_TRIANGLES, m_EBO.GetCount(), GL_UNSIGNED_INT, nullptr);
	}
};