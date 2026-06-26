#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;

#include "shader_base.h"
#include "element_type.h"
using namespace UM;

#include "material_params.h"

struct SurfaceShader : public ShaderBase {
	
	struct Vertex {
		// int vertexIndex;
		int localIndex;
		int cornerIndex;
		sl::algebra::vec3 p0;
		sl::algebra::vec3 p1;
		sl::algebra::vec3 p2;
		int facetIndex;
	};

	SurfaceShader(std::string name, Surface &m) : 
		ShaderBase(name, Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{}

	SurfaceShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(*new Triangles())
		{}

	virtual bool isCompatible(Geometry &geometry) override;
	virtual GeometryBuffer createGeometryBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) override;

	virtual unsigned int renderElement() override {
		return GL_TRIANGLES;
	}

	void clear() override;
	void clean() override;


	protected:

	Surface &_m;

	private:
};