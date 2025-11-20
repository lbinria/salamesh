#include "surface_renderer.h"

void SurfaceRenderer::init() {

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

	GLuint p0Loc = glGetAttribLocation(shader.id, "p0");
	GLuint p1Loc = glGetAttribLocation(shader.id, "p1");
	GLuint p2Loc = glGetAttribLocation(shader.id, "p2");
	GLuint facetIndexLocation = glGetAttribLocation(shader.id, "facetIndex");
	GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	GLuint localIndexLocation = glGetAttribLocation(shader.id, "localIndex");

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glEnableVertexAttribArray(p0Loc);
	glVertexAttribPointer(p0Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p0));

	glEnableVertexAttribArray(p1Loc);
	glVertexAttribPointer(p1Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p1));

	glEnableVertexAttribArray(p2Loc);
	glVertexAttribPointer(p2Loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, p2));

	glEnableVertexAttribArray(facetIndexLocation);
	glVertexAttribIPointer(facetIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));

	// glEnableVertexAttribArray(vertexIndexLocation);
	// glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	glEnableVertexAttribArray(localIndexLocation);
	glVertexAttribIPointer(localIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, localIndex));

	
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