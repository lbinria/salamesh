#pragma once

#include "renderer.h"
#include "color_mode.h"

#include "shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../include/json.hpp"
using json = nlohmann::json;

#include "../include/glm/glm.hpp"

using namespace UM;

struct HalfedgeRenderer : public IRenderer {

	struct LineVert {
		int halfedgeIndex;
		glm::vec3 P0;
		glm::vec3 P1;
		float side;
		float end;
		glm::vec3 bary;
	};

	using IRenderer::IRenderer;

	void init();
	virtual void push() = 0;
	void render(glm::vec3 &position);
	void clean();

	int getRenderElementKind() override { return ElementKind::EDGES | ElementKind::CORNERS; }

	// Rename to getThickness
	float getEdgeSize() const {
		return edgeSize;
	}

	// Rename to setThickness
	void setEdgeSize(float size) {
		shader.use();
		shader.setFloat("uThickness", size);
		edgeSize = size;
	}

	// Rename to getInsideColor
	glm::vec3 getEdgeInsideColor() const {
		return edgeInsideColor;
	}

	// Rename to setInsideColor
	void setEdgeInsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorInside", color);
		edgeInsideColor = color;
	}

	// Rename to getOutsideColor
	glm::vec3 getEdgeOutsideColor() const {
		return edgeOutsideColor;
	}

	// Rename to setOutsideColor
	void setEdgeOutsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorOutside", color);
		edgeOutsideColor = color;
	}

	private:

	float edgeSize;
	glm::vec3 edgeInsideColor;
	glm::vec3 edgeOutsideColor;

	void doLoadState(json &j) override {
		setEdgeSize(j["edgeSize"].get<float>());
		setEdgeInsideColor({j["edgeInsideColor"][0].get<float>(), j["edgeInsideColor"][1].get<float>(), j["edgeInsideColor"][2].get<float>()});
		setEdgeOutsideColor({j["edgeOutsideColor"][0].get<float>(), j["edgeOutsideColor"][1].get<float>(), j["edgeOutsideColor"][2].get<float>()});
	}

	void doSaveState(json &j) const override {
		j["edgeSize"] = edgeSize;
		j["edgeInsideColor"] = json::array({edgeInsideColor.x, edgeInsideColor.y, edgeInsideColor.z});
		j["edgeOutsideColor"] = json::array({edgeOutsideColor.x, edgeOutsideColor.y, edgeOutsideColor.z});
	}
	
};

struct SurfaceHalfedgeRenderer : public HalfedgeRenderer {

	SurfaceHalfedgeRenderer(Surface &m) : 
		HalfedgeRenderer(Shader("shaders/edge.vert", "shaders/edge.frag")),
		_m(m) {
			setEdgeSize(2.0f); // TODO here use a setting default edge size
			setEdgeInsideColor({0.0, 0.97, 0.73}); // TODO here use a setting default edge inside color
			setEdgeOutsideColor({0.0, 0.6, 0.45}); // TODO here use a setting default edge outside color
		}

	void push() override;

	Surface &_m;
};

struct VolumeHalfedgeRenderer : public HalfedgeRenderer {

	VolumeHalfedgeRenderer(Volume &m) : 
		HalfedgeRenderer(Shader("shaders/edge.vert", "shaders/edge.frag")),
		_m(m) {
			setEdgeSize(2.0f); // TODO here use a setting default edge size
			setEdgeInsideColor({0.0, 0.97, 0.73}); // TODO here use a setting default edge inside color
			setEdgeOutsideColor({0.0, 0.6, 0.45}); // TODO here use a setting default edge outside color
		}

	void push() override;

	Volume &_m;
};