#include "poly_renderer.h"

void PolyRenderer::push() {
 
	// Compute number of triangles needed to represent a facet
	int ntri = 0;
	for (auto &f : _m.iter_facets()) {
		int nvertsFacet = f.size();
		ntri += nvertsFacet;
	}
	nverts = 3 * ntri /* 3 points per tri, n tri per facet */;

	std::vector<Vertex> vertices;
	for (auto &f : _m.iter_facets()) {

		// There is as much triangles as vertices in facet,
		const int nv = f.size();
		const int ntri = nv;

		// Compute bary
		glm::vec3 bary{0.};
		for (int v = 0; v < nv; ++v) {
			auto pos = f.vertex(v).pos();
			bary += glm::vec3{pos.x, pos.y, pos.z};
		}
		bary /= nv;

		// Compute normal
		vec3 pts[nv];
		for (int v = 0; v < nv; ++v) {
			pts[v] = f.vertex(v).pos();
		}
		vec3 n = geo::normal(pts, nv);

		

		for (int t = 0; t < ntri; ++t) {

			const int lv = t;
			// Three points of current triangle
			vec3 verts[3] = {vec3(bary.x, bary.y, bary.z) , f.vertex(lv).pos(), f.vertex((lv + 1) % nv).pos()};
			auto v = f.vertex(lv);

			for (int i = 0; i < 3; ++i) {

				int v = i == 0 ? -1 : f.vertex((lv + (i - 1)) % nv);

				auto p0 = verts[i];
				auto p1 = verts[(i + 1) % 3];
				auto p2 = verts[(i + 2) % 3];

				// Compute the projected point from vertex to base
				auto s0 = p0 - p2;
				auto s1 = p1 - p2;
				auto length = s1.normalized() * s0;
				auto bh = p2 + s1.normalized() * length;

				// Compute height as the distance between base point and vertex
				float h = (p0 - bh).norm();

				// Or equivalently...
				// auto s0 = p0 - p1;
				// auto s1 = p2 - p1;

				// Compute height with heron formula
				const double a = (p1 - p0).norm();
				const double b = (p2 - p1).norm();
				const double c = (p2 - p0).norm();

				const double side_lengths[] = {a, b, c};
				const double s = (a + b + c) * .5;
				const double area = sqrt(s * (s - a) * (s - b) * (s - c));
				const double h2 = 2. * area / side_lengths[1];

				// glm::vec3 hh(0.);
				// hh[i] = h2;

				vertices.push_back({ 
					.vertexIndex = v,
					.localIndex = i,
					.facetIndex = f,
					.p = glm::vec3(p0.x, p0.y, p0.z),
					.n = glm::vec3(n.x, n.y, n.z),
					.b = bary,
					.h = glm::vec3(bh.x, bh.y, bh.z)
					// .h = hh
				});
			}
		}
	}

	SurfaceRenderer2::push(vertices);

}