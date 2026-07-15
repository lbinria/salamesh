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

struct PolyShader : public ShaderBase {

	struct Vertex {
		int vertexIndex;
		int localIndex;
		int cornerIndex;
		int cornerOff;
		int facetIndex;
		sl::algebra::vec3 p;
		sl::algebra::vec3 p0;
		sl::algebra::vec3 p1;
		sl::algebra::vec3 p2;
		sl::algebra::vec3 n;
	};

	PolyShader(std::string name, Surface &m) : 
		ShaderBase(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{
			shader.use();
			shader.setFloat3("color", sl::algebra::vec3(0.71f, 0.71f, 0.71f));
		}

	PolyShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))),
		_m(*new Polygons())
		{}

	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(MeshBuffer &meshBuffer, Mesh &mesh) override;

	virtual unsigned int renderElement() override {
		return GL_TRIANGLES;
	}

	void clear() override;
	void clean() override;


	protected:

	Surface &_m;

	private:

	unsigned int bufNVertsPerFacet; 
	unsigned int texNVertsPerFacet;
};