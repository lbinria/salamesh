#include "halfedge_renderer.h"
#include "../../core/graphic_api.h"
#include "../helpers.h"

void HalfedgeRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// For the moment don't use persistent mapped memory
	// TODO clean DO THAT IN RENDERER ?
	sl::createTBO(bufHighlight, tboHighlight);
	sl::createTBO(bufFilter, tboFilter);	
	sl::createTBO(bufColormap0, tboColormap0);
	sl::createTBO(bufColormap1, tboColormap1);
	sl::createTBO(bufColormap2, tboColormap2);

	shader.use();
	shader.setInt("colormap0", 0);
	shader.setInt("colormap1", 1);
	shader.setInt("colormap2", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("filterBuf", 4);
	shader.setInt("colormap0Buf", 5);
	shader.setInt("colormap1Buf", 6);
	shader.setInt("colormap2Buf", 7);

	// VBO
	sl::createVBOInteger(shader.id, "halfedgeIndex", sizeof(LineVert), (void*)offsetof(LineVert, halfedgeIndex));
	sl::createVBOVec3(shader.id, "aP0", sizeof(LineVert), (void*)offsetof(LineVert, P0));
	sl::createVBOVec3(shader.id, "aP1", sizeof(LineVert), (void*)offsetof(LineVert, P1));
	sl::createVBOFloat(shader.id, "aSide", sizeof(LineVert), (void*)offsetof(LineVert, side));
	sl::createVBOFloat(shader.id, "aEnd", sizeof(LineVert), (void*)offsetof(LineVert, end));
	sl::createVBOVec3(shader.id, "bary", sizeof(LineVert), (void*)offsetof(LineVert, bary));

}

void HalfedgeRenderer::render(glm::vec3 &position) {

	// if (!visible)
	// 	return;

	// glBindVertexArray(VAO);

	// glActiveTexture(GL_TEXTURE0 + 0);
	// glBindTexture(GL_TEXTURE_2D, texColormap0);

	// glActiveTexture(GL_TEXTURE0 + 1);
	// glBindTexture(GL_TEXTURE_2D, texColormap1);

	// glActiveTexture(GL_TEXTURE0 + 2);
	// glBindTexture(GL_TEXTURE_2D, texColormap2);

	// glActiveTexture(GL_TEXTURE0 + 3);
	// glBindTexture(GL_TEXTURE_BUFFER, tboHighlight);

	// glActiveTexture(GL_TEXTURE0 + 4);
	// glBindTexture(GL_TEXTURE_BUFFER, tboFilter);

	// glActiveTexture(GL_TEXTURE0 + 5);
	// glBindTexture(GL_TEXTURE_BUFFER, tboColormap0);

	// glActiveTexture(GL_TEXTURE0 + 6);
	// glBindTexture(GL_TEXTURE_BUFFER, tboColormap1);

	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_BUFFER, tboColormap2);

	// setPosition(position);

	// glDrawArrays(GL_TRIANGLES, 0, nelements);
}

void HalfedgeRenderer::render(RendererView &rv, glm::vec3 &position) {
	if (!rv.visible)
		return;

	glBindVertexArray(VAO);
	rv.use(position);
	glDrawArrays(GL_TRIANGLES, 0, nelements);
}

void SurfaceHalfedgeRenderer::push() {

	// Lazy-loading of halfedges, 
	// only push on visible if needed
	// if (!visible) {
	// 	shouldPush = true;
	// 	return;
	// }
	
	std::vector<LineVert> vertices;
	// pre-allocate to speed-up
	vertices.reserve(_m.nfacets() * 4 /* reserve for 4 side facets */ * 6 /* 1 quad, 2 tri per quad, 3 points per tri */); 

	for (auto &f : _m.iter_facets()) {
		int facetSize = _m.facet_size(f);

		vec3 b;
		for (int lv = 0; lv < f.size(); ++lv) {
			b += f.vertex(lv).pos();
		}
		b /= f.size();
		glm::vec3 bary = sl::um2glm(b);

		for (int i = 0; i < facetSize; ++i) {
			
			auto v0 = f.vertex(i);
			auto v1 = f.vertex((i + 1) % facetSize);
			auto p0 = v0.pos();
			auto p1 = v1.pos();
			
			int halfedgeIdx = f * facetSize + i;

			const glm::vec3 gp0(p0.x, p0.y, p0.z);
			const glm::vec3 gp1(p1.x, p1.y, p1.z);

			// build the 4 “corner” vertices
			LineVert lv0{halfedgeIdx, gp0, gp1, -1.0f, 0.0f, bary};  // corner: start, left side
			LineVert lv1{halfedgeIdx, gp0, gp1, +1.0f, 0.0f, bary};  // corner: start, right side
			LineVert lv2{halfedgeIdx, gp0, gp1, -1.0f, 1.0f, bary};  // corner: end,   left side
			LineVert lv3{halfedgeIdx, gp0, gp1, +1.0f, 1.0f, bary};  // corner: end,   right side

			vertices.push_back(lv0);
			vertices.push_back(lv1);
			vertices.push_back(lv2);
			
			vertices.push_back(lv2);
			vertices.push_back(lv3);
			vertices.push_back(lv1);

		}
	}

	writeVBOBuffer(vertices);

	shouldPush = false;
}

