#include "tet_renderer.h"

// void TetMaterial::push() {

// 	nelements = _m.nfacets() * 3 /* 3 points per tri */;
// 	std::vector<Vertex> vertices(nelements);

// 	int i = 0;
	
// 	for (int ci = 0; ci < _m.ncells(); ++ci) {
// 		for (int lfi = 0; lfi < 4; ++lfi) {

// 			// Compute normal
// 			Volume::Facet f(_m, _m.facet(ci, lfi));
// 			Triangle3 t = f;
// 			auto n = t.normal();

// 			vec3 v0 = f.vertex(0);
// 			vec3 v1 = f.vertex(1);
// 			vec3 v2 = f.vertex(2);

// 			const double a = (v1 - v0).norm();
// 			const double b = (v2 - v1).norm();
// 			const double c = (v2 - v0).norm();

// 			const double side_lengths[] = {a, b, c};
// 			const double s = a + b + c;
// 			const double area = sqrt(s * (s - a) * (s - b) * (s - c));

// 			for (int lv = 0; lv < 3; ++lv) {
// 				auto v = f.vertex(lv);
// 				vec3 p = v;

// 				// Compute height
// 				const double h = area / (.5 * side_lengths[(lv + 1) % 3]);
// 				glm::vec3 edge(0.f, 0.f, 0.f);
// 				edge[lv] = h;

// 				vertices[i] = { 
// 					.vertexIndex= v,
// 					.position= glm::vec3(p.x, p.y, p.z),
// 					.normal= glm::vec3(n.x, n.y, n.z),
// 					.heights= edge,
// 					.facetIndex= f,
// 					.cellIndex= ci
// 				};

// 				++i;
// 			}
// 		}
// 	}

// 	VolumeMaterial::push_bary(vertices);

// }