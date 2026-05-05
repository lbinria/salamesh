#pragma once

#include <vector>

struct GeometryBuffer {

	struct TextureBufferData {
		const void * data;
		size_t size;
	};

	struct VertexBufferData {
		const void * data;
		size_t size;
	};

	VertexBufferData vboBuffer;
	std::map<std::string, TextureBufferData> texBuffers;
	size_t nverts = 0;

	virtual ~GeometryBuffer() = default;
};

