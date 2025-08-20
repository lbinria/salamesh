#include "hex_renderer.h"

void HexRenderer::push() {

	#ifdef _DEBUG
	std::cout << "push start." << std::endl;
	#endif

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	nverts = _m.nfacets() * 2 /* 2 tri per facet */ * 3 /* 3 points per tri */;
	std::vector<Vertex> vertices(nverts);

	// Cell properties
	std::chrono::steady_clock::time_point begin_barys = std::chrono::steady_clock::now();

	// (8ms -> 3ms)
	std::vector<float> barys(_m.ncells() * 3);

	const int size = _m.cells.size() / 8;
	for (int ci = 0; ci < size; ++ci) {
		// Compute bary
		const int off = ci * 8;
		const vec3 &v0 = _m.points[_m.cells[off]];	
		const vec3 &v1 = _m.points[_m.cells[off + 1]];
		const vec3 &v2 = _m.points[_m.cells[off + 2]];
		const vec3 &v3 = _m.points[_m.cells[off + 3]];
		const vec3 &v4 = _m.points[_m.cells[off + 4]];
		const vec3 &v5 = _m.points[_m.cells[off + 5]];
		const vec3 &v6 = _m.points[_m.cells[off + 6]];
		const vec3 &v7 = _m.points[_m.cells[off + 7]];

		barys[ci * 3] = (v0.x + v1.x + v2.x + v3.x + v4.x + v5.x + v6.x + v7.x) / 8;
		barys[ci * 3 + 1] = (v0.y + v1.y + v2.y + v3.y + v4.y + v5.y + v6.y + v7.y) / 8;
		barys[ci * 3 + 2] = (v0.z + v1.z + v2.z + v3.z + v4.z + v5.z + v6.z + v7.z) / 8;
	}

	std::chrono::steady_clock::time_point end_barys = std::chrono::steady_clock::now();


	std::chrono::steady_clock::time_point begin_facets = std::chrono::steady_clock::now();


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
						vertexIndex: vi,
						position: glm::vec3(v.x, v.y, v.z),
						size: 1.f,
						normal: glm::vec3(n.x, n.y, n.z),
						heights: edge,
						facetIndex: ci * 6 + lfi,
						cellIndex: ci
					};

					++i;
				}
			}

		}
	}

	std::chrono::steady_clock::time_point end_facets = std::chrono::steady_clock::now();

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
	std::cout << "push end in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	std::cout << "compute bary in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_barys - begin_barys).count() << "ms" << std::endl;
	std::cout << "compute facets in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end_facets - begin_facets).count() << "ms" << std::endl;

	
	std::cout << "mesh has: " << _m.nverts() << " vertices." << std::endl;
	std::cout << "mesh has: " << _m.nfacets() << " facets." << std::endl;
	std::cout << "mesh has: " << _m.ncells() << " cells." << std::endl;
	std::cout << "should draw: " << vertices.size() << " vertices." << std::endl;
	#endif

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
	glBufferData(GL_TEXTURE_BUFFER, barys.size() * sizeof(float), barys.data(), GL_STATIC_DRAW);

	// // Update pointers
	// GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	// // Remap highlight
	// if(bufHighlight != 0) {
	// 	glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// 	ptrHighlight = nullptr;            // forget old pointer
	// 	glDeleteBuffers(1, &bufHighlight);
	// }

	// glGenBuffers(1, &bufHighlight);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// glBufferStorage(GL_TEXTURE_BUFFER, _m.ncells() * sizeof(float), nullptr, flags);
	// ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.ncells() * sizeof(float), flags);

	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);
	// TODO remap facet highlight, filter...

}