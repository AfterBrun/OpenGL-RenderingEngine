#pragma once
#include "common.h"
#include "Animation.h"

class Animator
{
public:
	static std::unique_ptr<Animator> NewAnimator(Animation* animation);

	void UpdateAnimation(float dt);

	void PlayAnimation(Animation* pAnimation);

	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

	const std::vector<glm::mat4>& GetFinalBoneMatrices() { return m_FinalBoneMatrices; }

private:
	Animator(Animation* animation);
	std::vector<glm::mat4> m_FinalBoneMatrices;
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;
};

