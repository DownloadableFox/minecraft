#pragma once

#include <filesystem>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace render {
	enum class ComponentType {
		RGB = GL_RGB,
		RGBA = GL_RGBA,
		DEPTH = GL_DEPTH_COMPONENT
	};

	enum class WrapMode {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
		CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
	};

	enum class FilterMode {
		NEAREST = GL_NEAREST,
		LINEAR = GL_LINEAR,
		NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
	};

	struct TextureCoords {
		float min_x, min_y, max_x, max_y;
	};

	class Texture {
	private:
		unsigned int m_RendererID;
		int m_Width, m_Height, m_Channels;
	public:
		Texture();
		Texture(int width, int height, const void* data, ComponentType type);
		Texture(const std::filesystem::path& path);
		~Texture();

		/* Disable copying */
		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		/* Enable moving */
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		/* Get renderer ID */
		inline unsigned int GetRendererID() const { return m_RendererID; }

		/* Bind and unbind */
		void Bind(unsigned int slot = 0) const;
		void Unbind() const;

		/* Set data */
		void SetData(const std::filesystem::path& path);
		void SetData(int width, int height, const void* data, ComponentType type);

		/* Wrap and filter modes */
		void SetWrapMode(WrapMode mode);
		void SetFilterMode(FilterMode min, FilterMode mag);

		/* Mipmaps */
		void GenerateMipmaps(unsigned int levels = 4);

		/* Atlas */
		TextureCoords GetTextureCoords(int index, int count) const;

		/* Getters */
		inline int GetWidth() const { return m_Width; }
		inline int GetHeight() const { return m_Height; }
		inline int GetChannels() const { return m_Channels; }
	};
};