#pragma once

#include "mesh_buffer.h"
#include "mesh.h"
#include "material.h"

struct ShaderBase;

struct ViewComponent {
	
	ViewComponent(std::string name, MeshBuffer &meshBuffer, Material &material, ShaderBase &shader) :
	_meshBuffer(meshBuffer),
	_material(material),
	_shader(shader),
	_name(name) {

	}

	Material& getMaterial() {
		return _material;
	}

	MeshBuffer &getMeshBuffer() {
		return _meshBuffer;
	}

	void setVisible(bool visible) {
		_visible = visible;
	}

	bool isVisible() const {
		return _visible;
	}

	void update(Mesh &mesh);


	ShaderBase &getShader() {
		return _shader;
	}

	private:
	std::string _name;
	MeshBuffer _meshBuffer;
	Material _material;
	ShaderBase &_shader;
	bool _visible = true;

};