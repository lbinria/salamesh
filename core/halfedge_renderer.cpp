#include "halfedge_renderer.h"
#include "../helpers/graphic_api.h"

void HalfedgeRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// For the moment don't use persistent mapped memory
	sl::createTBO(bufAttr, texAttr, 2);


	glGenBuffers(1, &bufHighlight);
	glGenTextures(1, &texHighlight);
	glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);
	glActiveTexture(GL_TEXTURE0 + 3); 
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);

	// For the moment don't use persistent mapped memory
	glGenBuffers(1, &bufFilter);
	glGenTextures(1, &texFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);
	glActiveTexture(GL_TEXTURE0 + 4); 
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufFilter);

	// WTF ?
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	
	shader.use();
	shader.setInt("attrBuf", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("filterBuf", 4);

	// VBO
	GLuint halfedgeIndexLoc = glGetAttribLocation(shader.id, "halfedgeIndex");
	glEnableVertexAttribArray(halfedgeIndexLoc);
	glVertexAttribIPointer(halfedgeIndexLoc, 1, GL_INT, sizeof(LineVert), (void*)offsetof(LineVert, halfedgeIndex));

	GLuint p0Loc = glGetAttribLocation(shader.id, "aP0");
	glEnableVertexAttribArray(p0Loc);
	glVertexAttribPointer(p0Loc, 3, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, P0));

	GLuint p1Loc = glGetAttribLocation(shader.id, "aP1");
	glEnableVertexAttribArray(p1Loc);
	glVertexAttribPointer(p1Loc, 3, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, P1));

	GLuint sideLoc = glGetAttribLocation(shader.id, "aSide");
	glEnableVertexAttribArray(sideLoc);
	glVertexAttribPointer(sideLoc, 1, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, side));

	GLuint endLoc = glGetAttribLocation(shader.id, "aEnd");
	glEnableVertexAttribArray(endLoc);
	glVertexAttribPointer(endLoc, 1, GL_FLOAT, GL_FALSE, sizeof(LineVert), (void*)offsetof(LineVert, end));

}

void HalfedgeRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

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

void HalfedgeRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteBuffers(1, &bufBary);
	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &texHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &texFilter);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.clean();
}

void SurfaceHalfedgeRenderer::push() {	
	
	std::vector<LineVert> vertices;

	for (auto &f : _m.iter_facets()) {
		int facetSize = _m.facet_size(f);
		for (int i = 0; i < facetSize; ++i) {
			
			auto v0 = f.vertex(i);
			auto v1 = f.vertex((i + 1) % facetSize);
			auto p0 = v0.pos();
			auto p1 = v1.pos();
			
			int halfedgeIdx = f * facetSize + i;

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