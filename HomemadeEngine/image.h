#pragma once

#include "common.h"

class image
{
public:
	static std::unique_ptr<image> CreateFromFile(const char* fileName, bool flipVertical = true);
	~image();

	int getWidth() const { return m_width; }
	int getHeight() const { return m_height; }
	int getChannelCount() const { return m_channel; }
	unsigned char* getData() const { return m_data; }

private:
	image() {};
	bool Init(const char* fileName, bool flipVertical = true);

	int m_width = 0;
	int m_height = 0;
	int m_channel = 0;
	unsigned char* m_data = nullptr;
};

