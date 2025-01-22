#include "arrays.h"

#include <iostream>

namespace render {
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_RendererID);
		glBindVertexArray(m_RendererID);
	}

	VertexArray::~VertexArray() {
		if (m_RendererID) {
			glDeleteVertexArrays(1, &m_RendererID);
		}
	}

	VertexArray::VertexArray(VertexArray&& other) noexcept {
		m_RendererID = other.m_RendererID;
		other.m_RendererID = 0;
	}

	VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
		if (this != &other) {
			if (m_RendererID) {
				glDeleteVertexArrays(1, &m_RendererID);
			}

			m_RendererID = other.m_RendererID;
			other.m_RendererID = 0;
		}

		return *this;
	}

	void VertexArray::Bind() const {
		if (m_RendererID) {
			glBindVertexArray(m_RendererID);
		} else {
			std::cerr << "Attempted to bind a moved vertex array" << std::endl;
		}
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}

	template <>
	void VertexBufferLayout::Push<float>(unsigned int count) {
		m_Attributes.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * sizeof(float);
	}

	template <>
	void VertexBufferLayout::Push<unsigned int>(unsigned int count) {
		m_Attributes.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * sizeof(unsigned int);
	}

	template <>
	void VertexBufferLayout::Push<unsigned char>(unsigned int count) {
		m_Attributes.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += count * sizeof(unsigned char);
	}
};