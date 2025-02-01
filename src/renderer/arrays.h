#pragma once

#include <glad/glad.h>
#include <vector>
#include <stdexcept>

#include "buffers.h"

namespace render {
	struct VertexAttribute {
		unsigned int type;
		unsigned int count;
		unsigned char normalized;

		static unsigned int GetSizeOfType(unsigned int type) {
			switch (type) {
			case GL_FLOAT: return sizeof(float);
			case GL_UNSIGNED_INT: return sizeof(unsigned int);
			case GL_UNSIGNED_BYTE: return sizeof(unsigned char);
			}

			return 0;
		}
	};

	class VertexBufferLayout {
	private:

		std::vector<VertexAttribute> m_Attributes;
		unsigned int m_Stride;

	public:
		VertexBufferLayout() : m_Stride(0) {}

		template <typename T>
		void Push(unsigned int count) {
			throw std::runtime_error("unsupported vertex attribute type pushed");
		}

		inline const std::vector<VertexAttribute>& GetElements() const { return m_Attributes; }
		inline unsigned int GetStride() const { return m_Stride; }
	};

	template<>
	void VertexBufferLayout::Push<float>(unsigned int count);

	template<>
	void VertexBufferLayout::Push<int>(unsigned int count);

	template<>
	void VertexBufferLayout::Push<unsigned int>(unsigned int count);

	template<>
	void VertexBufferLayout::Push<unsigned char>(unsigned int count);

	class VertexArray {
	private:
		unsigned int m_RendererID;
	public:
		VertexArray();
		~VertexArray();

		/* Disable copying */
		VertexArray(const VertexArray&) = delete;
		VertexArray& operator=(const VertexArray&) = delete;

		/* Enable moving */
		VertexArray(VertexArray&& other) noexcept;
		VertexArray& operator=(VertexArray&& other) noexcept;

		/* Bind and unbind */
		void Bind() const;
		void Unbind() const;

		/* Add buffer */
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) const {
			/* Bind vao and vbo */
			Bind();
			vb.Bind();

			/* Set vertex attributes */
			const auto& elements = layout.GetElements();
			size_t offset = 0;

			for (unsigned int i = 0; i < elements.size(); i++) {
				const auto& element = elements[i];
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
				offset += element.count * VertexAttribute::GetSizeOfType(element.type);
			}
		}
	};
};