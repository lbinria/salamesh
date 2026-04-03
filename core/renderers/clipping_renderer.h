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

	void init() override;
	void push() override;
	void render(RendererView &rv, glm::vec3 &position) override;
	void clean() override;
	void clear() override;

	int getRenderElementKind() override { return 0; }

	std::unique_ptr<RendererView> getDefaultView() override { 
		auto rv = std::make_unique<ClippingRendererView>(); 
		// rv->setClippingPlanePoint(getCenter());
		rv->setClippingPlaneNormal({1, 0, 0});
		return rv;
	}




	private:
	PointSet &ps;


	void doLoadState(json &j) override {
	}

	void doSaveState(json &j) const override {
	}
};