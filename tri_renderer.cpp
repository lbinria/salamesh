#include "tri_renderer.h"

void TriRenderer::push() {

	nverts = _m.nfacets() * 3 /* 3 points per tri */;
	// std::vector<Vertex> vertices(nverts);

	std::vector<Vertex> vertices;
	for (auto &f : _m.iter_facets()) {
		Triangle3 t = f;
		auto n = t.normal();

		const double a = (f.vertex(1).pos() - f.vertex(0).pos()).norm();
		const double b = (f.vertex(2).pos() - f.vertex(1).pos()).norm();
		const double c = (f.vertex(2).pos() - f.vertex(0).pos()).norm();

		const double side_lengths[] = {a, b, c};
		const double s = a + b + c;
		const double area = sqrt(s * (s - a) * (s - b) * (s - c));

		for (int lv = 0; lv < 3; ++lv) {
			auto v = f.vertex(lv);
			auto p = v.pos();

			// Compute height
			const double h = area / (.5 * side_lengths[(lv + 1) % 3]);

			glm::vec3 edge(0.f, 0.f, 0.f);
			edge[lv] = h;

			vertices.push_back({ 
				vertexIndex: v,
				position: glm::vec3(p.x, p.y, p.z),
				size: 1.f,
				normal: glm::vec3(n.x, n.y, n.z),
				heights: edge,
				facetIndex: f,
				cellIndex: 0
			});
		}
	}

	SurfaceRenderer::push(vertices);
}