#include "point_set_renderer.h"
#include "../../helpers/graphic_api.h"

void PointSetRenderer::init() {

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
	sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
	sl::createVBOVec3(shader.id, "aPos", sizeof(Vertex), (void*)offsetof(Vertex, position));
	sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));

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

	glDrawArrays(GL_POINTS, 0, ps.size());
}

void PointSetRenderer::clean() {
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

	// Clean shader
	shader.clean();
}