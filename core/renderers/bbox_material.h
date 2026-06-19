#pragma once

#include "shader_base.h"
#include "../shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "../../../include/json.hpp"
using json = nlohmann::json;

#include "../../../include/glm/glm.hpp"

using namespace UM;

struct BBoxMaterial : public ShaderBase {

	struct Vertex {
		glm::vec3 p;
	};

	// TODO get shader from renderer name
	BBoxMaterial(std::string name, PointSet &ps) : 
		ShaderBase(name, Shader(sl::shadersPath("bbox.vert"), sl::shadersPath("bbox.frag"))),
		ps(ps) {

		}

	void clear() override;
	void clean() override;

	private:
	PointSet &ps;
	glm::vec3 color;

	void doLoadState(json &j) override {
		
	}

	void doSaveState(json &j) const override {
		
	}
	
};