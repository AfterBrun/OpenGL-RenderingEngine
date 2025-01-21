#include "Animation.h"


std::unique_ptr<Animation> Animation::NewAnimation(const char* animationPath, Model* model)
{
    auto instance = std::unique_ptr<Animation>(new Animation(animationPath, model));
    return std::move(instance);
}

Animation::Animation(const char* animationPath, Model* model)
{
    Assimp::Importer importer;
    const std::string filePath = animationPath;
    const aiScene* scene = importer.ReadFile(filePath, aiProcess_Triangulate);
    auto  animation = scene->mAnimations[0];
    m_duration = animation->mDuration;
    m_tickPerSecond = animation->mTicksPerSecond;
    ReadNodesFromAssimp(&m_rootNode, scene->mRootNode);
    ReadMissingBones(animation, model);
}


Bone* Animation::FindBone(const std::string name)
{
    auto iter = std::find_if(m_bones.begin(), m_bones.end(),
        [&](const Bone& bone)
        {
            return bone.GetName() == name;
        });
    if (iter == m_bones.end()) return nullptr;
    else return &(*iter);
}


void Animation::ReadNodesFromAssimp(AssimpNodeData* root, const aiNode* assimpRoot)
{
    root->childCount = assimpRoot->mNumChildren;
    root->name = assimpRoot->mName.data;
    root->transformation = AssimpGLMhelper::atog_mat4x4(assimpRoot->mTransformation);

    for (int i = 0; i < assimpRoot->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadNodesFromAssimp(&newData, assimpRoot->mChildren[i]);
        root->child.push_back(newData);
    }
}

void Animation::ReadMissingBones(const aiAnimation* animation, Model* model)
{
    int size = animation->mNumChannels;

    auto& boneInfoMap = model->GetBoneInfoMap();
    int& boneCount = model->GetBoneCount(); 

    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        m_bones.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
    }

    m_boneInfoMap = boneInfoMap;
}
