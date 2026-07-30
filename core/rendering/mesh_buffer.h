#pragma once

#include "shader.h"
#include "material_params.h"
#include "mesh.h"
#include "mesh_primitive.h"
#include "helpers.h"

#include <ultimaille/all.h>
using namespace UM;

#include <map>

struct MeshBuffer {

	MeshBuffer(unsigned int vao, unsigned int vbo) : 
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

	void clean() {
		// TODO
	}

	template<typename T>
	void write(std::vector<T> data, GLenum usage = GL_STATIC_DRAW) {
		glNamedBufferData(vbo(), data.size() * sizeof(T), data.data(), usage);
	}

	template<typename T>
	void write(const std::string tboName, std::vector<T> data, GLenum usage = GL_STATIC_DRAW) {
		glNamedBufferData(tbos[tboName].buf, data.size() * sizeof(T), data.data(), usage);
	}

	void update(Mesh &mesh) {
		// shader.update(mesh);
	}

	unsigned int nelements = 0;

	// Texture Buffer Object (like a SSBO array of data)
	struct TBO {
		std::string name;
		unsigned int texUnit;
		unsigned int tex;
		unsigned int buf;
	};

	// std::vector<TBO> tbos;
	std::map<std::string, TBO> tbos;

	private:
	unsigned int _vao;
	unsigned int _vbo;

};
