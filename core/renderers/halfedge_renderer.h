#pragma once

#include "renderer.h"

#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../../include/glm/glm.hpp"

using namespace UM;

struct HalfedgeMaterial : public Material {

	struct LineVert {
		int halfedgeIndex;
		glm::vec3 P0;
		glm::vec3 P1;
		float side;
		float end;
		glm::vec3 bary;
	};

	using Material::Material;

	void init() override;
	virtual void push() = 0;
	void render(glm::vec3 &position) override;
	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::EDGES_ELT | ElementKind::CORNERS_ELT; }

	private:

	void doLoadState(json &j) override {
		for (auto &[paramsName, params] : _params) {
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

struct SurfaceHalfedgeRenderer : public HalfedgeMaterial {

	SurfaceHalfedgeRenderer(std::string name, Surface &m) : 
		HalfedgeMaterial(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}

	void push() override;

	Surface &_m;

};

struct VolumeHalfedgeRenderer : public HalfedgeMaterial {

	VolumeHalfedgeRenderer(std::string name, Volume &m) : 
		HalfedgeMaterial(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}

	void push() override;

	Volume &_m;
};

struct PolylineRenderer : public HalfedgeMaterial {

		PolylineRenderer(std::string name, PolyLine &m) : 
		HalfedgeMaterial(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag"))),
		_m(m) {

		}

	void push() override;

	PolyLine &_m;
};