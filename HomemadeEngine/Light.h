#pragma once

#include "common.h"
#include "ShaderProgram.h"

class Light
{
public:
	static std::unique_ptr<Light> CreateDirectionalLight();
	static std::unique_ptr<Light> CreatePointLight();
	static std::unique_ptr<Light> CreateSpotLight();

	void SetUniforms(ShaderProgram* program);
	~Light() {};
private:
	Light();
	//위치, 방향 정보
	glm::vec3 direction;
	glm::vec3 position;
	//빛의 세기 파라미터
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	//attenuation 파라미터
	float constant;
	float linear;
	float quadratic;
	//spot light cutoff 파라미터
	float cutoff;
	float cutoffOuter;
};
