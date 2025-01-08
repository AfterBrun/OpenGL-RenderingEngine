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
	//�ܰ� 1: frustum corners�� view space������ ��ǥ�� ���
	Frustum frustum;
	frustum.CalcCorners(persprojInfo);
	
	//�ܰ� 2: frustum corners�� world space�� ��ȯ
	glm::mat4 invCameraView = glm::inverse(cameraViewProj);
	frustum.Transform(invCameraView);
	Frustum worldSpaceFrustrum = frustum;
	//worldSpaceFrustrum.PrintFrustum();

	//�ܰ� 3: frustum corners�� light space�� ��ȯ(1��: ȸ�� ��ȯ�� �����)
	glm::mat4 lightView;
	glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f); //������ ��ġ�� ������ �ִٰ� ����
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); //������ up ����
	lightView = glm::lookAt(origin, origin + lightDir, up);
	frustum.Transform(lightView);

	//�ܰ� 4: light space���� frustum corners�� �´� AABB ����
	AABB aabb;
	frustum.CalcAABB(aabb);

	//�ܰ� 5: light space���� light Position�� �ٽ� ���
	glm::vec3 bottomLeft = glm::vec3(aabb.MinX, aabb.MinY, aabb.MaxZ);
	glm::vec3 topRight = glm::vec3(aabb.MaxX, aabb.MaxY, aabb.MaxZ);
	glm::vec4 lightPos_lightSpace = glm::vec4((bottomLeft + topRight) / 2.0f, 1.0f);

	//�ܰ� 6: light position�� light space -> world space�� ��ȯ
	glm::mat4 invLightView = glm::inverse(lightView);
	glm::vec4 lightPos_worldSpace = invLightView * lightPos_lightSpace;
	glm::vec3 lightPos_worldSpace_3d = glm::vec3(lightPos_worldSpace.x, lightPos_worldSpace.y, lightPos_worldSpace.z);
	lightWorldPos = lightPos_worldSpace_3d;

	//�ܰ� 7: ���α��� light position������ light space�� �ٽ� world space frustum�� ��ȯ�Ѵ�(2�� ȸ��, �̵� ��ȯ�� ����)
	lightView = glm::lookAt(lightPos_worldSpace_3d, lightPos_worldSpace_3d + lightDir, up);
	worldSpaceFrustrum.Transform(lightView);

	//�ܰ� 8: ���α��� light position �������� ��ȯ�� frustum�� �̿��� AABB�� �ٽ� ���Ѵ�
	AABB resultAABB;
	worldSpaceFrustrum.CalcAABB(resultAABB);
	resultAABB.UpdataOrthoInfo(resultInfo);
}

ShadowMap::~ShadowMap() {
	if (m_frameBufferID) {
		glDeleteFramebuffers(1, &m_frameBufferID);
	}
}