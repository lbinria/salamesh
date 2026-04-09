#pragma once

#include "../renderer.h"
#include "../../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../../include/json.hpp"
using json = nlohmann::json;

#include "../../../include/glm/glm.hpp"

using namespace UM;

struct BBoxRenderer : public Renderer {

	struct Vertex {
		glm::vec3 p;
	};

	// TODO get shader from renderer name
	BBoxRenderer(std::string name, PointSet &ps) : 
		Renderer(name, Shader(sl::shadersPath("bbox.vert"), sl::shadersPath("bbox.frag"))),
		ps(ps) {}

	RenderPrimitive getRenderPrimitive() const override {
		return RenderPrimitive::RENDER_LINES;
	}

	void clear() override;

	// TODO to remove
	void doLoadState(json &j) override {}
	void doSaveState(json &j) const override {}

	private:
	PointSet &ps;
	
};