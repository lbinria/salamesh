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

struct PolyShader : public ShaderBase {

	PolyShader(std::string name, Surface &m) : 
		ShaderBase(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))) {
			shader.use();
			shader.setFloat3("color", sl::algebra::vec3(0.71f, 0.71f, 0.71f));
		}

	PolyShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))) {
			
		}

	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;

	virtual unsigned int renderElement() override {
		return GL_TRIANGLES;
	}

};