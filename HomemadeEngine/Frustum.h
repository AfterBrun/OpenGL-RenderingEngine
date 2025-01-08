#pragma once
#include "common.h"

#define VOLUME_BIAS 30.0f

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
	float MinX = std::numeric_limits<float>::max();
	float MinY = std::numeric_limits<float>::max();
	float MinZ = std::numeric_limits<float>::max();

	float MaxX = std::numeric_limits<float>::lowest();
	float MaxY = std::numeric_limits<float>::lowest();
	float MaxZ = std::numeric_limits<float>::lowest();

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
		info.right = MaxX + VOLUME_BIAS;
		info.left = MinX - VOLUME_BIAS;
		info.bottom = MinY - VOLUME_BIAS;
		info.top = MaxY + VOLUME_BIAS;
		info.zNear = -(MaxZ + VOLUME_BIAS);
		info.zFar = -MinZ + VOLUME_BIAS;
	}
};

class Frustum
{
public:
	glm::vec4 nearTopLeft{-1.0f, 1.0f, 1.0f, 1.0f};
	glm::vec4 nearTopRight{ 1.0f, 1.0f, 1.0f, 1.0f };
	glm::vec4 nearBottomLeft{ -1.0f, -1.0f, 1.0f, 1.0f };
	glm::vec4 nearBottomRight{ 1.0f, -1.0f, 1.0f, 1.0f };

	glm::vec4 farTopLeft{ -1.0f, 1.0f, -1.0f, 1.0f };
	glm::vec4 farTopRight{ 1.0f, 1.0f, -1.0f, 1.0f };
	glm::vec4 farBottomLeft{ -1.0f, -1.0f, -1.0f, 1.0f };
	glm::vec4 farBottomRight{ 1.0f, -1.0f, -1.0f, 1.0f };

public:
	void CalcCorners(PerspectiveProjInfo info);
	void Transform(const glm::mat4& m);
	void CalcAABB(AABB& aabb);
	void PrintFrustum();

	Frustum() {};
	~Frustum() {};
};

