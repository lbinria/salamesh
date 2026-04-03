#include "poly_renderer.h"
#include "../../core/graphic_api.h"

void PolyRenderer::init() {

	// TODO maybe update buffer size of ptr on push ? move ncells somewher eelse

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	sl::createTBO(bufNVertsPerFacet, texNVertsPerFacet);

	#ifdef _DEBUG
	std::cout << "vertex attrib setup..." << std::endl;
	#endif

	// TODO create mesh_renderer with virual function setupVBO
	// TODO factorize to opengl function setupAttrib

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLuint vertexIndexLoc = glGetAttribLocation(shader.id, "vertexIndex");
	glEnableVertexAttribArray(vertexIndexLoc);
	glVertexAttribIPointer(vertexIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	GLuint localIndexLoc = glGetAttribLocation(shader.id, "localIndex");
	glEnableVertexAttribArray(localIndexLoc);
	glVertexAttribIPointer(localIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, localIndex));

	GLuint cornerIndexLoc = glGetAttribLocation(shader.id, "cornerIndex");
	glEnableVertexAttribArray(cornerIndexLoc);
	glVertexAttribIPointer(cornerIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));

	GLuint cornerOffLoc = glGetAttribLocation(shader.id, "cornerOff");
	glEnableVertexAttribArray(cornerOffLoc);
	glVertexAttribIPointer(cornerOffLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, cornerOff));

	GLuint facetIndexLoc = glGetAttribLocation(shader.id, "facetIndex");
	glEnableVertexAttribArray(facetIndexLoc);
	glVertexAttribIPointer(facetIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));

	GLuint pLoc = glGetAttribLocation(shader.id, "p");
	glEnableVertexAttribArray(pLoc);
	glVertexAttribPointer(pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p));

	GLuint p0Loc = glGetAttribLocation(shader.id, "p0");
	glEnableVertexAttribArray(p0Loc);
	glVertexAttribPointer(p0Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p0));

	GLuint p1Loc = glGetAttribLocation(shader.id, "p1");
	glEnableVertexAttribArray(p1Loc);
	glVertexAttribPointer(p1Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p1));

	GLuint p2Loc = glGetAttribLocation(shader.id, "p2");
	glEnableVertexAttribArray(p2Loc);
	glVertexAttribPointer(p2Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p2));

	GLuint nLoc = glGetAttribLocation(shader.id, "n");
	glEnableVertexAttribArray(nLoc);
	glVertexAttribPointer(nLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));

	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
	std::cout << "mesh setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	#endif

}

void PolyRenderer::push() {
 
	std::vector<float> nVertsPerFacet(_m.nfacets());

	// Compute number of triangles needed to represent a facet
	int ntri = 0;
	for (auto &f : _m.iter_facets()) {
		int nvertsFacet = f.size();
		ntri += nvertsFacet;
		nVertsPerFacet[f] = static_cast<float>(nvertsFacet);
	}
	nelements = 3 * ntri /* 3 points per tri, n tri per facet */;

	int cornerOff = 0;
	std::vector<Vertex> vertices;
	for (auto &f : _m.iter_facets()) {

		// There is as much triangles as vertices in facet,
		int nv = f.size();
		const int ntri = nv;

		// Compute bary
		glm::vec3 bary{0.};
		for (int v = 0; v < nv; ++v) {
			auto pos = f.vertex(v).pos();
			bary += glm::vec3{pos.x, pos.y, pos.z};
		}
		bary /= nv;

		// Compute normal
		std::vector<vec3> pts(nv);
		for (int v = 0; v < nv; ++v) {
			pts[v] = f.vertex(v).pos();
		}
		vec3 n = geo::normal(pts.data(), nv);

		

		for (int t = 0; t < ntri; ++t) {

			const int lv = t;
			// Three points of current triangle
			vec3 verts[3] = {vec3(bary.x, bary.y, bary.z) , f.vertex(lv).pos(), f.vertex((lv + 1) % nv).pos()};

			// Compute first corner index of the triangle
			int firstCornerIdx = cornerOff + lv;

			for (int i = 0; i < 3; ++i) {

				// Retrieve vertex id (0 is always the barycenter => no vertex associated)
				int v = i == 0 ? -1 : f.vertex((lv + (i - 1)) % nv);

				auto p = verts[i];
				auto p1 = verts[1];
				auto p2 = verts[2];

				vertices.push_back({
					.vertexIndex = v, // useless i think
					.localIndex = i,
					// .cornerIndex = firstCornerIdx,
					.cornerIndex = lv,
					.cornerOff = cornerOff,
					.facetIndex = f,
					.p = glm::vec3(p.x, p.y, p.z),
					.p0 = bary,
					.p1 = glm::vec3(p1.x, p1.y, p1.z),
					.p2 = glm::vec3(p2.x, p2.y, p2.z),
					.n = glm::vec3(n.x, n.y, n.z)
				});
			}
		}

		cornerOff += f.size();
	}

	writeVBOBuffer(vertices);

	// Write buffer
	glBindBuffer(GL_TEXTURE_BUFFER, bufNVertsPerFacet);
	glBufferData(GL_TEXTURE_BUFFER, nVertsPerFacet.size() * sizeof(float), nVertsPerFacet.data(), GL_STATIC_DRAW);
}

void PolyRenderer::render(RendererView &rv, glm::vec3 &position) {
	if (!rv.visible)
		return;

	glBindVertexArray(VAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	auto &prv = static_cast<PolyRendererView&>(rv);
	prv.setNVertsPerFacetBuf(8, texNVertsPerFacet);

	rv.use(shader);
	shader.setMat4("model", model);

	glDrawArrays(GL_TRIANGLES, 0, nelements);
}

void PolyRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
	nelements = 0;
}

void PolyRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &tboHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &tboFilter);
	glDeleteBuffers(1, &bufColormap0);
	glDeleteTextures(1, &tboColormap0);
	glDeleteBuffers(1, &bufColormap1);
	glDeleteTextures(1, &tboColormap1);
	glDeleteBuffers(1, &bufColormap2);
	glDeleteTextures(1, &tboColormap2);
	glDeleteBuffers(1, &bufNVertsPerFacet);
	glDeleteTextures(1, &texNVertsPerFacet);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}