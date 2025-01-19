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
	const aiScene* scene;
	if (std::strcmp(fileType.c_str(), ".fbx") == 0 && std::strcmp(fileType.c_str(), ".dae") == 0) { //파일이 fbx일 경우 텍스처를 뒤집지 않음
		scene = importer.ReadFile(path, aiProcess_Triangulate);
	}
	else if (std::strcmp(fileType.c_str(), ".dae") == 0) //파일이 dae일 경우
	{
		
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	}
	else
	{
		scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
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
	std::vector<Vertex> vertices;
	//Vertex vertex;
	std::vector<uint32_t> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;

		SetVertexBoneDataToDefault(vertex); //boneID, weights 디폴트 설정

		vertex.Position = AssimpGLMhelper::atog_vec3(mesh->mVertices[i]);
		vertex.Normal = AssimpGLMhelper::atog_vec3(mesh->mNormals[i]);

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
		vertices.push_back(vertex);
	}


	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	
	ExtractBoneWeightForVertices(vertices, mesh, scene);
	m_meshs.push_back(Mesh::Create(vertices, indices));
	
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
	if (material_count == 0)
	{
		auto in = image::CreateFromFile("./asset/model/vampire/textures/Vampire_normal.png");
		auto mat = texture::CreateFromImage(in.get(), string);
		m_meshs.back()->GetMaterialArrayPtr()->push_back(std::move(mat));
	}
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

void Model::SetVertexBoneDataToDefault(Vertex& vertex)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		vertex.boneIDs[i] = -1;
		vertex.weights[i] = 0.0f;
	}
}

void Model::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (vertex.boneIDs[i] < 0) {
			vertex.boneIDs[i] = boneID;
			vertex.weights[i] = weight;
			break;
		}
	}
}

void Model::ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) 
{
	for (int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++)
	{
		int boneID = -1;
		std::string boneName = mesh->mBones[boneIndex]->mName.C_Str(); //루프를 돌며 메쉬의 모든 본의 이름을 순회
		if (m_boneInfoMap.find(boneName) == m_boneInfoMap.end()) //만약 멤버 map에 본의 이름이 저장되어있지 않다면 새로 추가
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = m_boneCounter;
			newBoneInfo.offset = AssimpGLMhelper::atog_mat4x4(mesh->mBones[boneIndex]->mOffsetMatrix);
			m_boneInfoMap.insert({ boneName.c_str(), newBoneInfo });
			boneID = m_boneCounter;
			m_boneCounter++;
		}
		else
		{
			boneID = m_boneInfoMap[boneName].id;
		}
		auto weights = mesh->mBones[boneIndex]->mWeights;
		auto numWeights = mesh->mBones[boneIndex]->mNumWeights;
		for (unsigned int weightIndex = 0; weightIndex < numWeights; weightIndex++)
		{
			unsigned int vertexID = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			SetVertexBoneData(vertices[vertexID], boneID, weight);
		}
	}
}
