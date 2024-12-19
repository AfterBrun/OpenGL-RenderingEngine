#include "buffer.h"

std::unique_ptr<buffer> buffer::CreateBuffer(int target, void* data, size_t data_size, int use) {
	auto buf = std::unique_ptr<buffer>(new buffer());
	buf->init(target, data, data_size, use);
	return std::move(buf);
}

void buffer::init(int target, void* data, size_t data_size, int use) {
	this->target = target;
	this->use = use;
	glGenBuffers(1, &bufferID);
	//SPDLOG_INFO("buffer created: {}", bufferID);
	Bind();
	glBufferData(target, data_size, data, use);
	//SPDLOG_INFO("buffer data set");
}

buffer::~buffer() {
	if (bufferID) {
		glDeleteBuffers(1, &bufferID);
	}
}