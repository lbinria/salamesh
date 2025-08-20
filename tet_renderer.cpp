#include "tet_renderer.h"

void TetRenderer::push() {

	#ifdef _DEBUG
	std::cout << "push start." << std::endl;
	#endif

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	nverts = _m.nfacets() * 3 /* 3 points per tri */;
	std::vector<Vertex> vertices(nverts);

	// Cell properties

	// (8ms -> 3ms)
	std::vector<float> barys(_m.ncells() * 3);

	const int size = _m.cells.size() / 4;
	for (int ci = 0; ci < size; ++ci) {
		// Compute bary
		const int off = ci * 4;
		const vec3 &v0 = _m.points[_m.cells[off]];	
		const vec3 &v1 = _m.points[_m.cells[off + 1]];
		const vec3 &v2 = _m.points[_m.cells[off + 2]];
		const vec3 &v3 = _m.points[_m.cells[off + 3]];

		barys[ci * 3] = (v0.x + v1.x + v2.x + v3.x) / 4;
		barys[ci * 3 + 1] = (v0.y + v1.y + v2.y + v3.y) / 4;
		barys[ci * 3 + 2] = (v0.z + v1.z + v2.z + v3.z) / 4;
	}

	std::chrono::steady_clock::time_point end_barys = std::chrono::steady_clock::now();


	std::chrono::steady_clock::time_point begin_facets = std::chrono::steady_clock::now();


	int i = 0;
	
	for (int ci = 0; ci < _m.ncells(); ++ci) {
		for (int lfi = 0; lfi < 4; ++lfi) {


			// Compute normal
			Volume::Facet f(_m, _m.facet(ci, lfi));
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
				auto &p = v.pos();

				// Compute height
				const double h = area / (.5 * side_lengths[(lv + 1) % 3]);
				glm::vec3 edge(0.f, 0.f, 0.f);
				edge[lv] = h;

				vertices[i] = { 
					vertexIndex: v,
					position: glm::vec3(p.x, p.y, p.z),
					size: 1.f,
					normal: glm::vec3(n.x, n.y, n.z),
					heights: edge,
					facetIndex: f,
					cellIndex: ci
				};

				++i;
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