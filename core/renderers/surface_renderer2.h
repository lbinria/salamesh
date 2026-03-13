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
	
	SurfaceRenderer2(Surface &m, Shader shader) : 
		MeshRenderer(std::move(shader)),
		_m(m)
		{}

	void init() override;
	void render(glm::vec3 &position) override;



	void clear() override;
	void clean() override;

	int getRenderElementKind() override { return ElementKind::FACETS_ELT | ElementKind::CORNERS_ELT; }

	unsigned int bufPoints, bufFacets, bufFacetIndexes, bufOffsets;
	unsigned int tboPoints, tboFacets, tboFacetIndexes, tboOffsets;

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

struct TriRenderer2 : public SurfaceRenderer2 {

	TriRenderer2(Surface &m) : 
		// SurfaceRenderer2(m, Shader(sl::shadersPath("tri.vert"), sl::shadersPath("gizmo_line.frag")))
		SurfaceRenderer2(m, Shader(sl::shadersPath("tri.vert"), sl::shadersPath("surface.frag")))
		{}

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
	
		nelements = _m.nfacets() * 3;
	}
};

struct QuadRenderer2 : public SurfaceRenderer2 {

	QuadRenderer2(Surface &m) : 
		SurfaceRenderer2(m, Shader(sl::shadersPath("quads.vert"), sl::shadersPath("gizmo_line.frag")))
		{}

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
	
		nelements = _m.facets.size() /* n verts */ * 3;
	}
};

struct PolyRenderer2 : public SurfaceRenderer2 {

	PolyRenderer2(Surface &m) : 
		SurfaceRenderer2(m, Shader(sl::shadersPath("poly2.vert"), sl::shadersPath("gizmo_line.frag")))
		{}

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

		// All points
		std::vector<int> facetIndexes(_m.facets.size());
		int i = 0;
		for (int f = 0; f < _m.nfacets(); ++f) {
			int fs = _m.facet_size(f);
			for (int lv = 0; lv < fs; ++lv) {
				facetIndexes[i++] = f;
			}
		}

		auto &p = static_cast<Polygons&>(_m);

		glBindBuffer(GL_TEXTURE_BUFFER, bufOffsets);
		glBufferData(GL_TEXTURE_BUFFER, p.offset.size() * sizeof(int), p.offset.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_TEXTURE_BUFFER, bufFacetIndexes);
		glBufferData(GL_TEXTURE_BUFFER, facetIndexes.size() * sizeof(int), facetIndexes.data(), GL_STATIC_DRAW);



		// nelements = p->offset.back() * 3;
		nelements = _m.facets.size() * 3;


	}

};