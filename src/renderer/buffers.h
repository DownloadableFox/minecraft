#pragma once

#include "textures.h"

#include <glad/glad.h>
#include <vector>

namespace render {
    enum class BufferHint {
        STATIC_DRAW = GL_STATIC_DRAW,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        STREAM_DRAW = GL_STREAM_DRAW
    };

    class VertexBuffer {
    private:
        unsigned int m_RendererID;
    public:
        template <typename T>
		VertexBuffer(const std::vector<T>& data, BufferHint hint = BufferHint::STATIC_DRAW) : VertexBuffer(data.data(), data.size() * sizeof(T), hint) {}

        VertexBuffer(const void* data, size_t size, BufferHint hint = BufferHint::STATIC_DRAW);
        VertexBuffer(BufferHint hint = BufferHint::STATIC_DRAW);
        ~VertexBuffer();

        /* Disable copying */
        VertexBuffer(const VertexBuffer&) = delete;
        VertexBuffer& operator=(const VertexBuffer&) = delete;

        /* Enable moving */
        VertexBuffer(VertexBuffer&& other) noexcept;
        VertexBuffer& operator=(VertexBuffer&& other) noexcept;

        /* Set data */
        template <typename T>
        void SetData(const std::vector<T>& data) const {
			SetData(data.data(), data.size() * sizeof(T));
        }

        void SetData(const void* data, size_t size) const;

        /* Bind and unbind */
        void Bind() const;
        void Unbind() const;
    };

    class ElementBuffer {
    private:
        unsigned int m_RendererID;
        size_t m_Count;
    public:
        ElementBuffer(const std::vector<unsigned int>& data, BufferHint hint = BufferHint::STATIC_DRAW);
        ElementBuffer(const unsigned int* data, size_t count, BufferHint hint = BufferHint::STATIC_DRAW);
        ElementBuffer(BufferHint hint = BufferHint::STATIC_DRAW);
        ~ElementBuffer();

        /* Disable copying */
        ElementBuffer(const ElementBuffer&) = delete;
        ElementBuffer& operator=(const ElementBuffer&) = delete;

        /* Enable moving */
        ElementBuffer(ElementBuffer&& other) noexcept;
        ElementBuffer& operator=(ElementBuffer&& other) noexcept;

        /* Set data */
        void SetData(const std::vector<unsigned int>& data);
        void SetData(const unsigned int* data, size_t count);

        /* Bind and unbind */
        void Bind() const;
        void Unbind() const;

        /* Get count */
        size_t GetCount() const;
    };

    class RenderBuffer {
    private:
        unsigned int m_RendererID;
        int m_Width, m_Height;

    public:
        RenderBuffer(int width, int height);
        ~RenderBuffer();

        /* Disable copying */
        RenderBuffer(const RenderBuffer&) = delete;
        RenderBuffer& operator=(const RenderBuffer&) = delete;

        /* Enable moving */
        RenderBuffer(RenderBuffer&& other) noexcept;
        RenderBuffer& operator=(RenderBuffer&& other) noexcept;

        /* Bind and unbind */
        void Bind() const;
        void Unbind() const;

        /* Get renderer ID */
        inline unsigned int GetRendererID() const { return m_RendererID; }
    };

    class FrameBuffer {
    private:
        unsigned int m_RendererID;

    public:
        FrameBuffer();
        ~FrameBuffer();

        /* Disable copying */
        FrameBuffer(const FrameBuffer&) = delete;
        FrameBuffer& operator=(const FrameBuffer&) = delete;

        /* Enable moving */
        FrameBuffer(FrameBuffer&& other) noexcept;
        FrameBuffer& operator=(FrameBuffer&& other) noexcept;

        /* Bind and unbind */
        void Bind() const;
        void Unbind() const;

        /* Check if complete */
        bool IsComplete() const;

        /* Set viewport */
        void SetViewport(int width, int height);

        /* Attach texture */
        void AttachTexture(std::shared_ptr<Texture> texture);

        /* Attach render buffer */
        void AttachBuffer(const RenderBuffer& buffer);
    };
};