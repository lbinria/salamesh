#include "halfedge_renderer_2.h"

void HalfedgeRenderer2::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	
	// For the moment don't use persistent mapped memory
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
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	
	shader.use();
	// shader.setInt("attrBuf", 2);
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

	// GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	// glEnableVertexAttribArray(vertexIndexLocation);
	// glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	// GLuint sizeLocation = glGetAttribLocation(shader.id, "sizeScale");
	// glEnableVertexAttribArray(sizeLocation);
	// glVertexAttribPointer(sizeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, size));

}

void HalfedgeRenderer2::push() {	

	
	nverts = s.nfacets() * 3 * 6;
	std::vector<LineVert> vertices;

	for (auto &f : s.iter_facets()) {
		for (int i = 0; i < 3; ++i) {
			// auto lc0 = reference_cells[1].facets[i % 4 + 4 * f];
			// auto lc1 = reference_cells[1].facets[(i + 1) % 4 + 4 * f];
			
			auto v0 = f.vertex(i);
			auto v1 = f.vertex((i + 1) % 3);
			auto p0 = v0.pos();
			auto p1 = v1.pos();
			
			int halfedgeIdx = f * 3 + i;

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

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nverts * sizeof(LineVert), vertices.data(), GL_STATIC_DRAW);
}

void HalfedgeRenderer2::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

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

void HalfedgeRenderer2::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// TODO clean buffers, textures, unmap ptr...

	shader.clean();
}