#include "ShadowMap.h"

std::unique_ptr<ShadowMap> ShadowMap::Create(int width, int height) {
	auto instance = std::unique_ptr<ShadowMap>(new ShadowMap());
	if (!instance->Init(width, height)) {
		return nullptr;
	}
	return std::move(instance);
}

bool ShadowMap::Init(int width, int height) {
	m_width = width;
	m_height = height;
	glGenFramebuffers(1, &m_frameBufferID);
	Bind();
	m_depthTexture = texture::Create(width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_depthTexture->SetFilter(GL_NEAREST, GL_NEAREST);
	m_depthTexture->SetWrap(GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);
	m_depthTexture->SetBorderColor(glm::vec4(1.0f));

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->Get(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		SPDLOG_INFO("failed to create shadow map framebuffer {}", result);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void ShadowMap::CalcTightLightProjection(const glm::mat4& cameraViewProj, const glm::vec3& lightDir, 
										 const PerspectiveProjInfo& persprojInfo,
										 glm::vec3& lightWorldPos, OrthoProjInfo& resultInfo)
{
	//단계 1: frustum corners의 view space에서의 좌표를 계산
	Frustum frustum;
	frustum.CalcCorners(persprojInfo);
	
	//단계 2: frustum corners의 world space로 변환
	glm::mat4 invCameraView = glm::inverse(cameraViewProj);
	frustum.Transform(invCameraView);
	Frustum worldSpaceFrustrum = frustum;
	//worldSpaceFrustrum.PrintFrustum();

	//단계 3: frustum corners의 light space로 변환(1차: 회전 변환만 적용됨)
	glm::mat4 lightView;
	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f); //광원의 위치를 원점에 있다고 가정
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); //광원의 up 벡터
	lightView = glm::lookAt(origin, origin + lightDir, up);
	frustum.Transform(lightView);

	//단계 4: light space에서 frustum corners와 맞는 AABB 생성
	AABB aabb;
	frustum.CalcAABB(aabb);

	//단계 5: light space에서 light Position을 다시 계산
	glm::vec3 bottomLeft = glm::vec3(aabb.MinX, aabb.MinY, aabb.MaxZ);
	glm::vec3 topRight = glm::vec3(aabb.MaxX, aabb.MaxY, aabb.MaxZ);
	glm::vec4 lightPos_lightSpace = glm::vec4((bottomLeft + topRight) / 2.0f, 1.0f);

	//단계 6: light position을 light space -> world space로 변환
	glm::mat4 invLightView = glm::inverse(lightView);
	glm::vec4 lightPos_worldSpace = invLightView * lightPos_lightSpace;
	glm::vec3 lightPos_worldSpace_3d = glm::vec3(lightPos_worldSpace.x, lightPos_worldSpace.y, lightPos_worldSpace.z);
	lightWorldPos = lightPos_worldSpace_3d;

	//단계 7: 새로구한 light position으로의 light space로 다시 world space frustum을 변환한다(2차 회전, 이동 변환이 적용)
	lightView = glm::lookAt(lightPos_worldSpace_3d, lightPos_worldSpace_3d + lightDir, up);
	worldSpaceFrustrum.Transform(lightView);

	//단계 8: 새로구한 light position 기준으로 변환된 frustum을 이용해 AABB를 다시 구한다
	AABB resultAABB;
	worldSpaceFrustrum.CalcAABB(resultAABB);
	resultAABB.UpdataOrthoInfo(resultInfo);
}

ShadowMap::~ShadowMap() {
	if (m_frameBufferID) {
		glDeleteFramebuffers(1, &m_frameBufferID);
	}
}