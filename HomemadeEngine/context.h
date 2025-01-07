#pragma once

#include "common.h"
#include "ShaderProgram.h"
#include "buffer.h"
#include "vertexLayout.h"
#include "image.h"
#include "texture.h"
#include "camera.h"
#include "imgui/imgui.h"
#include "Mesh.h"
#include "Model.h"
#include "FrameBuffer.h"
#include "ShadowMap.h"
#include "Terrain.h"


class context
{
public:
	static std::unique_ptr<context> Create();
	~context();
	void Render();
	void viewSizeChange(int width, int height);
	void keyEvent(GLFWwindow* window);
	void mouseCursorEvent(double xPos, double yPos);
	void mouseButtonEvent(int button, int action, double xpos, double ypos);
private:
	context() {};
	bool Init();

	// light uniform Seting func
	void SetLightUniform_Dir(const ShaderProgram* program, const glm::vec3& lightDir);
	void SetLightUniform_Point(const ShaderProgram* program);
	void SetLightUniform_Spot(const ShaderProgram* program, const glm::vec3& position, const glm::vec3& direction);

	void RenderScene(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view);
	void RenderSkyBox(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view);
	void RenderTerrain(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view);
	void RenderGrass(const ShaderProgram* program, const glm::mat4& projection, const glm::mat4& view);

	//camera
	glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 1.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	std::unique_ptr<camera> camera = nullptr;
	PerspectiveProjInfo perspectiveProjInfo;

	//program, vao, vbo, ebo
	std::unique_ptr<ShaderProgram> simpleProgram = nullptr;
	std::unique_ptr<ShaderProgram> postProgram = nullptr;
	std::unique_ptr<ShaderProgram> lightProgram = nullptr;
	std::unique_ptr<ShaderProgram> skyboxProgram = nullptr;
	std::unique_ptr<ShaderProgram> instancingProgram = nullptr;
	std::unique_ptr<ShaderProgram> terrainProgram = nullptr;
	std::unique_ptr<ShaderProgram> normalProgram = nullptr;


	std::unique_ptr<vertexLayout> instanceVao = nullptr;
	std::unique_ptr<buffer> instanceVbo = nullptr;
	std::unique_ptr<buffer> instanceEbo = nullptr;

	//frame buffer
	std::unique_ptr<FrameBuffer> m_frameBuffer = nullptr;
	//shadow map
	std::unique_ptr<ShadowMap> m_shadowMap = nullptr;
	//normal map
	std::unique_ptr<texture> m_brickDiffuseTexture = nullptr;
	std::unique_ptr<texture> m_brickNormalTexture = nullptr;

	//mesh
	std::unique_ptr<Mesh> m_box = nullptr;
	std::unique_ptr<Mesh> m_floor = nullptr;
	std::unique_ptr<Mesh> m_plane = nullptr;
	std::unique_ptr<Mesh> m_grass = nullptr;
	std::unique_ptr<Mesh> m_brickwall = nullptr;

	//terrain
	std::unique_ptr<Terrain> m_terrain = nullptr;

	//model
	std::unique_ptr<Model> m_backpack = nullptr;

	//texture
	std::unique_ptr<texture> textureBox1 = nullptr;
	std::unique_ptr<texture> textureBox2 = nullptr;
	std::unique_ptr<texture> textureBox2_specular = nullptr;
	std::unique_ptr<texture> m_grassTexture = nullptr;
	std::unique_ptr<CubeTexture> m_cubeTexture = nullptr;

	std::unique_ptr<CubeTexture> m_terrainTexture = nullptr;


	//view size
	int m_width;
	int m_height;

	//main loop
	glm::vec4 m_clearColor { glm::vec4(0.2f, 0.3f, 0.3f, 1.0f) };

	// light parameter
	struct Light {
		glm::vec3 position{ glm::vec3(10.0f, 10.0f, 10.0f) };
		glm::vec3 direction{ glm::vec3(-0.5f, -1.5f, -1.0f) };
		glm::vec3 ambient{ glm::vec3(0.1f, 0.1f, 0.1f) };
		glm::vec3 diffuse{ glm::vec3(0.5f, 0.5f, 0.5f) };
		glm::vec3 specular{ glm::vec3(1.0f, 1.0f, 1.0f) };
		glm::vec2 cutoff{ glm::vec2(50.0f, 5.0f) };
		bool blinn = true;
	};
	Light m_light;

	struct Terrain_Material {
		glm::vec3 ambient{ glm::vec3(0.1f, 0.1f, 0.1f) };
		glm::vec3 diffuse{ glm::vec3(0.5f, 0.5f, 0.5f) };
		glm::vec3 specular{ glm::vec3(0.5f, 0.5f, 0.5f) };
		bool blinn = true;
	};
	Terrain_Material m_terrain_material;

	// material parameter
	struct Material {
		glm::vec3 ambient{ glm::vec3(1.0f, 0.5f, 0.3f) };
		glm::vec3 diffuse{ glm::vec3(1.0f, 0.5f, 0.3f) };
		glm::vec3 specular{ glm::vec3(0.5f, 0.5f, 0.5f) };
		float shininess{ 32.0f };
	};
	Material m_material;

	//gamma
	float m_gamma{ 1.0f };

	//delta time
	float deltaTime;

	std::vector<glm::vec3> m_grassPos;

};

