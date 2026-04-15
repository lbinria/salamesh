#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

#include "../../include/json.hpp"
using json = nlohmann::json;

#include "mesh_renderer.h"
#include "../element.h"
using namespace UM;

struct PolyRenderer : public MeshRenderer {

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

	PolyRenderer(std::string name, Surface &m) : 
		MeshRenderer(name, Shader(sl::shadersPath("poly.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{}


	std::vector<RendererElementField> getElementFields() override;
	std::vector<std::string> getBuffers() override;
	Renderer::GeometricData getData() override;
	void clear() override;


	protected:

	Surface &_m;

	private:

	unsigned int bufNVertsPerFacet; 
	unsigned int texNVertsPerFacet;

	void doLoadState(json &j) override {
		MeshRenderer::doLoadState(j);
	}
	
	void doSaveState(json &j) const override {
		MeshRenderer::doSaveState(j);
	}
};