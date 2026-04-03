#include "surface_renderer.h"
#include "../../core/graphic_api.h"

void SurfaceRenderer::init() {

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

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

void SurfaceRenderer::render(RendererView &rv, glm::vec3 &position) {
	if (!rv.visible)
		return;

	glBindVertexArray(VAO);
	rv.use(position);
	glDrawArrays(GL_TRIANGLES, 0, nelements);
}

void SurfaceRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
}

void SurfaceRenderer::clean() {
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