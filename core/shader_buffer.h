#pragma once
#include "shader.h"
#include "renderers/material_params.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct ShaderBuffer {


	ShaderBuffer(Shader &shader, unsigned int vao, unsigned int vbo, std::map<std::string, std::shared_ptr<MaterialParams>> &params) : 
		_shader(shader),
		_params(std::move(params)),
		_vao(vao),
		_vbo(vbo)
	{

	}

	// TODO move that, just for test
	void apply() {
		for (auto &[paramsName, params] : _params)
			params->apply(_shader);
	}

	unsigned int vao() const { return _vao; }
	unsigned int vbo() const { return _vbo; }

	const std::map<std::string, std::shared_ptr<MaterialParams>> getParams() const {
		return _params;
	}

	std::shared_ptr<MaterialParams> getParams(const std::string name) {
		return _params.contains(name) ? _params.at(name) : nullptr;
	}

	template<typename TParams>
	std::shared_ptr<TParams> getParams(const std::string name) {
		return _params.contains(name) ? std::static_pointer_cast<TParams>(_params.at(name)) : nullptr;
	}

	// TODO move that, just for test
	void setPosition(glm::vec3 position) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		// Set model to shader
		_shader.use();
		_shader.setMat4("model", model);
	}


	std::vector<unsigned char> rawData;
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
	std::map<std::string, std::shared_ptr<MaterialParams>> _params;
};
