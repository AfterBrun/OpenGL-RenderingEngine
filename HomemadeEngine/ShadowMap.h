#pragma once

#include "common.h"
#include "texture.h"
#include "Frustum.h"

class ShadowMap
{
public:
	static std::unique_ptr<ShadowMap> Create(int width, int height);
	void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID); }
	void BindToDefault() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	const texture* GetDepthTexture() { return m_depthTexture.get(); }
	void CalcTightLightProjection(const glm::mat4& cameraViewProj,        //in 
								  const glm::vec3& lightDir,			  //in
								  const PerspectiveProjInfo persprojInfo, //in
								  glm::vec3* lightWorldPos,				  //out
								  LightOrthoProjInfo* resultInfo);		  //out
	~ShadowMap();
private:
	ShadowMap() {};
	bool Init(int width, int height);

	int m_width{ 0 };
	int m_height{ 0 };
	uint32_t m_frameBufferID{ 0 };
	std::unique_ptr<texture> m_depthTexture{ nullptr };
};

