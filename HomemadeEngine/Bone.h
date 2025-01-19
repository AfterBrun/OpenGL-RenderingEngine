#pragma once
#include "common.h"
#include <assimp/scene.h>
#include "AssimpGLMhelper.h"

struct KeyPosition
{
	glm::vec3	position;
	float		timeStamp;
};

struct KeyRotation
{
	glm::quat	orientation;
	float		timeStamp;
};

struct KeyScale
{
	glm::vec3	scale;
	float		timeStamp;
};

class Bone
{
public:
	std::unique_ptr<Bone>		NewBone(const std::string name, int ID, const aiNodeAnim* channel);
	void						Update(float animationTime);

	glm::mat4					GetLocalTransformation() const { return m_localTransformation; }
	std::string					GetName() const { return m_name; }
	int							GetID() const { return m_id; }
	int							GetPositionIndex(float animationTime) const;
	int							GetRotationIndex(float animationTime) const;
	int							GetScaleIndex(float animationTime) const;

							   ~Bone() {};
private:
								Bone(const std::string name, int ID, const aiNodeAnim* channel);

	float						GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4					GetInterpolatePosition(float animationTime);
	glm::mat4					GetInterpolateRotation(float animationTime);
	glm::mat4					GetInterpolateScaling(float animationTime);

	std::string					m_name;						//name of bone
	int							m_id;						//id of bone
	glm::mat4					m_localTransformation;		//local transformation

	int							m_NumPositions;				//num of position key
	int							m_NumRotations;				//num of rotation key
	int							m_NumScalings;				//num of sales key

	std::vector<KeyPosition>	m_keyPositions;				//key position array
	std::vector<KeyRotation>	m_keyRotations;				//key position array
	std::vector<KeyScale>		m_keyScales;				//key position array
};

