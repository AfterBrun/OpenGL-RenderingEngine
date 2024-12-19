#include "texture.h"

std::unique_ptr<texture> texture::CreateFromImage(const image* image, uint32_t type) {  
	auto textureInstance = std::unique_ptr<texture>(new texture());
	textureInstance->CreateTexture();
	textureInstance->SetTextureFromImage(image, type);
	return std::move(textureInstance);
}

std::unique_ptr<texture> texture::CreateFromImage(const image* image, const char* materialType, uint32_t type) {
	auto textureInstance = std::unique_ptr<texture>(new texture());
	textureInstance->SetTypeString(materialType);
	textureInstance->CreateTexture();
	textureInstance->SetTextureFromImage(image, type);
	return std::move(textureInstance);
}

std::unique_ptr<texture> texture::Create(int width, int height, uint32_t format, uint32_t dataType) { 
	auto textureInstance = std::unique_ptr<texture>(new texture());
	textureInstance->CreateTexture();
	textureInstance->SetFormat(width, height, format, dataType);
	//textureInstance->SetFilter(GL_LINEAR, GL_LINEAR);
	return std::move(textureInstance);
}

void texture::CreateTexture() {
	glGenTextures(1, &textureID);
	Bind();
	SetWrap(GL_REPEAT, GL_REPEAT);
	SetFilter(GL_LINEAR, GL_LINEAR);
}

void texture::SetTextureFromImage(const image* image, uint32_t type) {
	GLint channel = GL_RGBA;
	switch (image->getChannelCount()) {
		default: break;
		case 1: channel = GL_RED; break;
		case 2: channel = GL_RG; break;
		case 3: channel = GL_RGB; break;
	}
	m_width = image->getWidth();
	m_height = image->getHeight();
	m_format = channel;
	m_dataTpye = type;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, m_format, type, image->getData());
	glGenerateMipmap(GL_TEXTURE_2D);
}

void texture::SetWrap(GLint wrap_s, GLint wrap_t) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
}

void texture::SetFilter(GLint min, GLint mag) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag);
}

void texture::SetFormat(int width, int height, uint32_t format, uint32_t dataType) {
	m_width = width;
	m_height = height;
	m_format = format;
	m_dataTpye = dataType;
	glTexImage2D(GL_TEXTURE_2D, 0, m_format, m_width, m_height, 0, m_format, dataType, nullptr);
}

void texture::SetBorderColor(const glm::vec4& color) const {
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
		glm::value_ptr(color));
}

texture::~texture() {
	if (textureID) {
		glDeleteTextures(1, &textureID);
	}
}

std::unique_ptr<CubeTexture> CubeTexture::CreateFromImages(const std::vector<image*> images) {
	auto instance = std::unique_ptr<CubeTexture>(new CubeTexture());
	if (!instance->Init(images)) {
		return nullptr;
	}
	return std::move(instance);
}

bool CubeTexture::Init(const std::vector<image*> images) {
	glGenTextures(1, &m_texture);
	Bind();
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for (uint32_t i = 0; i < images.size(); i++) {
		auto image = images[i];
		GLenum format = GL_RGBA;
		switch (image->getChannelCount()) {
			default: break;
			case 1: format = GL_RED; break;
			case 2: format = GL_RG; break;
			case 3: format = GL_RGB; break;
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image->getWidth(), image->getHeight(), 0, format,
			GL_UNSIGNED_BYTE, image->getData());
	}
	return true;
}

void CubeTexture::Bind() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}

CubeTexture::~CubeTexture() {
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
	}
}