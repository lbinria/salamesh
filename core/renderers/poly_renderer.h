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


	void init() override;
	void render(glm::vec3 &position) override;
	void render(RendererView &rv, glm::vec3 &position) override;

	void push() override;
	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }

	std::unique_ptr<RendererView> getDefaultView() override { 
		auto rv = std::make_unique<PolyRendererView>();
		rv->visible = true;
		rv->setColor({0.71f, 0.71f, 0.71f});
		rv->setMeshShrink(0.f);
		rv->setMeshSize(0.0f);
		return rv;
	}


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