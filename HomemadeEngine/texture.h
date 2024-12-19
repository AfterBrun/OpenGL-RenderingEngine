#pragma once

#include "common.h"
#include "image.h"

class texture
{
public:
	static std::unique_ptr<texture> Create(int width, int height, uint32_t format, 
		uint32_t dataType = GL_UNSIGNED_BYTE); //create empty texture
	static std::unique_ptr<texture> CreateFromImage(const image* image, uint32_t type = GL_UNSIGNED_BYTE);
	static std::unique_ptr<texture> CreateFromImage(const image* image, const char* materialType, uint32_t type = GL_UNSIGNED_BYTE);
	~texture();
	void Bind() const { glBindTexture(GL_TEXTURE_2D, textureID); }
	uint32_t Get() const { return textureID; } //get texture ID
	std::string GetType() { return type; }
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
	void SetTypeString(std::string typeName) { type = typeName; }
	std::string GetTypeString() { return type; }
	void SetFilter(GLint min, GLint mag);
	void SetWrap(GLint wrap_s, GLint wrap_t);
	void SetBorderColor(const glm::vec4& color) const;
private:
	texture() {};
	void CreateTexture();
	void SetTextureFromImage(const image* image, uint32_t type);
	void SetFormat(int width, int height, uint32_t format, uint32_t dataType);

	uint32_t textureID{ 0 };
	std::string type;
	int m_width{ 0 };
	int m_height{ 0 };
	uint32_t m_format{ 0 };
	uint32_t m_dataTpye{ GL_UNSIGNED_BYTE };
};

class CubeTexture 
{
public:
	static std::unique_ptr<CubeTexture> CreateFromImages(const std::vector<image*> images);
	void Bind() const;
	const uint32_t Get() const { return m_texture; }
	~CubeTexture();
private:
	CubeTexture() {};
	bool Init(const std::vector<image*> images);
	uint32_t m_texture{ 0 };
};