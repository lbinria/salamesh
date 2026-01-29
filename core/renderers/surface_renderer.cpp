#include "surface_renderer.h"
#include "../../core/graphic_api.h"

void SurfaceRenderer::init() {

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

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

	#ifdef _DEBUG
	std::cout << "vertex attrib setup..." << std::endl;
	#endif


	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	sl::createVBOVec3(shader.id, "p0", sizeof(Vertex), (void*)offsetof(Vertex, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(Vertex), (void*)offsetof(Vertex, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(Vertex), (void*)offsetof(Vertex, p2));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));
	
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

void SurfaceRenderer::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
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