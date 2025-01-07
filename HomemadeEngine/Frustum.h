#pragma once
#include "common.h"

struct OrthoProjInfo {
	float left;
	float right;
	float top;
	float bottom;
	float zNear;
	float zFar;
};

struct PerspectiveProjInfo {
	float fov;
	float width;
	float height;
	float zNear;
	float zFar;
};

class AABB {
public:
	float MinX = FLT_MAX;
	float MinY = FLT_MAX;
	float MinZ = FLT_MAX;

	float MaxX = FLT_MIN;
	float MaxY = FLT_MIN;
	float MaxZ = FLT_MIN;

public:
	AABB() {};
	~AABB() {};

	void Add(const glm::vec4& v) {
		MinX = std::min(MinX, v.x);
		MinY = std::min(MinY, v.y);
		MinZ = std::min(MinZ, v.z);

		MaxX = std::max(MaxX, v.x);
		MaxY = std::max(MaxY, v.y);
		MaxZ = std::max(MaxZ, v.z);
	}

	void UpdataOrthoInfo(OrthoProjInfo& info) {
		info.right = MaxX;
		info.left = MinX;
		info.bottom = MinY;
		info.top = MaxY;
		info.zNear = MinZ;
		info.zFar = MaxZ;
	}
};

class Frustum
{
public:
	glm::vec4 nearTopLeft{0.0f, 0.0f, 0.0f, 1.0f};
	glm::vec4 nearTopRight{ 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 nearBottomLeft{ 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 nearBottomRight{ 0.0f, 0.0f, 0.0f, 1.0f };

	glm::vec4 farTopLeft{ 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 farTopRight{ 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 farBottomLeft{ 0.0f, 0.0f, 0.0f, 1.0f };
	glm::vec4 farBottomRight{ 0.0f, 0.0f, 0.0f, 1.0f };

public:
	void CalcCorners(PerspectiveProjInfo info);
	void Transform(const glm::mat4& m);
	void CalcAABB(AABB& aabb);

	Frustum() {};
	~Frustum() {};
};

