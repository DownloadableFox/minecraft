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

    FrameBuffer::FrameBuffer() {
        glGenFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
    }

    FrameBuffer::~FrameBuffer() {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
        }
    }

    FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept {
        m_RendererID = other.m_RendererID;
        other.m_RendererID = 0;
    }

    FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
        if (this != &other) {
            if (m_RendererID) {
                glDeleteFramebuffers(1, &m_RendererID);
            }

            m_RendererID = other.m_RendererID;
            other.m_RendererID = 0;
        }

        return *this;
    }

    void FrameBuffer::Bind() const {
        if (m_RendererID) {
            glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        } else {
            std::cerr << "Attempted to bind a moved frame buffer" << std::endl;
        }
    }

    void FrameBuffer::Unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    bool FrameBuffer::IsComplete() const {
        this->Bind();
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }

    void FrameBuffer::SetViewport(int width, int height) {
        glViewport(0, 0, width, height);
    }

    void FrameBuffer::AttachTexture(std::shared_ptr<Texture> texture) {
        this->Bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture->GetRendererID(), 0);
    }

    void FrameBuffer::AttachBuffer(const RenderBuffer& buffer) {
        this->Bind();
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, buffer.GetRendererID());
    }

    RenderBuffer::RenderBuffer(int width, int height) {
        glGenRenderbuffers(1, &m_RendererID);
        glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }

    RenderBuffer::~RenderBuffer() {
        if (m_RendererID) {
            glDeleteRenderbuffers(1, &m_RendererID);
        }
    }

    RenderBuffer::RenderBuffer(RenderBuffer&& other) noexcept {
        m_RendererID = other.m_RendererID;
        m_Width = other.m_Width;
        m_Height = other.m_Height;
        other.m_RendererID = 0;
    }

    RenderBuffer& RenderBuffer::operator=(RenderBuffer&& other) noexcept {
        if (this != &other) {
            if (m_RendererID) {
                glDeleteRenderbuffers(1, &m_RendererID);
            }

            m_RendererID = other.m_RendererID;
            m_Width = other.m_Width;
            m_Height = other.m_Height;
            other.m_RendererID = 0;
        }

        return *this;
    }

    void RenderBuffer::Bind() const {
        if (m_RendererID) {
            glBindRenderbuffer(GL_RENDERBUFFER, m_RendererID);
        } else {
            std::cerr << "Attempted to bind a moved depth buffer" << std::endl;
        }
    }

    void RenderBuffer::Unbind() const {
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
};