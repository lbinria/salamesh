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

	enum Stream {
		POINTS_STREAM = 1,
		LINES_STREAM = 2,
		EDGES_STREAM = 4,
		TRIANGLES_STREAM = 8
	};

	MeshBuffer(unsigned int vao, unsigned int vbo, Stream streams) : 
		_vao(vao),
		_vbo(vbo),
		_streams(streams)
	{

	}

	unsigned int vao() const { return _vao; }
	unsigned int vbo() const { return _vbo; }
	Stream streams() const { return _streams; }

	// TODO move that, just for test
	void setPosition(Shader &shader, vec3 position) {
		mat4x4 model = mat<4,4>::identity();
		model = sl::translate(model, position);
		// Set model to shader
		shader.use();
		shader.setMat4("model", static_cast<sl::algebra::mat4x4>(model));
	}

	void clean() {
		// Delete buffers and VAO from the GPU
		glDeleteVertexArrays(1, &_vao);
		glDeleteBuffers(1, &_vbo);
		// Prevent accidental reuse
		_vao = 0;
		_vbo = 0;

		for (auto &[_, tbo] : tbos) {
			glDeleteBuffers(1, &tbo.buf);
			glDeleteTextures(1, &tbo.tex);
		}
		tbos.clear();
	}

	template<typename T>
	void write(std::vector<T> data, GLenum usage = GL_STATIC_DRAW) {
		glNamedBufferData(vbo(), data.size() * sizeof(T), data.data(), usage);
	}

	template<typename T>
	void write(const std::string tboName, std::vector<T> data, GLenum usage = GL_STATIC_DRAW) {
		glNamedBufferData(tbos[tboName].buf, data.size() * sizeof(T), data.data(), usage);
	}

	unsigned int nelements = 0;

	// Texture Buffer Object (like a SSBO array of data)
	struct TBO {
		std::string name;
		unsigned int texUnit;
		unsigned int tex;
		unsigned int buf;
	};

	std::map<std::string, TBO> tbos;

	bool dirty = true;

	private:
	unsigned int _vao;
	unsigned int _vbo;
	Stream _streams;

};
