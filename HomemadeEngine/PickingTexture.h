#pragma once
#include "common.h"

struct PixelInfo {
	uint32_t m_objectID = 0;
	uint32_t m_drawCall_ID = 0;
	uint32_t m_triangleID = 0;
};

class PickingTexture
{
public:
	PickingTexture(unsigned int windowWidth, unsigned int windowHeight);
	~PickingTexture();

	void Bind();
	void BindToDefault();
private:
	void Init(unsigned int windowWidth, unsigned int windowHeight);

	uint32_t m_FrameBufferID{ 0 };
	uint32_t m_pickingTexture{ 0 };
	uint32_t m_depthTexture{ 0 };
};

