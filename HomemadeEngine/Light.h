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
	//��ġ, ���� ����
	glm::vec3 direction;
	glm::vec3 position;
	//���� ���� �Ķ����
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	//attenuation �Ķ����
	float constant;
	float linear;
	float quadratic;
	//spot light cutoff �Ķ����
	float cutoff;
	float cutoffOuter;
};
