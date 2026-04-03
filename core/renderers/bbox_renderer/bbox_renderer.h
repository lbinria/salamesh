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

	void init() override;
	void push() override;
	void render(RendererView &rv, glm::vec3 &position) override;
	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return 0; }

	std::unique_ptr<RendererView> getDefaultView() override { 
		auto rv = std::make_unique<BBoxRendererView>(); 
		rv->visible = true;
		rv->setColor({0.78, 0.32, 0.67});
		return rv;
	}

	// TODO to remove
	void doLoadState(json &j) override {}
	void doSaveState(json &j) const override {}

	private:
	PointSet &ps;
	
};