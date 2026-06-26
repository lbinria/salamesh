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

	PointShader(std::string name, PointSet &ps) : 
		ShaderBase(name, Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))),
		ps(ps) {}

	PointShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("point.vert"), sl::shadersPath("point.frag"))), ps(*new PointSet()) {
		}

	virtual bool isCompatible(Geometry &geometry) override;
	virtual GeometryBuffer createGeometryBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) override;

	void clean() override;
	void clear() override;

	inline int count() {
		return ps.size();
	}

	PointSet& getPointSet() { return ps; }

	vec3& operator[](int index)
	{
		return ps[index];
	}

	private:
	PointSet &ps;

	void updatePointSet(GeometryBuffer &geometryBuffer, PointSet &ps);

};