#pragma once

#include "shader_base.h"

#include "shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;


#include "glm/glm.hpp"

using namespace UM;

struct ClippingMaterial : public ShaderBase {

	struct Vertex {
		glm::vec3 p;
	};

	ClippingMaterial(std::string name, PointSet &ps) : 
		ShaderBase(name, Shader(sl::shadersPath("clipping.vert"), sl::shadersPath("clipping.frag"))),
		ps(ps) {
		}

	void clean() override;
	void clear() override;


	private:
	PointSet &ps;

	void doLoadState(json &j) override {

	}

	void doSaveState(json &j) const override {

	}
};