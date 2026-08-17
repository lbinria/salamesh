#pragma once

#include "shader_base.h"

#include "shader.h"

#include <vector>
#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;

using namespace UM;

struct HalfedgeShader : public ShaderBase {

	HalfedgeShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("edge.vert"), sl::shadersPath("edge.frag")))
		{}

	HalfedgeShader(std::string name, Shader shader) : 
		ShaderBase(name, Shader(std::move(shader))) {
		}


	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;

	virtual unsigned int renderElement() {
		return GL_TRIANGLES;
	}
	
};