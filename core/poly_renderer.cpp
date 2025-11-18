#include "poly_renderer.h"
#include "../helpers/graphic_api.h"

void PolyRenderer::init() {

	// TODO maybe update buffer size of ptr on push ? move ncells somewher eelse

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// glGenBuffers(1, &bufBary);
	// glGenTextures(1, &texBary);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufBary);
	// glActiveTexture(GL_TEXTURE0 + 1); 
	// glBindTexture(GL_TEXTURE_BUFFER, texBary);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, bufBary);

	// glGenBuffers(1, &bufAttr);
	// glGenTextures(1, &texAttr);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	// glActiveTexture(GL_TEXTURE0 + 2); 
	// glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glGenBuffers(1, &bufAttr);
	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	glBufferStorage(GL_TEXTURE_BUFFER, _m.nfacets() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	ptrAttr = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.nfacets() * sizeof(float), flags);

	glGenTextures(1, &texAttr);
	glActiveTexture(GL_TEXTURE0 + 2); 
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);

	// glGenBuffers(1, &bufHighlight);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// glBufferStorage(GL_TEXTURE_BUFFER, _m.nfacets() * sizeof(float), nullptr, flags);
	// // Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	// ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.nfacets() * sizeof(float), flags);

	// glGenTextures(1, &texHighlight);
	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);

	// glGenBuffers(1, &bufFilter);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
	// glBufferStorage(GL_TEXTURE_BUFFER, _m.nfacets() * sizeof(float), nullptr, flags);
	// // Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	// ptrFilter = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, _m.nfacets() * sizeof(float), flags);

	// glGenTextures(1, &texFilter);
	// glActiveTexture(GL_TEXTURE0 + 5); 
	// glBindTexture(GL_TEXTURE_BUFFER, texFilter);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufFilter);

	// TODO factorize to opengl function createTBO
	// For the moment don't use persistent mapped memory
	// glGenBuffers(1, &bufHighlight);
	// glGenTextures(1, &texHighlight);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);

	sl::createTBO(bufHighlight, texHighlight, 3);

	// For the moment don't use persistent mapped memory
	// glGenBuffers(1, &bufFilter);
	// glGenTextures(1, &texFilter);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
	// glActiveTexture(GL_TEXTURE0 + 4); 
	// glBindTexture(GL_TEXTURE_BUFFER, texFilter);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufFilter);

	sl::createTBO(bufFilter, texFilter, 4);






	// Set up texture units	
	// glActiveTexture(GL_TEXTURE0 + 1);
	// glBindTexture(GL_TEXTURE_BUFFER, texBary);
	
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.use();
	// shader.setInt("bary", 1);
	shader.setInt("attributeData", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("filterBuf", 4);

	#ifdef _DEBUG
	std::cout << "vertex attrib setup..." << std::endl;
	#endif

	// TODO create mesh_renderer with virual function setupVBO
	// TODO factorize to opengl function setupAttrib
	GLuint pLoc = glGetAttribLocation(shader.id, "p");
	GLuint nLoc = glGetAttribLocation(shader.id, "n");
	GLuint bLoc = glGetAttribLocation(shader.id, "b");
	GLuint hLoc = glGetAttribLocation(shader.id, "h");
	GLuint facetIndexLocation = glGetAttribLocation(shader.id, "facetIndex");
	GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	GLuint localIndexLocation = glGetAttribLocation(shader.id, "localIndex");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(pLoc);
	glVertexAttribPointer(pLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p));

	glEnableVertexAttribArray(nLoc);
	glVertexAttribPointer(nLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, n));

	glEnableVertexAttribArray(bLoc);
	glVertexAttribPointer(bLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, b));

	glEnableVertexAttribArray(hLoc);
	glVertexAttribPointer(hLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, h));

	glEnableVertexAttribArray(facetIndexLocation);
	glVertexAttribIPointer(facetIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));

	glEnableVertexAttribArray(vertexIndexLocation);
	glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	glEnableVertexAttribArray(localIndexLocation);
	glVertexAttribIPointer(localIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
	std::cout << "mesh setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	#endif

}


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

			for (int i = 0; i < 3; ++i) {

				// Retrieve vertex id (0 is always the barycenter => no vertex associated)
				int v = i == 0 ? -1 : f.vertex((lv + (i - 1)) % nv);

				auto reverseI = (3 - i) % 3;
				auto reverseI1 = (3 - i + 1) % 3;
				auto reverseI2 = (3 - i + 2) % 3;

				auto p = verts[i];
				// auto p0 = verts[reverseI];
				// auto p1 = verts[(reverseI + 1) % 3];
				// auto p2 = verts[(reverseI + 2) % 3];
				auto p0 = verts[reverseI];
				auto p1 = verts[reverseI1];
				auto p2 = verts[reverseI2];

				// // Compute the projected point from vertex to base
				// auto s0 = p0 - p2;
				// auto s1 = p1 - p2;
				// auto length = s1.normalized() * s0;
				// auto bh = p2 + s1.normalized() * length;

				// Compute the projected point from vertex to base
				auto s0 = p2 - p1;
				auto s1 = p0 - p1;
				auto length = s0.normalized() * s1; // Project s1 onto s0
				auto bh = p1 + s0.normalized() * length;

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
					.p = glm::vec3(p.x, p.y, p.z),
					.n = glm::vec3(n.x, n.y, n.z),
					.b = bary,
					.h = glm::vec3(bh.x, bh.y, bh.z)
					// .h = hh
				});
			}
		}
	}

	PolyRenderer::push(vertices);

}

void PolyRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texColorMap);

	// glActiveTexture(GL_TEXTURE0 + 1);
	// glBindTexture(GL_TEXTURE_BUFFER, texBary);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	
	// Draw	
	shader.use();
	shader.setMat4("model", model);
	glDrawArrays(GL_TRIANGLES, 0, nverts);
}

void PolyRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// Unmap highlight
	// if (ptrHighlight) {
	// 	glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	// 	glUnmapBuffer(GL_TEXTURE_BUFFER);
	// 	ptrHighlight = nullptr;
	// }

	// Unmap filter
	// if (ptrFilter) {
	// 	glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
	// 	glUnmapBuffer(GL_TEXTURE_BUFFER);
	// 	ptrFilter = nullptr;
	// }

	glDeleteBuffers(1, &bufBary);
	// glDeleteTextures(1, &texBary);
	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &texHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &texFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}