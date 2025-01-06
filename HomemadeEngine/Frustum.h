#pragma once
#include "common.h"

struct LightOrthoProjInfo {

};

struct PerspectiveProjInfo {
	float fov;
	float width;
	float height;
};

class Frustum
{
public:
	glm::vec3 nearTopLeft;
	glm::vec3 nearTopRight;
	glm::vec3 nearBottomLeft;
	glm::vec3 nearBottemRight;

	glm::vec3 farTopLeft;
	glm::vec3 farTopRight;
	glm::vec3 farBottomLeft;
	glm::vec3 farBottemRight;

public:
	void CalcCorners(PerspectiveProjInfo info);

	Frustum() {};
	~Frustum() {};
};

