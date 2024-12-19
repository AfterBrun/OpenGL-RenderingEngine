#pragma once

#include "common.h"
#include "texture.h"

class FrameBuffer
{
public:
	static std::unique_ptr<FrameBuffer> Create(const std::shared_ptr<texture> colorAttatchment);
	void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID); }
	void BindToDefault() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	const std::shared_ptr<texture> GetColorAttatchment() const { return m_texture; }
	~FrameBuffer();
private:
	FrameBuffer() {};
	void GenFrameBuffer();
	void SetColorAttatchment(const std::shared_ptr<texture> colorAttatchment);
	bool SetRenderBuffer();
	

	uint32_t m_FrameBufferID{ 0 };
	uint32_t m_RenderBufferID{ 0 };
	std::shared_ptr<texture> m_texture{ nullptr };
};

