#include "Frustum.h"

void Frustum::CalcCorners(PerspectiveProjInfo info)
{
	float AR = info.height / info.width;
	float tanHalfFov = std::tanf(glm::radians(info.fov / 2.0f));
	float nearZ = info.zNear;
	float nearX = nearZ * tanHalfFov;
	float nearY = nearZ * tanHalfFov * AR;

	nearTopLeft = glm::vec4(-nearX, nearY, -nearZ, 1.0f);
	nearBottomLeft = glm::vec4(-nearX, -nearY, -nearZ, 1.0f);
	nearTopRight = glm::vec4(nearX, nearY, -nearZ, 1.0f);
	nearBottomRight = glm::vec4(nearX, -nearY, -nearZ, 1.0f);

	float farZ = info.zFar;
	float farX = farZ * tanHalfFov;
	float farY = farZ * tanHalfFov * AR;

	farTopLeft = glm::vec4(-farX, farY, -farZ, 1.0f);
	farBottomLeft = glm::vec4(-farX, -farY, -farZ, 1.0f);
	farTopRight = glm::vec4(farX, farY, -farZ, 1.0f);
	farBottomRight = glm::vec4(farX, -farY, -farZ, 1.0f);
}

void Frustum::Transform(const glm::mat4& m)
{
	nearTopLeft = m * nearTopLeft;
	nearTopRight = m * nearTopRight;
	nearBottomLeft = m * nearBottomLeft;
	nearBottomRight = m * nearBottomRight;

	farTopLeft = m * farTopLeft;
	farTopRight = m * farTopRight;
	farBottomLeft = m * farBottomLeft;
	farBottomRight = m * farBottomRight;
}

void Frustum::CalcAABB(AABB& aabb)
{
	aabb.Add(nearTopLeft);
	aabb.Add(nearBottomLeft);
	aabb.Add(nearTopRight);
	aabb.Add(nearBottomRight);

	aabb.Add(farTopLeft);
	aabb.Add(farBottomLeft);
	aabb.Add(farTopRight);
	aabb.Add(farBottomRight);
}

void Frustum::PrintFrustum()
{
	SPDLOG_INFO("near top left {}, {}, {}", nearTopLeft.x, nearTopLeft.y, nearTopLeft.z);
	SPDLOG_INFO("near top right {}, {}, {}", nearTopRight.x, nearTopRight.y, nearTopRight.z);
	SPDLOG_INFO("near bottom left {}, {}, {}", nearBottomLeft.x, nearBottomLeft.y, nearBottomLeft.z);
	SPDLOG_INFO("near bottom right {}, {}, {}", nearBottomRight.x, nearBottomRight.y, nearBottomRight.z);
	SPDLOG_INFO("far top left {}, {}, {}", farTopLeft.x, farTopLeft.y, farTopLeft.z);
	SPDLOG_INFO("far top right {}, {}, {}", farTopRight.x, farTopRight.y, farTopRight.z);
	SPDLOG_INFO("far bottom left {}, {}, {}", farBottomLeft.x, farBottomLeft.y, farBottomLeft.z);
	SPDLOG_INFO("far bottom right {}, {}, {}", farBottomRight.x, farBottomRight.y, farBottomRight.z);
	std::cout << "\n";
}
