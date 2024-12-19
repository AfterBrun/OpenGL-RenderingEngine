#include "ShaderProgram.h"

std::unique_ptr<ShaderProgram> ShaderProgram::CreateShaderProgram(const char* vs, const char* fs) {
	auto shaderProgram = (std::unique_ptr<ShaderProgram>)new ShaderProgram();
	if (!shaderProgram->Create(vs, fs)) {
		return nullptr;
	}
	return std::move(shaderProgram);
}

bool ShaderProgram::Create(const char* vs, const char* fs) {
	std::optional<std::string> vertex = ReadFileString(vs);
	const char* vertexSource = vertex.value().c_str();

	std::optional<std::string> fragment = ReadFileString(fs);
	const char* fragSource = fragment.value().c_str();

	if (!(vertex.has_value() && fragment.has_value())) {
		return false;
	}
	if (!(CompileShader(vertexSource, &vertexID, GL_VERTEX_SHADER) && CompileShader(fragSource, &fragmentID, GL_FRAGMENT_SHADER))) {
		return false;
	}
	if (!LinkProgram()) {
		return false;
	}
	return true;
}

bool ShaderProgram::CompileShader(const char* vsSource, uint32_t* shaderID, GLenum type) {
	*shaderID = glCreateShader(type);
	glShaderSource(*shaderID, 1, &vsSource, NULL);
	glCompileShader(*shaderID);
	int success;
	char log[512];
	glGetShaderiv(*shaderID, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(*shaderID, 512, NULL, log);
		SPDLOG_INFO("Shader compile failed: {}", log);
		return false;
	}
	SPDLOG_INFO("Vertex Shader Compiled!, ID: {}", *shaderID);
	return true;
}

bool ShaderProgram::LinkProgram() {
	programID = glCreateProgram();
	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragmentID);
	glLinkProgram(programID);
	int success;
	char log[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programID, 512, NULL, log);
		SPDLOG_INFO("Shader Linking Failed: {}", log);
		return false;
	}
	SPDLOG_INFO("Shader Program Created: {}", programID);
	return true;
}

ShaderProgram::~ShaderProgram() {
	if (vertexID) {
		SPDLOG_INFO("Vertex Shader Deleted: {}", vertexID);
		glDeleteShader(vertexID);
	}
	if (fragmentID) {
		SPDLOG_INFO("Fragment Shader Deleted: {}", fragmentID);
		glDeleteShader(fragmentID);
	}
	if (programID) {
		SPDLOG_INFO("Program Deleted: {}", programID);
		glDeleteProgram(programID);
	}
}

void ShaderProgram::Use() const {
	if (programID) {
		glUseProgram(programID);
	}
}

bool ShaderProgram::SetUniform(const char* uniformName, float v0, float v1, float v2, float v3) const {
	if (!programID) {
		SPDLOG_INFO("Shader Program is not exist");
		return false;
	}
	uint32_t uniform_location;
	uniform_location = glGetUniformLocation(programID, uniformName);
	glUniform4f(uniform_location, v0, v1, v2, v3);
	return true;
}

bool ShaderProgram::SetUniform(const char* uniformName, const glm::vec3& value) const {
	if (!programID) {
		SPDLOG_INFO("Shader Program is not exist");
		return false;
	}
	uint32_t uniform_location;
	uniform_location = glGetUniformLocation(programID, uniformName);
	glUniform3fv(uniform_location, 1, glm::value_ptr(value));
	return true;
}

bool ShaderProgram::SetUniform(const char* uniformName, const glm::vec4& value) const {
	if (!programID) {
		SPDLOG_INFO("Shader Program is not exist");
		return false;
	}
	uint32_t uniform_location;
	uniform_location = glGetUniformLocation(programID, uniformName);
	glUniform4fv(uniform_location, 1, glm::value_ptr(value));
	return true;
}

bool ShaderProgram::SetUniform(const char* uniformName, int v0) const {
	if (!programID) {
		SPDLOG_INFO("Shader Program is not exist");
		return false;
	}
	uint32_t uniform_location;
	uniform_location = glGetUniformLocation(programID, uniformName);
	glUniform1i(uniform_location, v0);
	return true;
}

bool ShaderProgram::SetUniform(const char* uniformName, float v0) const {
	if (!programID) {
		SPDLOG_INFO("Shader Program is not exist");
		return false;
	}
	uint32_t uniform_location;
	uniform_location = glGetUniformLocation(programID, uniformName);
	glUniform1f(uniform_location, v0);
	return true;
}

bool ShaderProgram::SetUniform(const char* uniformName, const glm::mat4& matrix) const {
	if (!programID) {
		SPDLOG_INFO("Shader Program is not exist");
		return false;
	}
	uint32_t uniform_location;
	uniform_location = glGetUniformLocation(programID, uniformName);
	glUniformMatrix4fv(uniform_location, 1, GL_FALSE, glm::value_ptr(matrix));
	return true;
}