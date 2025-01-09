#include "vertexLayout.h"

std::unique_ptr<vertexLayout> vertexLayout::CreateVertexLayout() {
	auto layoutInstance = std::unique_ptr<vertexLayout>(new vertexLayout());
	layoutInstance->init();
	return std::move(layoutInstance);
}

void vertexLayout::init() {
	glGenVertexArrays(1, &vaoID);
	//SPDLOG_INFO("vertex array created: {}", vaoID);
	Bind();
}

void vertexLayout::SetAttributePointer(GLuint location, GLint size, GLenum data_type, 
									   GLboolean normalized, GLsizei stride, int offset) 
{
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, size, data_type, normalized, stride, (void*)offset);
}

void vertexLayout::SetAttributeIPointer(GLuint location, GLint size, GLenum data_type,
										GLsizei stride, int offset)
{
	glEnableVertexAttribArray(location);
	glVertexAttribIPointer(location, size, data_type, stride, (void*)offset);
}

vertexLayout::~vertexLayout() {
	if (vaoID) {
		glDeleteVertexArrays(1, &vaoID);
	}
}