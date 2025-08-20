#include "quad_renderer.h"


void QuadRenderer::push() {

	#ifdef _DEBUG
	std::cout << "push start." << std::endl;
	#endif

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	nverts = _m.nfacets() * 6 /* 3 points per tri, 2 tri per facet */;

	// (8ms -> 3ms)
	std::vector<float> barys(_m.nfacets() * 3);

	const int size = _m.nfacets();
	for (int fi = 0; fi < size; ++fi) {
		// Compute bary
		const int off = fi * 4;
		const vec3 &v0 = _m.points[_m.facets[off]];	
		const vec3 &v1 = _m.points[_m.facets[off + 1]];
		const vec3 &v2 = _m.points[_m.facets[off + 2]];
		const vec3 &v3 = _m.points[_m.facets[off + 3]];

		barys[fi * 3] = (v0.x + v1.x + v2.x + v3.x) / 4;
		barys[fi * 3 + 1] = (v0.y + v1.y + v2.y + v3.y) / 4;
		barys[fi * 3 + 2] = (v0.z + v1.z + v2.z + v3.z) / 4;
	}

	constexpr int verts[2][3] = {{0, 1, 3}, {2, 3, 1}};

	std::vector<Vertex> vertices;
	for (auto &f : _m.iter_facets()) {

		// Get 4 points of facet
		const vec3 points[4] = {
			f.vertex(0).pos(),
			f.vertex(1).pos(),
			f.vertex(2).pos(),
			f.vertex(3).pos()
		};

		// Compute normal
		Quad3 q = f;
		auto n = q.normal();


		for (int t = 0; t < 2; ++t) {

			const double a = (points[verts[t][1]] - points[verts[t][0]]).norm();
			const double b = (points[verts[t][2]] - points[verts[t][1]]).norm();
			const double c = (points[verts[t][2]] - points[verts[t][0]]).norm();

			const double side_lengths[] = {a, b, c};
			const double s = a + b + c;
			const double area = sqrt(s * (s - a) * (s - b) * (s - c));

			for (int j = 0; j < 3; ++j) {

				auto lv = verts[t][j];

				auto v = f.vertex(lv);
				auto p = v.pos();

				// Compute height
				const double h = area / (.5 * side_lengths[(lv + 1) % 3]);

				glm::vec3 edge(0.f, 0.f, 0.f);
				edge[j] = h;
				// Exclude first tri height in order unmesh one side of the triangle (the diagonal),
				// It enable to obtain quad meshing
				edge[0] = std::numeric_limits<float>::max();


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