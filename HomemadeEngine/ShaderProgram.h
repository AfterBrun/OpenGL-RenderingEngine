#pragma once

#include "common.h"

class ShaderProgram
{
public:
	static std::unique_ptr<ShaderProgram> CreateShaderProgram(const char* vs, const char* fs);
	~ShaderProgram();
	void Use() const;
	bool SetUniform(const char* uniformName, float v0, float v1, float v2, float v3) const;
	bool SetUniform(const char* uniformName, const glm::vec3& value) const;
	bool SetUniform(const char* uniformName, const glm::vec4& value) const;
	bool SetUniform(const char* uniformName, int v0) const;
	bool SetUniform(const char* uniformName, float v0) const;
	bool SetUniform(const char* uniformName, const glm::mat4& matrix) const;
	uint32_t GetProgramID() { return programID; }

private:
	ShaderProgram() {}
	bool Create(const char* vs, const char* fs);
	bool CompileShader(const char* vsSource, uint32_t* shaderID, GLenum type);
	bool LinkProgram();

	uint32_t vertexID = 0;
	uint32_t fragmentID = 0;
	uint32_t programID = 0;
};

