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

#include "shader_base.h"
#include "element_type.h"
using namespace UM;

struct PolyShader : public ShaderBase {

	struct Vertex {
		int vertexIndex;
		int localIndex;
		int cornerIndex;
		int cornerOff;
		int facetIndex;
		glm::vec3 p;
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		glm::vec3 n;
	};

	PolyShader(std::string name, Surface &m) : 
		ShaderBase(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{
			shader.use();
			shader.setFloat3("color", {0.71f, 0.71f, 0.71f});
		}

	PolyShader(std::string name) : 
		ShaderBase(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))),
		_m(*new Polygons())
		{}

	virtual bool isCompatible(Geometry &geometry) override;
	virtual GeometryBuffer createGeometryBuffer() override;
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

	unsigned int bufNVertsPerFacet; 
	unsigned int texNVertsPerFacet;

	void doLoadState(json &j) override {
		
	}
	
	void doSaveState(json &j) const override {
		
	}
};