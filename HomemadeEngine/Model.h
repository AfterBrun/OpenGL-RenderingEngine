#pragma once

#include "common.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model
{
public:
	static std::unique_ptr<Model> LoadModel(const char* path);
	void Draw(const ShaderProgram* program) const; 
	~Model() {};
private:
	Model() {};
	bool LoadWithAssimp(std::string path);
	void ProcessNode(aiNode* node, const aiScene* scene);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void loadMaterialTextures(aiMaterial* material, aiTextureType type, const char* string);
	void ShowVertices(std::vector<Vertex>& vertices);

	std::vector<Vertex> m_vertices;
	std::vector<std::unique_ptr<Mesh>> m_meshs;
	std::string directory;
	std::vector<std::string> m_texture_loaded;
};

