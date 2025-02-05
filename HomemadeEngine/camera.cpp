#include "camera.h"


std::unique_ptr<camera> camera::Create(glm::vec3 pos, glm::vec3 front, glm::vec3 up) {
	auto cameraInstance = std::unique_ptr<camera>(new camera(pos, front, up));
	return std::move(cameraInstance);
}

camera::camera(glm::vec3 cameraPos, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
	this->cameraPos = cameraPos;
	this->cameraFront = cameraTarget;
	this->cameraFoward = cameraTarget;
	this->cameraUp = cameraUp;
	cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
}


void camera::MovePosition_KeyEvent(GLFWwindow* window, float speed, float deltaTime) {
	if (!ButtonPressed) return;
	cameraSpeed = speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += speed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= speed * cameraFront;
	}

	glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += speed * cameraRight;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= speed * cameraRight;
	}

	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		cameraPos += speed * cameraUp;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cameraPos -= speed * cameraUp;
	}

	/*
	m_roll = 0;

	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		m_roll = -0.01;
	}
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		m_roll = 0.01;
	}
	*/
}

void camera::Rotation_MouseEvent(double mouse_xpos, double mouse_ypos, float rotationSpeed, float deltaTime) {
	if (!ButtonPressed) return;
	sensitivity = rotationSpeed * deltaTime;
	auto pos = glm::vec2((float)mouse_xpos, (float)mouse_ypos);
	auto deltaPos = pos - prePos;
	m_yaw -= (deltaPos.x * sensitivity);
	m_pitch -= (deltaPos.y * sensitivity);

	if (m_yaw < 0.0f)   m_yaw += 360.0f;
	if (m_yaw > 360.0f) m_yaw -= 360.0f;

	if (m_pitch > 89.0f)  m_pitch = 89.0f;
	if (m_pitch < -89.0f) m_pitch = -89.0f;

	cameraFront = 
		glm::rotate(glm::mat4(1.0f), glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::rotate(glm::mat4(1.0f), glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	cameraFoward = glm::rotate(glm::mat4(1.0f), glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

	prePos = pos;
}

void camera::updateFlightCamera()
{
	glm::quat qPitch = glm::angleAxis(m_pitch, cameraRight);
	glm::quat qYaw = glm::angleAxis(m_yaw, cameraUp);
	glm::quat qRoll = glm::angleAxis(m_roll, cameraFront);

	glm::quat rot = qPitch * qYaw * qRoll;
	rot = glm::normalize(rot);
	glm::quat conj = glm::conjugate(rot);

	// rotate the axis of rotation 
	// by the combined quaternion
	cameraRight = rot * cameraRight * conj;
	cameraUp = rot * cameraUp * conj;
	cameraFront = rot * cameraFront * conj;
}

glm::mat4 camera::getViewMatrix() {
	return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}