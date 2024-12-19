#include "Camera2.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

std::unique_ptr<Camera> Camera::Create(glm::vec3 position) {
	auto instance = std::unique_ptr<Camera>(new Camera());
	instance->Init(position);
	return std::move(instance);
}

void Camera::Init(glm::vec3 position) {
	m_position = position;
	m_orientation = glm::fquat(1, 0, 0, 0);
}

	/*
Camera::Camera() 
{
	m_position = position;
	m_orientation = glm::fquat(1, 0, 0, 0);
	m_fov = 50.0f;
	m_aspect = 4.0f/3.0f;
	m_nearPlane = 0.1f;
	m_farPlane = 100.0f;
}
	*/

const glm::vec3 Camera::getPosition() const {
	return m_position;
}

const glm::vec3 Camera::right() const {
	return glm::vec3(glm::row(rotation(), 0));
}

const glm::vec3 Camera::up() const {
	return glm::vec3(glm::row(rotation(), 1));
}

const glm::vec3 Camera::forward() const {
	return glm::vec3(glm::row(rotation(), 2));
}

void Camera::setPosition(const glm::vec3 & position) {
	m_position = position;
}

void Camera::move(const glm::vec3 & delta) {
	m_position += delta;
}

void Camera::moveLeftRight(float delta) {
	m_position -= delta * right();
}

void Camera::moveUpDown(float delta) {
	m_position -= delta * up();
}

void Camera::moveForwardBackward(float delta) {
	m_position -= delta * forward();
}

void Camera::rotate(float angle, glm::vec3 & axis) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, axis));
	
	m_orientation = m_orientation * rot;
}

void Camera::pitch(float angle) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, right()));

	m_orientation = m_orientation * rot;
}

void Camera::yaw(float angle) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, up()));

	m_orientation = m_orientation * rot;
}
void Camera::roll(float angle) {
	glm::fquat rot = glm::normalize(glm::angleAxis(angle, forward()));

	m_orientation = m_orientation * rot;
}
/*
glm::mat4 Camera::projection() const {
	return glm::perspective(m_fov, m_aspect, m_nearPlane, m_farPlane);
}
*/
glm::mat4 Camera::translation() const {
	glm::mat4 trans = glm::translate(glm::mat4(), -m_position);

	double dArray[16] = { 0.0 };
	const float* pSource = (const float*)glm::value_ptr(trans);
	for (int i = 0; i < 16; ++i)
		dArray[i] = pSource[i];
	SPDLOG_INFO("{} {} {} {}", dArray[0], dArray[1], dArray[2], dArray[3]);
	SPDLOG_INFO("{} {} {} {}", dArray[4], dArray[5], dArray[6], dArray[7]);
	SPDLOG_INFO("{} {} {} {}", dArray[8], dArray[9], dArray[10], dArray[11]);
	SPDLOG_INFO("{} {} {} {}", dArray[12], dArray[13], dArray[14], dArray[15]);

	return glm::translate(glm::mat4(), -m_position);
}

glm::mat4 Camera::rotation() const {
	glm::mat4 rot = glm::toMat4(m_orientation);
	/*
	double dArray[16] = { 0.0 };
	const float* pSource = (const float*)glm::value_ptr(rot);
	for (int i = 0; i < 16; ++i)
		dArray[i] = pSource[i];

	SPDLOG_INFO("{} {} {} {}", dArray[0], dArray[1], dArray[2], dArray[3]);
	SPDLOG_INFO("{} {} {} {}", dArray[4], dArray[5], dArray[6], dArray[7]);
	SPDLOG_INFO("{} {} {} {}", dArray[8], dArray[9], dArray[10], dArray[11]);
	SPDLOG_INFO("{} {} {} {}", dArray[12], dArray[13], dArray[14], dArray[15]);
	*/
	return glm::toMat4(m_orientation);
}

glm::mat4 Camera::view() const {
	return rotation();
		//* translation();
}

/*
glm::mat4 Camera::matrix() const {
	return projection() * view();
}
void Camera::setFieldOfView(float fieldOfView) {
	m_fov = fieldOfView;
}

void Camera::setAspectRatio(float aspectRatio) {
	m_aspect = aspectRatio;
}

void Camera::setNearFarPlanes(float near, float far) {
	m_nearPlane = near;
	m_farPlane = far;
}
*/