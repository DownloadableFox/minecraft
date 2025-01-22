#include "textures.h"

#include <iostream>

#include <stb/stb_image.h>

namespace render {
	Texture::Texture(const std::filesystem::path& path) : m_RendererID(0) {
		stbi_set_flip_vertically_on_load(true);

		/* Load texture */
		unsigned char* data = stbi_load(path.string().c_str(), &m_Width, &m_Height, &m_Channels, 0);
		if (!data) {
			std::cerr << "Failed to load texture: " << path << std::endl;
			return;
		}

		/* Generate texture */
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		/* Set texture parameters */
		GLenum format = GL_RGB;
		if (m_Channels == 4) {
			format = GL_RGBA;
		}

		/* Upload texture */
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);

		/* Generate mipmaps */
		stbi_image_free(data);
	}

	Texture::~Texture() {
		if (m_RendererID) {
			glDeleteTextures(1, &m_RendererID);
		}
	}

	Texture::Texture(Texture&& other) noexcept {
		m_RendererID = other.m_RendererID;
		m_Width = other.m_Width;
		m_Height = other.m_Height;
		m_Channels = other.m_Channels;
	}

	Texture& Texture::operator=(Texture&& other) noexcept {
		if (this != &other) {
			if (m_RendererID) {
				glDeleteTextures(1, &m_RendererID);
			}

			m_RendererID = other.m_RendererID;
			m_Width = other.m_Width;
			m_Height = other.m_Height;
			m_Channels = other.m_Channels;
		}

		return *this;
	}

	void Texture::Bind(unsigned int slot) const {
		if (slot >= 32) {
			std::cerr << "Texture slot out of range: " << slot << std::endl;
			return;
		}

		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
	}

	void Texture::Unbind() const {
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void Texture::SetWrapMode(WrapMode mode) {
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLenum>(mode));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLenum>(mode));
	}

	void Texture::SetFilterMode(FilterMode min, FilterMode mag) {
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(min));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mag));
	}

	void Texture::GenerateMipmaps() {
		glBindTexture(GL_TEXTURE_2D, m_RendererID);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	TextureCoords Texture::GetTextureCoords(int index, int count) const {
		/* Calculate texture coordinates */
		float x = static_cast<float>(index % count) / count;
		float y = static_cast<float>(index / count) / count;

		float width = 1.0f / count;
		float height = 1.0f / count;

		/* Invert y-axis */
		y = 1.0f - y - height;

		return { x, y, x + width, y + height };
	}
};