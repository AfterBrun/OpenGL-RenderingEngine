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

ShadowMap::~ShadowMap() {
	if (m_frameBufferID) {
		glDeleteFramebuffers(1, &m_frameBufferID);
	}
}