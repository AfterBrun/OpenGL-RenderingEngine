#pragma once

#include "common.h"
#include "image.h"
#include "texture.h"
#include "ShaderProgram.h"

struct Vertices {
	glm::vec3 pos;
	glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec2 texCoords;
	glm::vec3 tangent = glm::vec3(0.0f, 0.0f, 0.0f);
};

class Terrain
{
public:
	static std::unique_ptr<Terrain> CreateWithHeightMap(const char* fileName);
	~Terrain();
	void SetTexture(const std::vector<image*> image);
	void SetMaterial(const image* diffuse, const image* normalMap);
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	void Draw(const ShaderProgram* program) const;
	static void CalcTangent(std::vector<Vertices>& vertices, std::vector<uint32_t>& indices);
private:
	Terrain() {};
	bool Init(const char* fileName);
	void InitVertices(std::vector<Vertices>& vertices);
	void InitNormals(std::vector<Vertices>& vertices, const std::vector<uint32_t>& indices);
	void InitTexCoords(std::vector<Vertices>& vertices);
	void InitIndices(std::vector<unsigned int>& indices);

	//std::vector<float> m_vertices;
	//std::vector<float> m_texCoords;
	std::vector<unsigned int> m_indices;
	std::vector<Vertices> m_vertices;
	int m_width = 0;
	int m_height = 0;
	int m_numStrips = 0;
	int m_numTrisPerStrip = 0;
	unsigned char* m_heightMapData = nullptr;
	std::unique_ptr<image> m_heightMap = nullptr;
	std::unique_ptr<texture> m_texture0 = nullptr;
	std::unique_ptr<texture> m_texture1 = nullptr;
	std::unique_ptr<texture> m_texture2 = nullptr;
	std::unique_ptr<texture> m_texture3 = nullptr;
	std::unique_ptr<texture> m_diffuse = nullptr;
	std::unique_ptr<texture> m_normalMap = nullptr;

	uint32_t m_terrainVAO = 0;  
	uint32_t m_terrainVBO = 0;
	uint32_t m_terrainTextureVBO = 0;
	uint32_t m_terrainIBO = 0;
};

