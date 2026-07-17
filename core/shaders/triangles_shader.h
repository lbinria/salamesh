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

struct TrianglesShader : public ShaderBase {
	
	struct Vertex {
		// int vertexIndex;
		int localIndex;
		int cornerIndex;
		sl::algebra::vec3 p0;
		sl::algebra::vec3 p1;
		sl::algebra::vec3 p2;
		int facetIndex;
	};

	TrianglesShader(std::string name, Surface &m) : 
		ShaderBase(name, Shader(sl::shadersPath("triangles.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{}

	TrianglesShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("triangles.vert"), sl::shadersPath("surface.frag"))),
		_m(*new Triangles())
		{}

	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(MeshBuffer &meshBuffer, Mesh &mesh) override;

	virtual unsigned int renderElement() override {
		return GL_TRIANGLES;
	}

	protected:

	Surface &_m;

	private:
};