#include "Model.h"

std::unique_ptr<Model> Model::LoadModel(const char* path) {
	auto model_instance = std::unique_ptr<Model>(new Model());
	if (!model_instance->LoadWithAssimp(path)) {
		return nullptr;
	}
	return std::move(model_instance);
}

bool Model::LoadWithAssimp(std::string path) {
	Assimp::Importer importer;
	auto fileType = path.substr(path.find_last_of("."), path.length());
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (std::strcmp(fileType.c_str(), ".fbx") == 0) { //파일이 fbx일 경우 텍스처를 뒤집지 않음
		scene = importer.ReadFile(path, aiProcess_Triangulate);
	}
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		SPDLOG_INFO("Failed to load Model with Assimp: {}", importer.GetErrorString());
		return false;
	}
	directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
	return true;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene) {
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene);
	}

	for (uint32_t i = 0; i < node->mNumChildren; i++) {
		ProcessNode(node->mChildren[i], scene);
	}
}

void Model::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
	Vertex vertex;
	std::vector<uint32_t> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		/*
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		*/

		if (mesh->mTextureCoords[0]) // mesh가 텍스처 좌표를 가지고 있는가?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoord = vec;
		}
		else {
			vertex.TexCoord = glm::vec2(0.0f, 0.0f);
		}
		m_vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	m_meshs.push_back(Mesh::Create(m_vertices, indices));
	m_vertices.clear();

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
	}

}


void Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, const char* string) {
	unsigned int material_count = material->GetTextureCount(type);
	//SPDLOG_INFO("{} : {}", string, material_count);
	for (unsigned int i = 0; i < material->GetTextureCount(type); i++)
	{
		bool skip = false;
		aiString str;
		material->GetTexture(type, i, &str);
		std::string dir = (directory + "/" + str.C_Str()).c_str();
		for (unsigned int j = 0; j < m_texture_loaded.size(); j++) {
			if (std::strcmp(m_texture_loaded[j].data(), str.C_Str()) == 0) {
				skip = true;
			}
		}
		if (skip) continue;
		SPDLOG_INFO("image file directory: {}", dir);
		auto in = image::CreateFromFile(dir.c_str());
		if (!in) continue;
		auto mat = texture::CreateFromImage(in.get());
		m_meshs.back()->GetMaterialArrayPtr()->push_back(std::move(mat));
		m_meshs.back()->GetMaterialArrayPtr()->back()->SetTypeString(string);
		SPDLOG_INFO("Material Count: {}", m_meshs.back()->GetMaterialArrayPtr()->size());
		m_texture_loaded.push_back(str.C_Str());
	}
}

void Model::Draw(const ShaderProgram* program) const {
	for (unsigned int i = 0; i < m_meshs.size(); i++) {
		m_meshs[i]->Draw(program);
	}
}

void Model::ShowVertices(std::vector<Vertex>& vertices) {
	for (int i = 0; i < vertices.size(); i++) {
		SPDLOG_INFO("position {}, {}, {}, normal {}, {}, {}, texcoord {}, {}", vertices[i].Position.x, vertices[i].Position.y,
			vertices[i].Position.z, vertices[i].Normal.x, vertices[i].Normal.y, vertices[i].Normal.z, vertices[i].TexCoord.x,
			vertices[i].TexCoord.y);
	}
}