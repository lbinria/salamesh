#include "tri_renderer.h"

void TriRenderer::push() {

	#ifdef _DEBUG
	std::cout << "push start." << std::endl;
	#endif

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	nverts = _m.nfacets() * 3 /* 3 points per tri */;
	// std::vector<Vertex> vertices(nverts);

	// (8ms -> 3ms)
	std::vector<float> barys(_m.nfacets() * 3);

	const int size = _m.nfacets();
	for (int fi = 0; fi < size; ++fi) {
		// Compute bary
		const int off = fi * 3;
		const vec3 &v0 = _m.points[_m.facets[off]];	
		const vec3 &v1 = _m.points[_m.facets[off + 1]];
		const vec3 &v2 = _m.points[_m.facets[off + 2]];

		barys[fi * 3] = (v0.x + v1.x + v2.x) / 3;
		barys[fi * 3 + 1] = (v0.y + v1.y + v2.y) / 3;
		barys[fi * 3 + 2] = (v0.z + v1.z + v2.z) / 3;
	}


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
	// glBufferStorage(GL_TEXTURE_BUFFER, hex.ncells() * sizeof(float), nullptr, flags);
	// ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, hex.ncells() * sizeof(float), flags);

	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);
	// TODO remap facet highlight, filter...

}