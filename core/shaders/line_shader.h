#pragma once

#include "shader_base.h"
#include "../shaders/shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;

#include "glm/glm.hpp"

using namespace UM;

struct LineShader : public ShaderBase {

	struct LineComponent {
		glm::vec3 p;
		glm::vec3 color;
	};

	struct Line {
		glm::vec3 a;
		glm::vec3 b;
		glm::vec3 color;
	};

	LineShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("gizmo_line.vert"), sl::shadersPath("gizmo_line.frag"))) {
			
		}


	virtual bool isCompatible(Geometry &geometry) override;
	virtual GeometryBuffer createShaderBuffer() override;
	virtual Material createMaterial() override;
	
	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) override;
	virtual unsigned int renderElement() override {
		return GL_LINES;
	}


	void clean() override;
	void clear() override;


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