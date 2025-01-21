#pragma once
#include "common.h"
#include "Model.h"
#include "Bone.h"
#include "animData.h"

#include <assimp/scene.h>

struct AssimpNodeData
{
	glm::mat4						transformation;
	std::string						name;
	int								childCount;
	std::vector<AssimpNodeData>		child;
};

class Animation
{
public:
	static std::unique_ptr<Animation>				NewAnimation(const char* animationPath, Model* model);
	Bone*											FindBone(const std::string name);
	inline float									GetTickPerSecond() const { return m_tickPerSecond; }
	inline float									GetDuration() const { return m_duration; }
	inline const AssimpNodeData*					GetRootNode() const { return &m_rootNode; }
	inline const std::map<std::string, BoneInfo>&	GetBoneIDMap() { return m_boneInfoMap; }

												   ~Animation() {};
private:
													Animation(const char* animationPath, Model* model);
	void											ReadNodesFromAssimp(AssimpNodeData* root, const aiNode* assimpRoot);
	void											ReadMissingBones(const aiAnimation* animation, Model* model);
	
	float											m_duration;
	int												m_tickPerSecond;
	std::vector<Bone>								m_bones;
	std::map<std::string, BoneInfo>					m_boneInfoMap;
	AssimpNodeData									m_rootNode;

};

