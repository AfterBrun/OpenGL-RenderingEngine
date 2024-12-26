#include "Terrain.h"

std::unique_ptr<Terrain> Terrain::CreateWithHeightMap(const char* fileName) {
	auto instance = std::unique_ptr<Terrain>(new Terrain());
	instance->Init(fileName);
	return std::move(instance);
}

bool Terrain::Init(const char* fileName) {
	m_heightMap = image::CreateFromFile(fileName);
	if (!m_heightMap) return false;
	
	m_width = m_heightMap->getWidth();
	m_height = m_heightMap->getHeight();
	m_heightMapData = m_heightMap->getData();
	
	InitVertices(m_vertices);
	InitIndices(m_indices);
	InitNormals(m_vertices, m_indices);
	InitTexCoords(m_vertices);
	CalcTangent(m_vertices, m_indices);

	glGenVertexArrays(1, &m_terrainVAO);
	glBindVertexArray(m_terrainVAO);

	glGenBuffers(1, &m_terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(m_vertices[0]), &m_vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, texCoords));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertices), (void*)offsetof(Vertices, tangent));
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &m_terrainIBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned), &m_indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
	return true;
}

void Terrain::SetTexture(const std::vector<image*> image) {
	m_texture0 = texture::CreateFromImage(image[0]);
	m_texture0->SetWrap(GL_REPEAT, GL_REPEAT);
	m_texture0->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	m_texture1 = texture::CreateFromImage(image[1]);
	m_texture1->SetWrap(GL_REPEAT, GL_REPEAT);
	m_texture1->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	m_texture2 = texture::CreateFromImage(image[2]);
	m_texture2->SetWrap(GL_REPEAT, GL_REPEAT);
	m_texture2->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	m_texture3 = texture::CreateFromImage(image[3]);
	m_texture3->SetWrap(GL_REPEAT, GL_REPEAT);
	m_texture3->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

void Terrain::SetMaterial(const image* diffuse, const image* normalMap)
{
	m_diffuse = texture::CreateFromImage(diffuse);
	m_diffuse->SetWrap(GL_REPEAT, GL_REPEAT);
	m_diffuse->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

	m_normalMap = texture::CreateFromImage(normalMap);
	m_normalMap->SetWrap(GL_REPEAT, GL_REPEAT);
	m_normalMap->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
}

void Terrain::Draw(const ShaderProgram* program, float yScale, float yShift) const {
	glBindVertexArray(m_terrainVAO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);

	glActiveTexture(GL_TEXTURE0);
	m_diffuse->Bind();
	program->SetUniform("material.diffuse", 0);

	glActiveTexture(GL_TEXTURE1);
	m_normalMap->Bind();
	program->SetUniform("material.normalMap", 1);

	/*
	glActiveTexture(GL_TEXTURE0);
	m_texture0->Bind();
	program->SetUniform("tex0", 0);

	glActiveTexture(GL_TEXTURE1);
	m_texture1->Bind();
	program->SetUniform("tex1", 1);

	glActiveTexture(GL_TEXTURE2);
	m_texture2->Bind();
	program->SetUniform("tex2", 2);

	glActiveTexture(GL_TEXTURE3);
	m_texture3->Bind();
	program->SetUniform("tex3", 3);
	*/
	program->SetUniform("yScale", yScale / 256.0f);
	program->SetUniform("yShift", yShift);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glActiveTexture(GL_TEXTURE0);
}

Terrain::~Terrain() {
	if (m_terrainVAO) {
		glDeleteBuffers(1, &m_terrainVAO);
	}
	if (m_terrainVBO) {
		glDeleteBuffers(1, &m_terrainVBO);
	}
	if (m_terrainIBO) {
		glDeleteBuffers(1, &m_terrainIBO);
	}
}

void Terrain::InitVertices(std::vector<Vertices>& vertices) {
	unsigned int bytePerPixel = m_heightMap->getChannelCount();

	vertices.resize(m_width * m_height);

	float yScale = 64.0f / 256.0f;
	unsigned int index = 0;
	for (unsigned int i = 0; i < m_height; i++) {
		for (unsigned int j = 0; j < m_width; j++) {
			unsigned char* pixelOffset = m_heightMapData + (j + m_width * i) * bytePerPixel;
			unsigned char y = pixelOffset[0];
			vertices[index].pos.x = (-m_height / 2.0f + i);        // v.x
			vertices[index].pos.y = y * yScale - 20.0f;				   // v.y
			//vertices[index].pos.y = y;							   // v.y
			vertices[index].pos.z = (-m_width / 2.0f + j);         // v.z
			index++;
		}
	}
}

void Terrain::InitNormals(std::vector<Vertices>& vertices, const std::vector<unsigned int>& indices) {
	for (unsigned int i = 0; i < indices.size(); i += 3) { //인덱스상 버텍스 3개씩(즉 삼각형 하나씩) 처리한다
		unsigned int index0 = indices[i];
		unsigned int index1 = indices[i + 1];
		unsigned int index2 = indices[i + 2];
		glm::vec3 edge0 = vertices[index1].pos - vertices[index0].pos;
		glm::vec3 edge1 = vertices[index2].pos - vertices[index0].pos;
		glm::vec3 normal = glm::cross(edge0, edge1);

		vertices[index0].normal += glm::normalize(normal); //구해진 노말벡터를 계속 더함으로서 평균값을 낸다
		vertices[index1].normal += glm::normalize(normal);
		vertices[index2].normal += glm::normalize(normal);
	}

	for (unsigned int i = 0; i < vertices.size(); i++) {
		vertices[i].normal = glm::normalize(vertices[i].normal);
	}
}

void Terrain::CalcTangent(std::vector<Vertices>& vertices, std::vector<uint32_t>& indices)
{
	auto compute = [](
		const glm::vec3& pos1, const glm::vec3& pos2, const glm::vec3& pos3,
		const glm::vec2& uv1, const glm::vec2& uv2, const glm::vec2& uv3)
		-> glm::vec3 {

		auto edge1 = pos2 - pos1;
		auto edge2 = pos3 - pos1;
		auto deltaUV1 = uv2 - uv1;
		auto deltaUV2 = uv3 - uv1;
		float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		if (det != 0.0f) {
			auto invDet = 1.0f / det;
			return deltaUV2.y * edge1 - deltaUV1.y * edge2;
		}
		else {
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}
	};

	// initialize
	std::vector<glm::vec3> tangents;
	tangents.resize(vertices.size());
	memset(tangents.data(), 0, tangents.size() * sizeof(glm::vec3));

	// accumulate triangle tangents to each vertex
	for (size_t i = 0; i < indices.size(); i += 3) {
		auto v1 = indices[i];
		auto v2 = indices[i + 1];
		auto v3 = indices[i + 2];

		tangents[v1] += compute( //탄젠트의 평균을 구한다
			vertices[v1].pos, vertices[v2].pos, vertices[v3].pos,
			vertices[v1].texCoords, vertices[v2].texCoords, vertices[v3].texCoords);

		tangents[v2] = compute(
			vertices[v2].pos, vertices[v3].pos, vertices[v1].pos,
			vertices[v2].texCoords, vertices[v3].texCoords, vertices[v1].texCoords);

		tangents[v3] = compute(
			vertices[v3].pos, vertices[v1].pos, vertices[v2].pos,
			vertices[v3].texCoords, vertices[v1].texCoords, vertices[v2].texCoords);
	}

	// normalize
	for (size_t i = 0; i < vertices.size(); i++) {
		vertices[i].tangent = glm::normalize(tangents[i]);
	}
}

void Terrain::InitTexCoords(std::vector<Vertices>& vertices) {
	unsigned int index = 0;
	//텍스처 좌표 설정
	for (unsigned i = 0; i < m_height; i++) {
		for (unsigned j = 0; j < m_width; j++) {
			vertices[index].texCoords.y = (float)i / 20.0f;
			vertices[index].texCoords.x = (float)j / 20.0f;
			index++;
		}
	}
}

void Terrain::InitIndices(std::vector<unsigned int>& indices) {
	unsigned int quads = (m_width - 1) * (m_height - 1); //지형 폴리곤의 사각형의 갯수
	indices.resize(quads * 6); //인덱스 배열의 길이 설정 
								 //*6을 하는 이유: 한 사각형당 정점의 갯수는 4개이며 각 삼각형 폴리곤 하나를 그리기 위해
								 //                3개의 인덱스가 필요하고 사각형당 2개의 삼각형이 존재하니 6개의 인덱스가 
								 //                필요하다.

	unsigned int index = 0;
	for (unsigned int z = 0; z < m_height - 1; z++) {
		for (unsigned int x = 0; x < m_width - 1; x++) {
			unsigned int indexBottomLeft = z * m_width + x; //사각형의 왼쪽 아래
			unsigned int indexTopLeft = (z + 1) * m_width + x; //사각형의 왼쪽 위
			unsigned int indexTopRight = (z + 1) * m_width + (x + 1); //사각형의 오른쪽 위
			unsigned int indexBottomRight = z * m_width + (x + 1); //사각형의 오른쪽 아래

			//왼쪽 위 삼각형  |/
			indices[index++] = indexBottomLeft;
			indices[index++] = indexTopRight;
			indices[index++] = indexTopLeft;
			//오른쪽 아래 삼각형 /_|
			indices[index++] = indexBottomLeft;
			indices[index++] = indexBottomRight;
			indices[index++] = indexTopRight;
		}
	}
}