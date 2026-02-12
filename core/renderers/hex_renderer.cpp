#include "hex_renderer.h"

void HexRenderer::push() {

	nverts = _m.nfacets() * 2 /* 2 tri per facet */ * 3 /* 3 points per tri */;
	std::vector<Vertex> vertices(nverts);

	int i = 0;
	const auto ref = reference_cells[_m.cell_type];
	constexpr int verts[2][3] = {{0, 1, 3}, {2, 3, 1}};
	
	for (int ci = 0; ci < _m.ncells(); ++ci) {
		for (int lfi = 0; lfi < 6; ++lfi) {
			// Get 4 points of facet
			const vec3 points[4] = {
				_m.points[_m.cells[ci * 8 + ref.facets[lfi * 4]]],
				_m.points[_m.cells[ci * 8 + ref.facets[lfi * 4 + 1]]],
				_m.points[_m.cells[ci * 8 + ref.facets[lfi * 4 + 2]]],
				_m.points[_m.cells[ci * 8 + ref.facets[lfi * 4 + 3]]]
			};

			const int vertices_ref[4] = {
				_m.cells[ci * 8 + ref.facets[lfi * 4]],
				_m.cells[ci * 8 + ref.facets[lfi * 4 + 1]],
				_m.cells[ci * 8 + ref.facets[lfi * 4 + 2]],
				_m.cells[ci * 8 + ref.facets[lfi * 4 + 3]]
			};

			// vec3 n = UM::normal(points, 4);
			
			vec3 res{0, 0, 0};
			const vec3 bary = (points[0] + points[1] + points[2] + points[3]);
			res += cross(points[0]-bary, points[1]-bary);
			res += cross(points[1]-bary, points[2]-bary);
			res += cross(points[2]-bary, points[3]-bary);
			res += cross(points[3]-bary, points[0]-bary);
			const vec3 n = res.normalized();

			for (int t = 0; t < 2; ++t) {

				const double a = (points[verts[t][1]] - points[verts[t][0]]).norm();
				const double b = (points[verts[t][2]] - points[verts[t][1]]).norm();
				const double c = (points[verts[t][2]] - points[verts[t][0]]).norm();

				const double side_lengths[] = {a, b, c};
				const double s = a + b + c;
				const double area = sqrt(s * (s - a) * (s - b) * (s - c));

				for (int j = 0; j < 3; ++j) {
					const auto v = points[verts[t][j]];
					const auto vi = vertices_ref[verts[t][j]];

					// Compute height
					const double h = area / (.5 * side_lengths[(j + 1) % 3]);

					glm::vec3 edge(0.f, 0.f, 0.f);
					edge[j] = h;
					// Exclude first tri height in order unmesh one side of the triangle (the diagonal),
					// It enable to obtain quad meshing
					edge[0] = std::numeric_limits<float>::max();

					vertices[i] = { 
						.vertexIndex= vi,
						.position= glm::vec3(v.x, v.y, v.z),
						.normal= glm::vec3(n.x, n.y, n.z),
						.heights= edge,
						.facetIndex= ci * 6 + lfi,
						.cellIndex= ci
					};

					++i;
				}
			}

		}
	}

	VolumeRenderer::push_bary(vertices);

}