void VolumeHalfedgeRenderer::push() {
	// nverts = 24 * _m.ncells() * 6;
	std::vector<LineVert> vertices;

	for (auto &c : _m.iter_cells()) {
		int nf = _m.nfacets_per_cell();

		for (int f = 0; f < nf; ++f) {
			int facetSize = _m.facet_size(f);

			// TODO here just manage tet / hex, should manage prism, pyramid, etc...
			// The best is to see if we can make refCell useless
			int refCell = facetSize == 3 ? 0 : 1;

			for (int i = 0; i < facetSize; ++i) {
				auto lc0 = reference_cells[refCell].facets[i % facetSize + facetSize * f];
				auto lc1 = reference_cells[refCell].facets[(i + 1) % facetSize + facetSize * f];
				auto v0 = c.corner(lc0).vertex();
				auto v1 = c.corner(lc1).vertex();
				vec3 p0 = v0;
				vec3 p1 = v1;

				glm::vec3 bary = sl::um2glm((p0 + p1) * .5);
				
				// int halfedgeIdx = c * 24 + f * i;
				int halfedgeIdx = c * (nf * facetSize) + f * i;

				const glm::vec3 gp0(p0.x, p0.y, p0.z);
				const glm::vec3 gp1(p1.x, p1.y, p1.z);

				// build the 4 “corner” vertices
				LineVert lv0{halfedgeIdx, gp0, gp1, -1.0f, 0.0f, bary};  // corner: start, left side
				LineVert lv1{halfedgeIdx, gp0, gp1, +1.0f, 0.0f, bary};  // corner: start, right side
				LineVert lv2{halfedgeIdx, gp0, gp1, -1.0f, 1.0f, bary};  // corner: end,   left side
				LineVert lv3{halfedgeIdx, gp0, gp1, +1.0f, 1.0f, bary};  // corner: end,   right side

				vertices.push_back(lv0);
				vertices.push_back(lv1);
				vertices.push_back(lv2);
				
				vertices.push_back(lv2);
				vertices.push_back(lv3);
				vertices.push_back(lv1);

			}
		}
	}

	nelements = vertices.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}

void PolylineRenderer::push() {
	// Create vertices
	/* 6 vertices per edges : 2 triangles for a rect with each 3 vertices */
	std::vector<LineVert> vertices(_m.nedges() * 6); 

	for (auto &e : _m.iter_edges()) {

		vec3 p0 = e.from().pos();
		vec3 p1 = e.to().pos();

		glm::vec3 bary = sl::um2glm((p0 + p1) * .5);

		LineVert lv0{e, sl::um2glm(p0), sl::um2glm(p1), -1.f, 0.f, bary};
		LineVert lv1{e, sl::um2glm(p0), sl::um2glm(p1), +1.f, 0.f, bary};
		LineVert lv2{e, sl::um2glm(p0), sl::um2glm(p1), -1.f, 1.f, bary};
		LineVert lv3{e, sl::um2glm(p0), sl::um2glm(p1), +1.f, 1.f, bary};

		vertices[e * 6 + 0] = lv0;
		vertices[e * 6 + 1] = lv1;
		vertices[e * 6 + 2] = lv2;

		vertices[e * 6 + 3] = lv2;
		vertices[e * 6 + 4] = lv3;
		vertices[e * 6 + 5] = lv1;
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
	nelements = vertices.size();
}

void HalfedgeRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(LineVert), nullptr, GL_STATIC_DRAW);
	nelements = 0;
}

void HalfedgeRenderer::clean() {
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
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}