#include "quad_renderer.h"


// void QuadRenderer::push() {

// 	nverts = _m.nfacets() * 6 /* 3 points per tri, 2 tri per facet */;

// 	constexpr int verts[2][3] = {{0, 1, 3}, {2, 3, 1}};

// 	std::vector<Vertex> vertices;
// 	for (auto &f : _m.iter_facets()) {

// 		// // Get 4 points of facet
// 		// const vec3 points[4] = {
// 		// 	f.vertex(0).pos(),
// 		// 	f.vertex(1).pos(),
// 		// 	f.vertex(2).pos(),
// 		// 	f.vertex(3).pos()
// 		// };

// 		for (int t = 0; t < 2; ++t) {

// 			auto p0 = f.vertex(0).pos();
// 			auto p1 = f.vertex(1).pos();
// 			auto p2 = f.vertex(2).pos();
// 			auto p3 = f.vertex(3).pos();

// 			// const double a = (points[verts[t][1]] - points[verts[t][0]]).norm();
// 			// const double b = (points[verts[t][2]] - points[verts[t][1]]).norm();
// 			// const double c = (points[verts[t][2]] - points[verts[t][0]]).norm();

// 			// const double side_lengths[] = {a, b, c};
// 			// const double s = a + b + c;
// 			// const double area = sqrt(s * (s - a) * (s - b) * (s - c));

// 			for (int j = 0; j < 3; ++j) {

// 				auto lv = verts[t][j];

// 				auto v = f.vertex(lv);
// 				auto p = v.pos();
// 				// Compute height
// 				// const double h = area / (.5 * side_lengths[(lv + 1) % 3]);

// 				// glm::vec3 edge(0.f, 0.f, 0.f);
// 				// edge[j] = h;
// 				// Exclude first tri height in order unmesh one side of the triangle (the diagonal),
// 				// It enable to obtain quad meshing
// 				// edge[0] = std::numeric_limits<float>::max();


// 				vertices.push_back({ 
// 					.vertexIndex= v,
// 					.localIndex= lv,
// 					.p0= glm::vec3(p0.x, p0.y, p0.z),
// 					.p1= glm::vec3(p1.x, p1.y, p1.z),
// 					.p2= glm::vec3(p2.x, p2.y, p2.z),
// 					.facetIndex= f
// 				});
// 			}
// 		}
// 	}

// 	shader.use();
// 	shader.setInt("nvertsPerFacet", 4);

// 	SurfaceRenderer::push(vertices);

// }