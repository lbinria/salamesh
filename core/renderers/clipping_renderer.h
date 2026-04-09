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

	RenderPrimitive getRenderPrimitive() const override {
		return RenderPrimitive::RENDER_TRIANGLES;
	}


	void clear() override;


	private:
	PointSet &ps;


	void doLoadState(json &j) override {
	}

	void doSaveState(json &j) const override {
	}
};