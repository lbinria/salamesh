#pragma once 

#include "data/material.h"
#include "data/material_buf.h"
#include "data/geometry_layout.h"
#include "data/geometry_buffer.h"

#include "utils/opengl_helper.h"

struct ShaderPass {

	ShaderPass(Shader shader) : _shader(shader) {}

	void init() {

	}

	virtual bool isMaterialSupported(std::shared_ptr<Material> material) const = 0;

	virtual void render(const Material &material, glm::vec3 position) = 0;
	virtual std::shared_ptr<Material> createMaterial() = 0;
	virtual GeometryLayout getLayout() = 0;

	protected:
	Shader _shader;

};