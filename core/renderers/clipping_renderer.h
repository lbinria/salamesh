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

struct ClippingRenderer : public IRenderer {

	struct Vertex {
		glm::vec3 p;
	};

	ClippingRenderer(PointSet &ps) : 
		IRenderer(Shader("shaders/clipping.vert", "shaders/clipping.frag")),
		ps(ps) {
			visible = false;
			// setColor(glm::vec3(1.0, 1.0, 1.0));
		}

	void init() override;
	void push() override;
	void render(glm::vec3 &position) override;
	void clean() override;

	int getRenderElementKind() override { return 0; }

	void setClipping(bool enabled) {
		IRenderer::setClipping(enabled);
		visible = enabled;
	}

	void setClippingPlanePoint(glm::vec3 p) {
		IRenderer::setClippingPlanePoint(p);
		clippingPlanePoint = p;
		push();
	}

	void setClippingPlaneNormal(glm::vec3 n) {
		IRenderer::setClippingPlaneNormal(n);
		clippingPlaneNormal = n;
		push();
	}

	// glm::vec3 getColor() const {
	// 	return color;
	// }

	// void setColor(glm::vec3 c) {
	// 	shader.use();
	// 	shader.setFloat3("color", c);
	// 	color = c;
	// }


	private:
	PointSet &ps;
	glm::vec3 color;
	glm::vec3 clippingPlanePoint{0.0f, 0.0f, 0.0f}; // A point on the plane
	glm::vec3 clippingPlaneNormal{1.0f, 0.0f, 0.0f}; // The normal of the plane

	void doLoadState(json &j) override {
		setColor({j["clippingColor"][0].get<float>(), j["clippingColor"][1].get<float>(), j["clippingColor"][2].get<float>()});
		setClippingPlanePoint({j["clippingPlanePoint"][0].get<float>(), j["clippingPlanePoint"][1].get<float>(), j["clippingPlanePoint"][2].get<float>()});
		setClippingPlaneNormal({j["clippingPlaneNormal"][0].get<float>(), j["clippingPlaneNormal"][1].get<float>(), j["clippingPlaneNormal"][2].get<float>()});
	}

	void doSaveState(json &j) const override {
		j["clippingColor"] = json::array({color.x, color.y, color.z});
		j["clippingPlanePoint"] = json::array({clippingPlanePoint.x, clippingPlanePoint.y, clippingPlanePoint.z});
		j["clippingPlaneNormal"] = json::array({clippingPlaneNormal.x, clippingPlaneNormal.y, clippingPlaneNormal.z});
	}
};