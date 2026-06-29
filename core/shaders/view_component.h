#pragma once

#include "geometry_buffer.h"
#include "material.h"

struct ShaderBase;

struct ViewComponent {
	
	ViewComponent(std::string name, GeometryBuffer &geometryBuffer, Material &material, ShaderBase &shader) :
	_geometryBuffer(geometryBuffer),
	_material(material),
	_shader(shader),
	_name(name) {
		_index = maxIndex;
		++maxIndex;
	}

	int getIndex() const {
		return _index;
	}

	static inline int getMaxIndex() {
		return maxIndex;
	}

	static void clearIndex() {
		maxIndex = 0;
	}

	Material& getMaterial() {
		return _material;
	}

	GeometryBuffer &getGeometryBuffer() {
		return _geometryBuffer;
	}

	void setVisible(bool visible) {
		_visible = visible;
	}

	bool isVisible() const {
		return _visible;
	}

	void update(Geometry &geometry);


	ShaderBase &getShader() {
		return _shader;
	}

	private:
	std::string _name;
	GeometryBuffer _geometryBuffer;
	Material _material;
	ShaderBase &_shader;
	bool _visible = true;

	static inline int maxIndex = 0;
	int _index;

};