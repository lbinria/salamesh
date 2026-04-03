#include "point_set_renderer.h"
#include "../../core/graphic_api.h"

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	
	// // For the moment don't use persistent mapped memory
	// // TODO clean DO THAT IN RENDERER ?
	// sl::createTBO(bufHighlight, tboHighlight);
	// sl::createTBO(bufFilter, tboFilter);
	// sl::createTBO(bufColormap0, tboColormap0);
	// sl::createTBO(bufColormap1, tboColormap1);
	// sl::createTBO(bufColormap2, tboColormap2);

	// shader.use();
	// shader.setInt("colormap0", 0);
	// shader.setInt("colormap1", 1);
	// shader.setInt("colormap2", 2);
	// shader.setInt("highlightBuf", 3);
	// shader.setInt("filterBuf", 4);
	// shader.setInt("colormap0Buf", 5);
	// shader.setInt("colormap1Buf", 6);
	// shader.setInt("colormap2Buf", 7);

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

	writeVBOBuffer(vertices);
}

void PointSetRenderer::render(RendererView &rv, glm::vec3 &position) {
	if (!rv.visible)
		return;

	glBindVertexArray(VAO);
	rv.use(position);
	glDrawArrays(GL_POINTS, 0, nelements);
}

void PointSetRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	clearPoints();
}

void PointSetRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// glDeleteBuffers(1, &bufHighlight);
	// glDeleteTextures(1, &tboHighlight);
	// glDeleteBuffers(1, &bufFilter);
	// glDeleteTextures(1, &tboFilter);
	// glDeleteBuffers(1, &bufColormap0);
	// glDeleteTextures(1, &tboColormap0);
	// glDeleteBuffers(1, &bufColormap1);
	// glDeleteTextures(1, &tboColormap1);
	// glDeleteBuffers(1, &bufColormap2);
	// glDeleteTextures(1, &tboColormap2);
	// glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// // Clean shader
	// shader.clean();
}