#pragma once

#include <vector>
#include <memory>

#include "buffers.h"
#include "arrays.h"
#include "textures.h"
#include "shaders.h"

namespace render {
	class Mesh {
	private:
		VertexArray m_VAO;
		VertexBuffer m_VBO;
		ElementBuffer m_EBO;
		std::vector<std::shared_ptr<Texture>> m_Textures;

	public:
		template <typename T>
		Mesh(
			const VertexBufferLayout& layout,
			const std::vector<T>& vertices,
			const std::vector<unsigned int>& indices,
			const std::vector<std::shared_ptr<Texture>>& textures) :
			m_VAO(),
			m_VBO(vertices, BufferHint::STATIC_DRAW),
			m_EBO(indices, BufferHint::STATIC_DRAW),
			m_Textures(textures)
		{
			/* Bind buffers to vertex array */
			m_VAO.Bind();
			m_VBO.Bind();
			m_EBO.Bind();

			/* Set vertex buffer layout */
			m_VAO.AddBuffer(m_VBO, layout);
		}

		Mesh(
			const VertexBufferLayout& layout,
			const void* vertices,
			size_t vertices_size,
			const unsigned int* indices,
			size_t indices_count,
			const std::vector<std::shared_ptr<Texture>>& textures
		);

		/* Disable copying */
		Mesh(const Mesh&) = delete;
		Mesh& operator=(const Mesh&) = delete;

		/* Enable moving */
		Mesh(Mesh&& other) noexcept;
		Mesh& operator=(Mesh&& other) noexcept;

		void Draw(const ShaderProgram& program) const;
	};
};