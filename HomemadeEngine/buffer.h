#pragma once

#include "common.h"

class buffer
{
public:
	static std::unique_ptr<buffer> CreateBuffer(int target, void* data, size_t data_size, int use);
	void Bind() { glBindBuffer(target, bufferID); }
	uint32_t GetID() { return bufferID; }
	~buffer();

private:
	buffer() {};
	void init(int target, void* data, size_t data_size, int use);

	uint32_t bufferID{ 0 };
	uint32_t target{ 0 };
	uint32_t use{ 0 };
};