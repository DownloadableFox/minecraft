#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class Camera {
private:
	glm::vec3 m_Position;
	glm::vec3 m_Front;
	glm::vec3 m_Up;

public:
	Camera(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up) 
		: m_Position(position), m_Front(target), m_Up(up) {}

	/* Setters / Getters */
	inline void SetPosition(const glm::vec3& position) { m_Position = position; }
	inline void SetFront(const glm::vec3& target) { m_Front = target; }
	inline void SetUp(const glm::vec3& up) { m_Up = up; }
	inline const glm::vec3& GetPosition() const { return m_Position; }
	inline const glm::vec3& GetFront() const { return m_Front; }
	inline const glm::vec3& GetUp() const { return m_Up; }
	inline const glm::vec3 GetRight() const { return glm::normalize(glm::cross(m_Front, m_Up)); }

	inline glm::mat4 GetViewMatrix() const {
		return glm::lookAt(m_Position, m_Front + m_Position, m_Up);
	}
};
