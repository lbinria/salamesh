#include "point_set_renderer.h"

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	
	// glGenBuffers(1, &bufAttr);
	// glGenTextures(1, &texAttr);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	// glActiveTexture(GL_TEXTURE0 + 2);
	// glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);

	GLbitfield flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

	glGenBuffers(1, &bufAttr);
	glBindBuffer(GL_TEXTURE_BUFFER, bufAttr);
	glBufferStorage(GL_TEXTURE_BUFFER, ps.size() * sizeof(float), nullptr, flags);
	// Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	ptrAttr = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, ps.size() * sizeof(float), flags);

	glGenTextures(1, &texAttr);
	glActiveTexture(GL_TEXTURE0 + 2); 
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufAttr);

	// Highlight
	// glGenBuffers(1, &bufHighlight);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufHighlight);

	// glBufferStorage(GL_TEXTURE_BUFFER, ps.size() * sizeof(float), nullptr, flags);
	// // Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	// ptrHighlight = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, ps.size() * sizeof(float), flags);

	// glGenTextures(1, &texHighlight);
	// glActiveTexture(GL_TEXTURE0 + 3); 
	// glBindTexture(GL_TEXTURE_BUFFER, texHighlight);
	// glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, bufHighlight);

	// // Filter
	// glGenBuffers(1, &bufFilter);
	// glBindBuffer(GL_TEXTURE_BUFFER, bufFilter);

	// glBufferStorage(GL_TEXTURE_BUFFER, ps.size() * sizeof(float), nullptr, flags);
	// // Map once and keep pointer (not compatible for MacOS... because need OpenGL >= 4.6 i think)
	// ptrFilter = (float*)glMapBufferRange(GL_TEXTURE_BUFFER, 0, ps.size() * sizeof(float), flags);

	// glGenTextures(1, &texFilter);
	// glActiveTexture(GL_TEXTURE0 + 4); 
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


	shader.use();
	shader.setInt("attrBuf", 2);
	shader.setInt("highlightBuf", 3);
	shader.setInt("filterBuf", 4);

	// VBO
	GLuint vertexIndexLocation = glGetAttribLocation(shader.id, "vertexIndex");
	glEnableVertexAttribArray(vertexIndexLocation);
	glVertexAttribIPointer(vertexIndexLocation, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));

	GLuint positionLocation = glGetAttribLocation(shader.id, "aPos");
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

	GLuint sizeLocation = glGetAttribLocation(shader.id, "sizeScale");
	glEnableVertexAttribArray(sizeLocation);
	glVertexAttribPointer(sizeLocation, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, size));

}

void PointSetRenderer::push() {	

	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = glm::vec3(v.x, v.y, v.z),
			.size = 1.f
		};
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}

void PointSetRenderer::render(glm::vec3 &position) {

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

	glDrawArrays(GL_POINTS, 0, ps.size());
}

void PointSetRenderer::clean() {
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