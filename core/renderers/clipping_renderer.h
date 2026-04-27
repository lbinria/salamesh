#pragma once

#include "renderer.h"

#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../include/json.hpp"
using json = nlohmann::json;


#include "../../include/glm/glm.hpp"

using namespace UM;

struct ClippingRenderer : public Renderer {

	struct Vertex {
		glm::vec3 p;
	};

	ClippingRenderer(std::string name, PointSet &ps) : 
		Renderer(name, Shader(sl::shadersPath("clipping.vert"), sl::shadersPath("clipping.frag"))),
		ps(ps) {
		}

	std::string getType() const { return "clipping"; }

	Renderer::GeometricData getData() override;
	std::vector<RendererElementField> getElementFields() override;

	RenderPrimitive getRenderPrimitive() const override {
		return RenderPrimitive::RENDER_TRIANGLES;
	}

	size_t getElementSize() override {
		return sizeof(Vertex);
	}

	void clear() override;


	private:
	PointSet &ps;
	std::vector<Vertex> vertices;

	glm::vec3 point;
	glm::vec3 plane;

	void doLoadState(json &j) override {
	}

	void doSaveState(json &j) const override {
	}
};