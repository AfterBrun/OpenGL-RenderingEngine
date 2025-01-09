#pragma once

#include "common.h"

class vertexLayout
{
public:
	static std::unique_ptr<vertexLayout> CreateVertexLayout();
	void Bind() const { glBindVertexArray(vaoID); }
	void SetAttributePointer(GLuint location, GLint size, GLenum data_type, GLboolean normalized, GLsizei stride, int offset);
	void SetAttributeIPointer(GLuint location, GLint size, GLenum data_type, GLsizei stride, int offset);
	~vertexLayout();
private:
	vertexLayout() {};
	void init();

	uint32_t vaoID{ 0 };
};
