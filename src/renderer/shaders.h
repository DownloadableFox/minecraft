#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace render {
	enum class ShaderType {
		VERTEX = GL_VERTEX_SHADER,
		FRAGMENT = GL_FRAGMENT_SHADER
	};

	class ShaderCollection {
	private:
		std::vector<unsigned int> m_Shaders;
	public:
		ShaderCollection();
		~ShaderCollection();

		/* Disable copying */
		ShaderCollection(const ShaderCollection&) = delete;
		ShaderCollection& operator=(const ShaderCollection&) = delete;

		/* Enable moving */
		ShaderCollection(ShaderCollection&&) noexcept;
		ShaderCollection& operator=(ShaderCollection&&) noexcept;

		/* Add shader */
		void AddShader(const std::filesystem::path& path, ShaderType type);

		/* Get shaders */
		inline const std::vector<unsigned int>& GetShaders() const { return m_Shaders; }
	};

	class ShaderProgram {
	private:
		unsigned int m_RendererID;
		std::unordered_map<std::string, int> m_UniformCache;
	public:
		ShaderProgram(const ShaderCollection& collection);
		~ShaderProgram();

		/* Disable copying */
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;

		/* Enable moving */
		ShaderProgram(ShaderProgram&&) noexcept;
		ShaderProgram& operator=(ShaderProgram&&) noexcept;

		/* Use program */
		void Bind() const;
		void Unbind() const;

		/* Uniform location */
		int GetUniformLocation(const std::string& name);

		/* Set uniform */
		void SetUniformMat4f(const std::string& name, glm::mat4 matrix);
	};
};