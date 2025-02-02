#include "shaders.h"

#include <iostream>
#include <fstream>

namespace render {
    unsigned int compile_shader(const std::string& source, unsigned int type) {
        /* Create shader */
        unsigned int id = glCreateShader(type);

        /* Compile shader */
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        /* Check for errors */
        int result;
        glGetShaderiv(id, GL_COMPILE_STATUS, &result);

        /* Handle error */
        if (result == GL_FALSE) {
            /* Get error message */
            int length;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

            /* Allocate memory on stack */
			char* message = new char[length];
            glGetShaderInfoLog(id, length, &length, message);

            /* Print error message */
            std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
            std::cerr << message << std::endl;

			/* Clean up */
			delete[] message;
            glDeleteShader(id);

            return 0;
        }

        return id;
    }

	ShaderCollection::ShaderCollection() {}
	
	ShaderCollection::~ShaderCollection() {
		for (unsigned int shader : m_Shaders) {
			glDeleteShader(shader);
		}
	}

	ShaderCollection::ShaderCollection(ShaderCollection&& other) noexcept {
		m_Shaders = std::move(other.m_Shaders);
	}

	ShaderCollection& ShaderCollection::operator=(ShaderCollection&& other) noexcept {
		if (this != &other) {
			for (unsigned int shader : m_Shaders) {
				glDeleteShader(shader);
			}

			m_Shaders = std::move(other.m_Shaders);
		}

		return *this;
	}

	void ShaderCollection::AddShader(const std::filesystem::path& path, ShaderType type) {
		/* Read shader */
		std::string source;
		std::ifstream file(path);

		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				source += line + '\n';
			}

			file.close();
		} else {
			std::cerr << "Failed to open shader file: " << path << std::endl;
			return;
		}

		/* Compile shader */
		unsigned int shader = compile_shader(source, static_cast<unsigned int>(type));
		if (shader) {
			m_Shaders.push_back(shader);
		} else {
			std::cerr << "Failed to add shader to collection: " << path << std::endl;
		}
	}

	ShaderProgram::ShaderProgram(const ShaderCollection& collection) {
		/* Create program */
		m_RendererID = glCreateProgram();

		/* Attach shaders */
		for (unsigned int shader : collection.GetShaders()) {
			glAttachShader(m_RendererID, shader);
		}

		/* Link program */
		glLinkProgram(m_RendererID);

		/* Validate program */
		glValidateProgram(m_RendererID);

		/* Check for errors */
		int result;
		glGetProgramiv(m_RendererID, GL_LINK_STATUS, &result);

		/* Handle error */
		if (result == GL_FALSE) {
			/* Get error message */
			int length;
			glGetProgramiv(m_RendererID, GL_INFO_LOG_LENGTH, &length);

			/* Allocate memory on stack */
			char* message = new char[length];
			glGetProgramInfoLog(m_RendererID, length, &length, message);

			/* Print error message */
			std::cerr << "Failed to link shader program" << std::endl;
			std::cerr << message << std::endl;

			/* Clean up */
			delete[] message;
			glDeleteProgram(m_RendererID);
			
			/* Reset */
			m_RendererID = 0;
		}

		/* Use shader */
		Bind();
	}

	ShaderProgram::~ShaderProgram() {
		if (m_RendererID) {
			glDeleteProgram(m_RendererID);
		}
	}

	ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept {
		m_RendererID = other.m_RendererID;
	}

	ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept {
		if (this != &other) {
			if (m_RendererID) {
				glDeleteProgram(m_RendererID);
			}

			m_RendererID = other.m_RendererID;
		}

		return *this;
	}

	void ShaderProgram::Bind() const {
		if (m_RendererID) {
			glUseProgram(m_RendererID);
		} else {
			std::cerr << "Attempted to bind a moved shader program" << std::endl;
		}
	}

	void ShaderProgram::Unbind() const {
		glUseProgram(0);
	}

	int ShaderProgram::GetUniformLocation(const std::string& name) {
		if (m_UniformCache.find(name) != m_UniformCache.end()) {
			return m_UniformCache[name];
		}

		int location = glGetUniformLocation(m_RendererID, name.c_str());
		if (location == -1) {
			std::cerr << "Failed to get uniform location: " << name << std::endl;
		}
		
		m_UniformCache[name] = location;
		return location;
	}

	void ShaderProgram::SetUniform1iv(const std::string& name, int count, const int* value) {
		Bind();

		int location = GetUniformLocation(name);
		glUniform1iv(location, count, value);
	}


	void ShaderProgram::SetUniform2f(const std::string& name, float v0, float v1) {
		Bind();

		int location = GetUniformLocation(name);
		glUniform2f(location, v0, v1);
	}

	void ShaderProgram::SetUniformMat4f(const std::string& name, glm::mat4 matrix) {
		Bind();

		int location = GetUniformLocation(name);
		glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
	}
};