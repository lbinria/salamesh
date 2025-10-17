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

struct LineRenderer : public IRenderer {

	struct LineComponent {
		glm::vec3 p;
		glm::vec3 color;
	};

	struct Line {
		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 color;
	};

	LineRenderer() : 
		IRenderer(Shader("shaders/gizmo_line.vert", "shaders/gizmo_line.frag")) {
			
		}

	void init();
	void push();
	void render(glm::vec3 &position);
	void clean();

	int getRenderElementKind() override { return 0; }

	void clearLines() {
		lines.clear();
	}

	void addLine(Line line) {
		lines.push_back(line);

		if (autoUpdate)
			push();
	}

	bool getAutoUpdate() { return autoUpdate; }
	void setAutoUpdate(bool val) { autoUpdate = val; }

	private:
	
	std::vector<Line> lines;
	bool autoUpdate = false;

	void doLoadState(json &j) override {
		
	}

	void doSaveState(json &j) const override {
		
	}
	
};