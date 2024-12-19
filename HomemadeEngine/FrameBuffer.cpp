#include "FrameBuffer.h"

std::unique_ptr<FrameBuffer> FrameBuffer::Create(const std::shared_ptr<texture> colorAttatchment) {
	auto instance = std::unique_ptr<FrameBuffer>(new FrameBuffer());
	instance->GenFrameBuffer();
	instance->SetColorAttatchment(colorAttatchment);
	if (!instance->SetRenderBuffer()) {
		return nullptr;
	}
	return std::move(instance);
}

void FrameBuffer::GenFrameBuffer() {
	glGenFramebuffers(1, &m_FrameBufferID);
	Bind();
}

void FrameBuffer::SetColorAttatchment(const std::shared_ptr<texture> colorAttatchment) {
	m_texture = colorAttatchment;
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->Get(), 0);
}

bool FrameBuffer::SetRenderBuffer() {
	glGenRenderbuffers(1, &m_RenderBufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferID);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_texture->GetWidth(), m_texture->GetWidth());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderBufferID);
	
	auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE) {
		SPDLOG_INFO("Failed to Create FrameBuffer: {}", result);
		return false;
	}
	BindToDefault();
	return true;
}

FrameBuffer::~FrameBuffer() {
	if (m_FrameBufferID) {
		glDeleteFramebuffers(1, &m_FrameBufferID);
	}
}