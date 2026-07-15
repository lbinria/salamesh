#pragma once

#include "shader_base.h"

#include "shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;

using namespace UM;

struct HalfedgeShader : public ShaderBase {

	struct LineVert {
		int halfedgeIndex;
		sl::algebra::vec3 P0;
		sl::algebra::vec3 P1;
		float side;
		float end;
		sl::algebra::vec3 bary;
	};

	HalfedgeShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag")))
		{}

	HalfedgeShader(std::string name, Shader shader) : 
		ShaderBase(name, Shader(std::move(shader))) {
		}


	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(MeshBuffer &meshBuffer, Mesh &mesh) override;

	virtual unsigned int renderElement() {
		return GL_TRIANGLES;
	}

	void clear() override;
	void clean() override;


	private:

	void updateHalfedges(MeshBuffer &meshBuffer, Surface &m);
	void updateHalfedges(MeshBuffer &meshBuffer, PolyLine &m);
	
};

struct SurfaceHalfedgeShader : public HalfedgeShader {

	SurfaceHalfedgeShader(std::string name, Surface &m) : 
		HalfedgeShader(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}


	Surface &_m;

};

struct VolumeHalfedgeShader : public HalfedgeShader {

	VolumeHalfedgeShader(std::string name, Volume &m) : 
		HalfedgeShader(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}


	Volume &_m;
};

struct PolylineShader : public HalfedgeShader {

		PolylineShader(std::string name, PolyLine &m) : 
		HalfedgeShader(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}


	PolyLine &_m;
};