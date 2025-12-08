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

struct BBoxRenderer : public IRenderer {

	struct Vertex {
		glm::vec3 p;
	};

	BBoxRenderer(PointSet &ps) : 
		IRenderer(Shader("shaders/bbox.vert", "shaders/bbox.frag")),
		ps(ps) {
			setColor(glm::vec3(1.0, 1.0, 1.0));
		}

	void init();
	void push();
	void render(glm::vec3 &position);
	void clean();

	int getRenderElementKind() override { return 0; }

	glm::vec3 getColor() const {
		return color;
	}

	void setColor(glm::vec3 c) {
		shader.use();
		shader.setFloat3("color", c);
		color = c;
	}


	private:
	PointSet &ps;
	glm::vec3 color;

	void doLoadState(json &j) override {
		setColor({j["bboxColor"][0].get<float>(), j["bboxColor"][1].get<float>(), j["bboxColor"][2].get<float>()});
	}

	void doSaveState(json &j) const override {
		j["bboxColor"] = json::array({color.x, color.y, color.z});
	}
	
};