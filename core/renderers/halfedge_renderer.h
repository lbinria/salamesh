#pragma once

#include "renderer.h"
#include "../color_mode.h"

#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../../include/glm/glm.hpp"

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

	int getRenderElementKind() override { return ElementKind::EDGES_ELT | ElementKind::CORNERS_ELT; }

	float getThickness() const {
		return edgeSize;
	}

	void setThickness(float size) {
		shader.use();
		shader.setFloat("thickness", size);
		edgeSize = size;
	}

	glm::vec3 getInsideColor() const {
		return edgeInsideColor;
	}

	void setInsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorInside", color);
		edgeInsideColor = color;
	}

	glm::vec3 getOutsideColor() const {
		return edgeOutsideColor;
	}

	void setOutsideColor(glm::vec3 color) {
		shader.use();
		shader.setFloat3("uColorOutside", color);
		edgeOutsideColor = color;
	}

	private:

	float edgeSize;
	glm::vec3 edgeInsideColor;
	glm::vec3 edgeOutsideColor;

	void doLoadState(json &j) override {
		setThickness(j["edgeSize"].get<float>());
		setInsideColor({j["edgeInsideColor"][0].get<float>(), j["edgeInsideColor"][1].get<float>(), j["edgeInsideColor"][2].get<float>()});
		setOutsideColor({j["edgeOutsideColor"][0].get<float>(), j["edgeOutsideColor"][1].get<float>(), j["edgeOutsideColor"][2].get<float>()});
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
			setThickness(2.0f);
			setInsideColor({0.0, 0.97, 0.73});
			setOutsideColor({0.0, 0.6, 0.45});
		}

	void push() override;

	Surface &_m;
};

struct VolumeHalfedgeRenderer : public HalfedgeRenderer {

	VolumeHalfedgeRenderer(Volume &m) : 
		HalfedgeRenderer(Shader("shaders/edge.vert", "shaders/edge.frag")),
		_m(m) {
			setThickness(2.0f);
			setInsideColor({0.0, 0.97, 0.73});
			setOutsideColor({0.0, 0.6, 0.45});
		}

	void push() override;

	Volume &_m;
};