#include "PickingTexture.h"

PickingTexture::PickingTexture(unsigned int windowWidth, unsigned int windowHeight)
{
	Init(windowWidth, windowHeight);
}	

PickingTexture::~PickingTexture()
{
	if (!m_FrameBufferID) {
		glDeleteBuffers(1, &m_FrameBufferID);
	}
	if (!m_pickingTexture) {
		glDeleteTextures(1, &m_pickingTexture);
	}
	if (!m_depthTexture) {
		glDeleteTextures(1, &m_depthTexture);
	}

}

void PickingTexture::Bind()
{
	glBindBuffer(GL_FRAMEBUFFER, m_FrameBufferID);
}

void PickingTexture::BindToDefault()
{
	glBindBuffer(GL_FRAMEBUFFER, 0);
}

void PickingTexture::Init(unsigned int windowWidth, unsigned int windowHeight)
{

}
