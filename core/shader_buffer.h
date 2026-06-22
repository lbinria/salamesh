#pragma once
#include "shader.h"
#include "shaders/material_params.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct GeometryBuffer {


	GeometryBuffer(Shader &shader, unsigned int vao, unsigned int vbo) : 
		_shader(shader),
		_vao(vao),
		_vbo(vbo)
	{

	}

	unsigned int vao() const { return _vao; }
	unsigned int vbo() const { return _vbo; }

	// TODO move that, just for test
	void setPosition(glm::vec3 position) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		// Set model to shader
		_shader.use();
		_shader.setMat4("model", model);
	}

	unsigned int nelements = 0;

	Shader &getShader() {
		return _shader;
	}

	struct TBO {
		std::string name;
		unsigned int texUnit;
		unsigned int tex;
	};

	std::vector<TBO> tbos;



	private:
	unsigned int _vao;
	unsigned int _vbo;
	Shader &_shader;

};
