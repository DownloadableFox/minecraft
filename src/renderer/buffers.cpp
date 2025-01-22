#include "buffers.h"

#include <iostream>

namespace render {
    VertexBuffer::VertexBuffer(const void* data, size_t size, BufferHint hint) {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, size, data, static_cast<GLenum>(hint));
    }

    VertexBuffer::VertexBuffer(BufferHint hint) {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, static_cast<GLenum>(hint));
    }

    VertexBuffer::~VertexBuffer() {
        if (m_RendererID) {
            glDeleteBuffers(1, &m_RendererID);
        }
    }

    VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept {
        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }

    VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
        if (this != &other) {
            if (m_RendererID) {
                glDeleteBuffers(1, &m_RendererID);
            }

            m_RendererID = other.m_RendererID;
            other.m_RendererID = 0;
        }

        return *this;
    }

    void VertexBuffer::SetData(const void* data, size_t size) const {
        Bind();
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    void VertexBuffer::Bind() const {
        if (m_RendererID) {
            glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
        }
        else {
            std::cerr << "Attempted to bind a moved vertex buffer" << std::endl;
        }
    }

    void VertexBuffer::Unbind() const {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    ElementBuffer::ElementBuffer(const std::vector<unsigned int>& data, BufferHint hint) : ElementBuffer(data.data(), data.size(), hint) {}

    ElementBuffer::ElementBuffer(const unsigned int* data, size_t count, BufferHint hint) : m_Count(count) {
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, static_cast<GLenum>(hint));
    }

    ElementBuffer::ElementBuffer(BufferHint hint) : m_Count(0) {        
        glGenBuffers(1, &m_RendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 0, nullptr, static_cast<GLenum>(hint));
    }

    ElementBuffer::~ElementBuffer() {
        if (m_RendererID) {
            glDeleteBuffers(1, &m_RendererID);
        }
    }

    ElementBuffer::ElementBuffer(ElementBuffer&& other) noexcept {
        m_RendererID = other.m_RendererID;
        m_Count = other.m_Count;
        other.m_RendererID = 0;
        other.m_Count = 0;
    }

    ElementBuffer& ElementBuffer::operator=(ElementBuffer&& other) noexcept {
        if (this != &other) {
            if (m_RendererID) {
                glDeleteBuffers(1, &m_RendererID);
            }

            m_RendererID = other.m_RendererID;
            m_Count = other.m_Count;
            other.m_RendererID = 0;
            other.m_Count = 0;
        }

        return *this;
    }

    void ElementBuffer::SetData(const std::vector<unsigned int>& data) {
        SetData(data.data(), data.size());
    }

    void ElementBuffer::SetData(const unsigned int* data, size_t count) {
        Bind();
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(unsigned int), data);
        m_Count = count;
    }

    void ElementBuffer::Bind() const {
        if (m_RendererID) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
        } else {
            std::cerr << "Attempted to bind a moved index buffer" << std::endl;
        }
    }

    void ElementBuffer::Unbind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    size_t ElementBuffer::GetCount() const {
        return m_Count;
    }
};