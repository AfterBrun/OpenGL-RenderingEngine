#include "Bone.h"


std::unique_ptr<Bone> Bone::NewBone(const std::string name, int ID, const aiNodeAnim* channel)
{
	auto boneInstance = std::unique_ptr<Bone>(new Bone(name, ID, channel));
	return std::move(boneInstance);
}

void Bone::Update(float animationTime)
{
	glm::mat4 translate = GetInterpolatePosition(animationTime);
	glm::mat4 rotation = GetInterpolateRotation(animationTime);
	glm::mat4 scale = GetInterpolateScaling(animationTime);
	m_localTransformation = translate * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime) const
{
	for (int index = 0; index < m_NumPositions - 1; index++)
	{
		if (animationTime < m_keyPositions[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

int Bone::GetRotationIndex(float animationTime) const
{
	for (int index = 0; index < m_NumRotations - 1; index++)
	{
		if (animationTime < m_keyRotations[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

int Bone::GetScaleIndex(float animationTime) const
{
	for (int index = 0; index < m_NumScalings - 1; index++)
	{
		if (animationTime < m_keyScales[index + 1].timeStamp)
			return index;
	}
	assert(0);
}

Bone::Bone(const std::string name, int ID, const aiNodeAnim* channel)
	:
	m_name(name), m_id(ID), m_localTransformation(0.0f)
{
	m_NumPositions = channel->mNumPositionKeys;
	for (int positionIndex = 0; positionIndex < m_NumPositions; positionIndex++)
	{
		auto aiPosition = channel->mPositionKeys[positionIndex].mValue;
		float timeStamp = channel->mPositionKeys[positionIndex].mTime;
		KeyPosition newKeyPosition;
		newKeyPosition.position = AssimpGLMhelper::atog_vec3(aiPosition);
		newKeyPosition.timeStamp = timeStamp;
		m_keyPositions.push_back(newKeyPosition);
	}

	m_NumRotations = channel->mNumRotationKeys;
	for (int rotationIndex = 0; rotationIndex < m_NumRotations; rotationIndex++)
	{
		auto aiRotation = channel->mRotationKeys[rotationIndex].mValue;
		float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
		KeyRotation newKeyRotation;
		newKeyRotation.orientation = AssimpGLMhelper::atog_quat(aiRotation);
		newKeyRotation.timeStamp = timeStamp;
		m_keyRotations.push_back(newKeyRotation);
	}

	m_NumScalings = channel->mNumScalingKeys;
	for (int sclaeIndex = 0; sclaeIndex < m_NumScalings; sclaeIndex++)
	{
		auto aiScale = channel->mScalingKeys[sclaeIndex].mValue;
		float timeStamp = channel->mScalingKeys[sclaeIndex].mTime;
		KeyScale newScaling;
		newScaling.scale = AssimpGLMhelper::atog_vec3(aiScale);
		newScaling.timeStamp = timeStamp;
		m_keyScales.push_back(newScaling);
	}

}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float frameDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / frameDiff;
	return scaleFactor;
}

glm::mat4 Bone::GetInterpolatePosition(float animationTime)
{
	if (m_NumPositions == 1)
		return glm::translate(glm::mat4(1.0f), m_keyPositions[0].position);

	int p0ldIndex = GetPositionIndex(animationTime);
	int p1ldIndex = p0ldIndex + 1;
	float scaleFactor = GetScaleFactor(	m_keyPositions[p0ldIndex].timeStamp, 
										m_keyPositions[p1ldIndex].timeStamp, 
										animationTime);
	glm::vec3 finalPosition = glm::mix(	m_keyPositions[p0ldIndex].position, 
										m_keyPositions[p1ldIndex].position, 
										scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::GetInterpolateRotation(float animationTime)
{
	if (m_NumRotations == 1)
	{
		auto rotation = glm::normalize(m_keyRotations[0].orientation);
		return glm::toMat4(rotation);
	}

	int p0ldIndex = GetRotationIndex(animationTime);
	int p1ldIndex = p0ldIndex + 1;
	float scaleFactor = GetScaleFactor(	m_keyRotations[p0ldIndex].timeStamp, 
										m_keyRotations[p1ldIndex].timeStamp, 
										animationTime);
	glm::quat finalRotation = glm::slerp(	m_keyRotations[p0ldIndex].orientation, 
											m_keyRotations[p1ldIndex].orientation, 
											scaleFactor);
	finalRotation = glm::normalize(finalRotation);
	return glm::toMat4(finalRotation);
}

glm::mat4 Bone::GetInterpolateScaling(float animationTime)
{
	if (m_NumScalings == 1)
		return glm::translate(glm::mat4(1.0f), m_keyScales[0].scale);

	int p0ldIndex = GetRotationIndex(animationTime);
	int p1ldIndex = p0ldIndex + 1;
	float scaleFactor = GetScaleFactor(	m_keyScales[p0ldIndex].timeStamp,
										m_keyScales[p1ldIndex].timeStamp,
										animationTime);
	glm::vec3 finalScale = glm::mix(m_keyScales[p0ldIndex].scale, 
									m_keyScales[p1ldIndex].scale, 
									scaleFactor);
	return glm::scale(glm::mat4(1.0f), finalScale);
}

