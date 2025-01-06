#pragma once
#include "common.h"

struct LightOrthoProjInfo {
	
};

struct PerspectiveProjInfo {
	float fov;
	float width;
	float height;
	float zNear;
	float zFar;
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
	void Transform(glm::mat4& m);

	Frustum() {};
	~Frustum() {};
};

