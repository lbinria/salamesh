
#pragma once

#include "geometry.h"

struct Material {

	Material(const Material&) = delete;
	Material& operator=(const Material&) = delete;

	Material() {
		// Init VAO, VBO
		glGenVertexArrays(1, &_vao);
		glGenBuffers(1, &_vbo);
	}

	virtual std::string getType() const = 0;
	virtual void fill(const Geometry &geometry) = 0;

	unsigned int vao() const { return _vao; }
	unsigned int vbo() const { return _vbo; }

	const MaterialParams& getParams() const {
		return *_params;
	}

	void setMaterial(std::shared_ptr<MaterialParams> material) {
		_params = material;
	}


	void addTBO(const std::string name, std::pair<unsigned int, unsigned int> tbo) {
		_tbos[name] = tbo;
	}

	const GeometryBuffer& getGeometryBuffer() const {
		return *_geometryBuffer;
	}

	protected:
	std::map<std::string, std::pair<unsigned int, unsigned int>> _tbos;
	std::shared_ptr<MaterialParams> _params;
	std::unique_ptr<GeometryBuffer> _geometryBuffer;


	private:
	unsigned int _vao = 0, _vbo = 0;

};