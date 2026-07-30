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

	struct Vertex {
		int vertexIndex;
		sl::algebra::vec3 position;
		float size;
	};

	PointShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))) {
		}

	virtual bool isCompatible(Mesh &mesh) override;
	virtual MeshBuffer createMeshBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(MeshBuffer &meshBuffer, Mesh &mesh) override;

	private:

	void updatePointSet(MeshBuffer &meshBuffer, PointSet &ps);

};