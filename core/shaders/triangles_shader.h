#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "json.hpp"
using json = nlohmann::json;

#include "shader_base.h"
#include "element_type.h"
using namespace UM;

#include "material_params.h"

struct TrianglesShader : public ShaderBase {
	
	TrianglesShader(std::string name, Surface &m) : 
		ShaderBase(name, Shader(sl::shadersPath("triangles.vert"), sl::shadersPath("surface.frag"))) {}

	TrianglesShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("triangles.vert"), sl::shadersPath("surface.frag"))) {}

	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;

	virtual unsigned int renderElement() override {
		return GL_TRIANGLES;
	}

};