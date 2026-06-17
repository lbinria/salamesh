#pragma once

#include "shader_base.h"

#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../../include/glm/glm.hpp"

using namespace UM;

struct HalfedgeShader : public ShaderBase {

	struct LineVert {
		int halfedgeIndex;
		glm::vec3 P0;
		glm::vec3 P1;
		float side;
		float end;
		glm::vec3 bary;
	};

	HalfedgeShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag")))
		{}

	HalfedgeShader(std::string name, Shader shader) : 
		ShaderBase(name, Shader(std::move(shader))) {
		}


	virtual bool isCompatible(Geometry &geometry) override;
	virtual ShaderBuffer createShaderBuffer() override;
	virtual void update(ShaderBuffer &shaderBuffer, Geometry &geometry) override;

	virtual unsigned int renderElement() {
		return GL_TRIANGLES;
	}

	void init() override;
	virtual void push() {};
	void render(glm::vec3 &position) override;
	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::EDGES_ELT | ElementKind::CORNERS_ELT; }

	private:

	void updateHalfedges(ShaderBuffer &shaderBuffer, Surface &m);


	void doLoadState(json &j) override {
		for (auto &[paramsName, params] : _params) {
			if (j.contains(paramsName))
				params->loadState(j[paramsName]);
		}
	}

	void doSaveState(json &j) const override {
		for (auto &[paramsName, params] : _params) {
			j[paramsName] = json::object();
			params->saveState(j[paramsName]);
		}
	}
	
};

struct SurfaceHalfedgeShader : public HalfedgeShader {

	SurfaceHalfedgeShader(std::string name, Surface &m) : 
		HalfedgeShader(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}

	void push() override;

	Surface &_m;

};

struct VolumeHalfedgeShader : public HalfedgeShader {

	VolumeHalfedgeShader(std::string name, Volume &m) : 
		HalfedgeShader(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}

	void push() override;

	Volume &_m;
};

struct PolylineShader : public HalfedgeShader {

		PolylineShader(std::string name, PolyLine &m) : 
		HalfedgeShader(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}

	void push() override;

	PolyLine &_m;
};