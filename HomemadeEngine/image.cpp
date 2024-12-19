#include "image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

std::unique_ptr<image> image::CreateFromFile(const char* fileName, bool flipVertical) {
	auto imageInstance = std::unique_ptr<image>(new image());
	if (!imageInstance->Init(fileName, flipVertical)) {
		return nullptr;
	}
	return std::move(imageInstance);
}

bool image::Init(const char* fileName, bool flipVertical) {
	stbi_set_flip_vertically_on_load(flipVertical);
	m_data = stbi_load(fileName, &m_width, &m_height, &m_channel, 0);
	if (!m_data)
	{
		const char* failReason = stbi_failure_reason();
		SPDLOG_INFO("File Name: {}", fileName);
		SPDLOG_INFO("Failed to load texture image: {}", failReason);
		return false;
	}
	return true;
}

image::~image() {
	if (m_data) {
		stbi_image_free(m_data);
	}
}
