#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "json.hpp"
using json = nlohmann::json;

#include "mesh_renderer.h"
#include "element_type.h"
using namespace UM;

#include "material_params.h"

struct SurfaceShader : public MeshMaterial {
	
	struct Vertex {
		// int vertexIndex;
		int localIndex;
		int cornerIndex;
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		int facetIndex;
	};

	SurfaceShader(std::string name, Surface &m) : 
		MeshMaterial(name, Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{}

	SurfaceShader(std::string name) : 
		MeshMaterial(name, Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(*new Triangles())
		{}

	virtual bool isCompatible(Geometry &geometry) override;
	virtual GeometryBuffer createShaderBuffer() override;
	virtual Material createMaterial() override;
	virtual void update(GeometryBuffer &geometryBuffer, Geometry &geometry) override;

	virtual unsigned int renderElement() override {
		return GL_TRIANGLES;
	}

	void clear() override;
	void clean() override;


	protected:

	Surface &_m;

	private:

	void doLoadState(json &j) override {
		MeshMaterial::doLoadState(j);
	}
	void doSaveState(json &j) const override {
		MeshMaterial::doSaveState(j);
	}
};