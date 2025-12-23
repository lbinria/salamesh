#include "surface_renderer.h"
#include "../../helpers/graphic_api.h"

void SurfaceRenderer::init() {

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// For the moment don't use persistent mapped memory
	sl::createTBO(bufAttr, texAttr, 1);
	sl::createTBO(bufHighlight, texHighlight, 2);
	sl::createTBO(bufFilter, texFilter, 3);

	sl::createTBO(bufColormap0, texColormap0, 4);
	sl::createTBO(bufColormap1, texColormap1, 5);
	sl::createTBO(bufColormap2, texColormap2, 6);

	// TODO seems useless
	// Set up texture units		
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texColormap0);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_BUFFER, texColormap1);

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_BUFFER, texColormap2);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	shader.use();
	shader.setInt("attrBuf", 1);
	shader.setInt("highlightBuf", 2);
	shader.setInt("filterBuf", 3);
	shader.setInt("colormap0Buf", 4);
	shader.setInt("colormap1Buf", 5);
	shader.setInt("colormap2Buf", 6);

	#ifdef _DEBUG
	std::cout << "vertex attrib setup..." << std::endl;
	#endif


	// TODO below ??? needeed ??? i don't think !
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
	glBindTexture(GL_TEXTURE_BUFFER, texAttr);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_BUFFER, texHighlight);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_BUFFER, texFilter);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_BUFFER, texColormap0);

	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_BUFFER, texColormap1);

	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_BUFFER, texColormap2);



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

	glDeleteBuffers(1, &bufAttr);
	glDeleteTextures(1, &texAttr);
	glDeleteBuffers(1, &bufHighlight);
	glDeleteTextures(1, &texHighlight);
	glDeleteBuffers(1, &bufFilter);
	glDeleteTextures(1, &texFilter);
	glDeleteBuffers(1, &bufColormap0);
	glDeleteTextures(1, &texColormap0);
	glDeleteBuffers(1, &bufColormap1);
	glDeleteTextures(1, &texColormap1);
	glDeleteBuffers(1, &bufColormap2);
	glDeleteTextures(1, &texColormap2);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}