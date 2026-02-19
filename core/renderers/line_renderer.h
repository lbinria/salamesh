#pragma once

#include "renderer.h"
#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "../../include/glm/glm.hpp"

using namespace UM;

struct LineRenderer : public Renderer {

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
		Renderer(Shader(sl::shadersPath("gizmo_line.vert"), sl::shadersPath("gizmo_line.frag"))) {
			
		}

	void init() override;
	void push() override;
	void render(glm::vec3 &position) override;
	void clean() override;
	void clear() override;

	int getRenderElementKind() override { return 0; }

	void clearLines() {
		lines.clear();
		nverts = 0;
	}

	// TODO generate guid for line
	// TODO maybe push partially data to GPU instead of all data
	void addLine(Line line) {
		lines.push_back(line);

		if (autoUpdate)
			push();
	}

	void addLines(std::vector<Line> allLines) {
		lines.insert(lines.end(), allLines.begin(), allLines.end());

		if (autoUpdate)
			push();
	}

	// TODO removeLine by guid / idx / range

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