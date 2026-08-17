#pragma once
#include <vector>

struct ShaderLayout {

	int size;

	struct ShaderLayoutElement {
		const char * name;
		size_t ptr;
	};

	std::vector<ShaderLayoutElement> elements;
};