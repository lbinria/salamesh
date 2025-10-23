#include "tri_renderer.h"

void TriRenderer::push() {

	nverts = _m.nfacets() * 3 /* 3 points per tri */;

	std::vector<Vertex> vertices;
	for (auto &f : _m.iter_facets()) {
		
		auto p0 = f.vertex(0).pos();
		auto p1 = f.vertex(1).pos();
		auto p2 = f.vertex(2).pos();

		for (int lv = 0; lv < 3; ++lv) {
			auto v = f.vertex(lv);
			auto p = v.pos();

			vertices.push_back({ 
				.vertexIndex= v,
				.localIndex= lv,
				.p= glm::vec3(p.x, p.y, p.z),
				.p0= glm::vec3(p0.x, p0.y, p0.z),
				.p1= glm::vec3(p1.x, p1.y, p1.z),
				.p2= glm::vec3(p2.x, p2.y, p2.z),
				.p3= glm::vec3(0.f, 0.f, 0.f),
				.size= 1.f,
				.facetIndex = f,
				.cellIndex = 0
			});
		}
	}

	SurfaceRenderer::push_bary(vertices);
}