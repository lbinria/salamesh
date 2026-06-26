#pragma once

#include "shader_base.h"
#include "../shaders/shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;

using namespace UM;

struct LineShader : public ShaderBase {

	struct LineComponent {
		sl::algebra::vec3 p;
		sl::algebra::vec3 color;
	};

	LineShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("gizmo_line.vert"), sl::shadersPath("gizmo_line.frag"))) {
			
		}


	virtual bool isCompatible(Geometry &geometry) override;
	virtual GeometryBuffer createGeometryBuffer() override;
	virtual Material createMaterial() override;
	
	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) override;
	virtual unsigned int renderElement() override {
		return GL_LINES;
	}


	void clean() override;
	void clear() override;


	private:
	
};