#include "halfedge_renderer.h"
#include "../../core/graphic_api.h"
#include "../helpers.h"

void HalfedgeRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// For the moment don't use persistent mapped memory
	sl::createTBO(bufAttr, texAttr);
	sl::createTBO(bufHighlight, tboHighlight);
	sl::createTBO(bufFilter, tboFilter);
	
	// TODO clean DO THAT IN RENDERER ?
	sl::createTBO(bufColormap0, tboColormap0);
	sl::createTBO(bufColormap1, tboColormap1);
	sl::createTBO(bufColormap2, tboColormap2);

	shader.use();
	shader.setInt("colormap", 0);
	shader.setInt("colormap0", 1);
	shader.setInt("colormap1", 2);
	shader.setInt("colormap2", 3);

	shader.setInt("attrBuf", 4);
	shader.setInt("highlightBuf", 5);
	shader.setInt("filterBuf", 6);
	shader.setInt("colormap0Buf", 7);
	shader.setInt("colormap1Buf", 8);
	shader.setInt("colormap2Buf", 9);

	// VBO
	sl::createVBOInteger(shader.id, "halfedgeIndex", sizeof(LineVert), (void*)offsetof(LineVert, halfedgeIndex));
	sl::createVBOVec3(shader.id, "aP0", sizeof(LineVert), (void*)offsetof(LineVert, P0));
	sl::createVBOVec3(shader.id, "aP1", sizeof(LineVert), (void*)offsetof(LineVert, P1));
	sl::createVBOFloat(shader.id, "aSide", sizeof(LineVert), (void*)offsetof(LineVert, side));
	sl::createVBOFloat(shader.id, "aEnd", sizeof(LineVert), (void*)offsetof(LineVert, end));
	sl::createVBOVec3(shader.id, "bary", sizeof(LineVert), (void*)offsetof(LineVert, bary));

}

void HalfedgeRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texColorMap);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texColormap0);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texColormap1);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, texColormap2);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_BUFFER, tboHighlight);

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_BUFFER, tboFilter);

	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_BUFFER, tboColormap0);

	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_BUFFER, tboColormap1);

	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_BUFFER, tboColormap2);

	setPosition(position);

	glDrawArrays(GL_TRIANGLES, 0, nverts);
}

void SurfaceHalfedgeRenderer::push() {	
	
	std::vector<LineVert> vertices;

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

			// build the 4 “corner” vertices
			LineVert lv0{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 0.0f, bary};  // corner: start, left side
			LineVert lv1{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), +1.0f, 0.0f, bary};  // corner: start, right side
			LineVert lv2{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 1.0f, bary};  // corner: end,   left side
			LineVert lv3{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), +1.0f, 1.0f, bary};  // corner: end,   right side

			vertices.push_back(lv0);
			vertices.push_back(lv1);
			vertices.push_back(lv2);
			
			vertices.push_back(lv2);
			vertices.push_back(lv3);
			vertices.push_back(lv1);

		}
	}

	nverts = vertices.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
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
				auto p0 = v0.pos();
				auto p1 = v1.pos();
				
				// int halfedgeIdx = c * 24 + f * i;
				int halfedgeIdx = c * (nf * facetSize) + f * i;

				// build the 4 “corner” vertices
				LineVert lv0{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 0.0f};  // corner: start, left side
				LineVert lv1{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), +1.0f, 0.0f};  // corner: start, right side
				LineVert lv2{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), -1.0f, 1.0f};  // corner: end,   left side
				LineVert lv3{halfedgeIdx, glm::vec3(p0.x, p0.y, p0.z), glm::vec3(p1.x, p1.y, p1.z), +1.0f, 1.0f};  // corner: end,   right side

				vertices.push_back(lv0);
				vertices.push_back(lv1);
				vertices.push_back(lv2);
				
				vertices.push_back(lv2);
				vertices.push_back(lv3);
				vertices.push_back(lv1);

			}
		}
	}

	nverts = vertices.size();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}

void HalfedgeRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(LineVert), nullptr, GL_STATIC_DRAW);
	nverts = 0;
}

void HalfedgeRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &tboHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &tboFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.clean();
}