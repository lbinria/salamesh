#pragma once

#include <vector>
#include <tuple>
#include <memory>

#include <ultimaille/all.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

#include "../include/json.hpp"
using json = nlohmann::json;

#include "mesh_renderer.h"
#include "../data/element_type.h"
using namespace UM;

#include "material_params.h"

struct SurfaceMaterial : public MeshMaterial {
	
	struct Vertex {
		// int vertexIndex;
		int localIndex;
		int cornerIndex;
		glm::vec3 p0;
		glm::vec3 p1;
		glm::vec3 p2;
		int facetIndex;
	};

	SurfaceMaterial(std::string name, Surface &m) : 
		MeshMaterial(name, Shader(sl::shadersPath("surface.vert"), sl::shadersPath("surface.frag"))),
		_m(m)
		{}

	void init() override;
	void render(glm::vec3 &position) override;
	virtual void push() override = 0;
	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }


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