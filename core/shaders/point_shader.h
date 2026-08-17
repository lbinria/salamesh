#pragma once

#include <vector>
#include <optional>

#include "json.hpp"
using json = nlohmann::json;

#include <ultimaille/all.h>

#include "shader_base.h"

#include "shader.h"

using namespace UM;



struct PointShader : public ShaderBase {

	PointShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))) {
		}

	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;
	// virtual ShaderLayout getLayout() const override;

};