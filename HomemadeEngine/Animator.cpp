#include "Animator.h"

std::unique_ptr<Animator> Animator::NewAnimator(Animation* animation)
{
	auto instance = std::unique_ptr<Animator>(new Animator(animation));
	return std::move(instance);
}

Animator::Animator(Animation* animation)
{
	m_CurrentTime = 0.0;
	m_CurrentAnimation = animation;
	m_FinalBoneMatrices.reserve(100);

	for (int i = 0; i < 100; i++)
		m_FinalBoneMatrices.push_back(glm::mat4(1.0f));
}


void Animator::UpdateAnimation(float dt)
{
	m_DeltaTime = dt;
	if (m_CurrentAnimation)
	{
		m_CurrentTime += m_CurrentAnimation->GetTickPerSecond() * dt;
		m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
		CalculateBoneTransform(m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
	}
}

void Animator::PlayAnimation(Animation* pAnimation)
{
	m_CurrentAnimation = pAnimation;
	m_CurrentTime = 0.0f;
}

void Animator::CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

	if (Bone)
	{
		Bone->Update(m_CurrentTime);
		nodeTransform = Bone->GetLocalTransformation();
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto& boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap.at(nodeName).id;
		glm::mat4 offset = boneInfoMap.at(nodeName).offset;
		m_FinalBoneMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childCount; i++)
		CalculateBoneTransform(&node->child[i], globalTransformation);
}

