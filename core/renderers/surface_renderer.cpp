#include "surface_renderer.h"
#include "../../helpers/graphic_api.h"

void SurfaceRenderer::init() {

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// For the moment don't use persistent mapped memory
	sl::createTBO(bufAttr, texAttr, 2);
	sl::createTBO(bufHighlight, texHighlight, 3);
	sl::createTBO(bufFilter, texFilter, 4);

	// Set up texture units		
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.use();
	shader.setInt("attributeData", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("filterBuf", 4);

	#ifdef _DEBUG
	std::cout << "vertex attrib setup..." << std::endl;
	#endif


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	GLuint p0Loc = glGetAttribLocation(shader.id, "p0");
	glEnableVertexAttribArray(p0Loc);
	glVertexAttribPointer(p0Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p0));

	GLuint p1Loc = glGetAttribLocation(shader.id, "p1");
	glEnableVertexAttribArray(p1Loc);
	glVertexAttribPointer(p1Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p1));

	GLuint p2Loc = glGetAttribLocation(shader.id, "p2");
	glEnableVertexAttribArray(p2Loc);
	glVertexAttribPointer(p2Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p2));

	GLuint facetIndexLoc = glGetAttribLocation(shader.id, "facetIndex");
	glEnableVertexAttribArray(facetIndexLoc);
	glVertexAttribIPointer(facetIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));

	GLuint localIndexLoc = glGetAttribLocation(shader.id, "localIndex");
	glEnableVertexAttribArray(localIndexLoc);
	glVertexAttribIPointer(localIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, localIndex));

	GLuint cornerIndexLoc = glGetAttribLocation(shader.id, "cornerIndex");
	glEnableVertexAttribArray(cornerIndexLoc);
	glVertexAttribIPointer(cornerIndexLoc, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));
	
	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

	#ifdef _DEBUG
	std::cout << "mesh setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
	#endif

}

void SurfaceRenderer::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_1D, texColorMap);

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

void SurfaceRenderer::clean() {
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