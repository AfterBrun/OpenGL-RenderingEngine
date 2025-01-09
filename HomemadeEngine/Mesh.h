#pragma once

#include "common.h"
#include "buffer.h"
#include "vertexLayout.h"
#include "texture.h"
#include "ShaderProgram.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoord;
	glm::vec3 Tangent;

	int boneIDs[MAX_BONE_INFLUENCE]; //�� ���ؽ��� ������ ��ĥ bone�� ID�� ��� �迭(�ִ� 4��)
	float weights[MAX_BONE_INFLUENCE]; //�� bone���� ���� ������ ��� �迭
};

class Mesh
{
public:
	static std::unique_ptr<Mesh> Create(std::vector<Vertex>& vertex, std::vector<uint32_t>& indices);
	static std::unique_ptr<Mesh> CreateBox();
	static std::unique_ptr<Mesh> CreatePlane();
	static void CalcTangent(std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
	const vertexLayout* GetVertexArrayObject() const { return m_vao.get(); }
	std::shared_ptr<buffer> GetVertexBuffer() const { return m_vbo; }
	std::shared_ptr<buffer> GetElementBuffer() const { return m_ebo; }
	uint32_t GetElemetCount() const { return m_indicesCount; }
	void Draw(const ShaderProgram* program);
	std::vector<std::unique_ptr<texture>>* GetMaterialArrayPtr() { return &m_materials; }
	~Mesh() {};
private:
	Mesh() {};
	void Init(std::vector<Vertex>& vertex, std::vector<uint32_t>& indices);
	
	uint32_t m_primitiveType{ GL_TRIANGLES };
	GLsizei m_indicesCount{ 0 };

	std::unique_ptr<vertexLayout> m_vao{ nullptr };
	std::shared_ptr<buffer> m_vbo{ nullptr };
	std::shared_ptr<buffer> m_ebo{ nullptr };
	std::vector<std::unique_ptr<texture>> m_materials;
};

