#pragma once
#include "shader.h"
#include "material_params.h"
#include "helpers.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct GeometryBuffer {

	GeometryBuffer(unsigned int vao, unsigned int vbo) : 
		_vao(vao),
		_vbo(vbo)
	{

	}

	unsigned int vao() const { return _vao; }
	unsigned int vbo() const { return _vbo; }

	// TODO move that, just for test
	void setPosition(Shader &shader, vec3 position) {
		mat4x4 model = mat<4,4>::identity();
		model = sl::translate(model, position);
		// Set model to shader
		shader.use();
		shader.setMat4("model", static_cast<sl::algebra::mat4x4>(model));
	}

	unsigned int nelements = 0;

	// Texture Buffer Object (like a SSBO array of data)
	struct TBO {
		std::string name;
		unsigned int texUnit;
		unsigned int tex;
	};

	std::vector<TBO> tbos;

	private:
	unsigned int _vao;
	unsigned int _vbo;

};
