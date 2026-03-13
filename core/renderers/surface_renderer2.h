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
#include "../element.h"
using namespace UM;

struct SurfaceRenderer2 : public MeshRenderer {
	
	SurfaceRenderer2(Surface &m) : 
		MeshRenderer(Shader(sl::shadersPath("surface2.vert"), sl::shadersPath("gizmo_line.frag"))),
		_m(m)
		{}

	void init() override;
	void render(glm::vec3 &position) override;

	void push() override {

		// Convert vec3 f64 to vec3 f32
		auto pointsData = *_m.points.data.get();

		std::vector<glm::vec3> points;
		for (auto &p : pointsData) {
			points.push_back(sl::um2glm(p));
		}

		glBindBuffer(GL_TEXTURE_BUFFER, bufPoints);
		glBufferData(GL_TEXTURE_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_TEXTURE_BUFFER, bufFacets);
		glBufferData(GL_TEXTURE_BUFFER, _m.facets.size() * sizeof(int), _m.facets.data(), GL_STATIC_DRAW);
	
		int fs = _m.facets.size();

		if (auto p = dynamic_cast<Polygons*>(&_m)) {
			glBindBuffer(GL_TEXTURE_BUFFER, bufOffsets);
			glBufferData(GL_TEXTURE_BUFFER, p->offset.size() * sizeof(int), p->offset.data(), GL_STATIC_DRAW);

			int lastOff = p->offset.back();
			nelements = p->offset.back() * 3;

		} else {

			int facetSize = _m.facet_size(0);
			shader.use();
			shader.setInt("regularOffset", facetSize);

			nelements = _m.nfacets() * facetSize * 3;
		}

	}

	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }

	unsigned int bufPoints, bufFacets, bufOffsets;
	unsigned int tboPoints, tboFacets, tboOffsets;

	protected:

	Surface &_m;

	private:

	void doLoadState(json &j) override {
		MeshRenderer::doLoadState(j);
	}
	void doSaveState(json &j) const override {
		MeshRenderer::doSaveState(j);
	}
};

struct QuadRenderer2 : public SurfaceRenderer2 {

	using SurfaceRenderer2::SurfaceRenderer2;



};