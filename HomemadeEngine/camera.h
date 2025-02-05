#pragma once

#include "common.h"

class camera
{
public:
	static std::unique_ptr<camera> Create(glm::vec3 pos, glm::vec3 front, glm::vec3 up);
	void MovePosition_KeyEvent(GLFWwindow* window, float speed, float deltaTime);
	void Rotation_MouseEvent(double mouse_xpos, double mouse_ypos, float rotationSpeed, float deltaTime);
	void SetMouseRotationButton(bool ButtonPressed) { this->ButtonPressed = ButtonPressed; }
	void SetMousePrevPos(double xpos, double ypos) { prePos = glm::vec2(xpos, ypos); }
	glm::vec3 getCameraPos() { return cameraPos; }
	float* CameraPosPtr() { return glm::value_ptr(cameraPos); }
	glm::vec3 getCameraTarget() { return cameraPos + cameraFront; }
	glm::vec3 getCameraUp() { return cameraUp; }
	glm::vec3 getCameraFront() { return cameraFront; }
	
	void updateFlightCamera(); //for flight camera

	glm::mat4 getViewMatrix();
	~camera() {};
private:
	camera(glm::vec3 cameraPos, glm::vec3 cameraFront, glm::vec3 cameraUp);

	//glfw window for keyboad event poll
	GLFWwindow* window{ nullptr };
	//camera variables
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraFoward;
	glm::vec3 cameraUp;
	glm::vec3 cameraRight;


	glm::vec2 prePos;

	float cameraSpeed = 0.0f;
	float sensitivity = 0.0f;
	float m_yaw = 0.0f, m_pitch = 0.0f;
	float m_roll = 0.0f;

	bool ButtonPressed = false;
	float deltaTime;
};

