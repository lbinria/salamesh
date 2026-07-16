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

struct TetrahedrasShader : public ShaderBase {
	
	struct Vertex {
		sl::algebra::vec3 p0;
		sl::algebra::vec3 p1;
		sl::algebra::vec3 p2;
		sl::algebra::vec3 bary;
		int localIndex;
		int vertexIndex;
		int facetIndex;
		int cellIndex;
	};

	TetrahedrasShader(std::string name, Volume &m) : 
		ShaderBase(name, Shader(sl::shadersPath("tetrahedras.vert"), sl::shadersPath("volumus.frag"))),
		_m(m)
		{}

	TetrahedrasShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("tetrahedras.vert"), sl::shadersPath("volumus.frag"))),
		_m(*new Tetrahedra())
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

	Volume &_m;

	private:
};