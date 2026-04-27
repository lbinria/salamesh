#include "surface_renderer2.h"
#include "../../core/graphic_api.h"

// void SurfaceRenderer2::init() {

// 	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

// 	glGenVertexArrays(1, &VAO);
// 	glGenBuffers(1, &VBO);

// 	// For the moment don't use persistent mapped memory
// 	// TODO clean DO THAT IN RENDERER ?
// 	sl::createTBO(bufHighlight, tboHighlight);
// 	sl::createTBO(bufFilter, tboFilter);
// 	sl::createTBO(bufColormap0, tboColormap0);
// 	sl::createTBO(bufColormap1, tboColormap1);
// 	sl::createTBO(bufColormap2, tboColormap2);

// 	sl::createTBO(bufPoints, tboPoints, GL_RGB32F);
// 	sl::createTBO(bufFacets, tboFacets, GL_R32I);
// 	sl::createTBO(bufOffsets, tboOffsets, GL_R32I);
// 	sl::createTBO(bufFacetIndexes, tboFacetIndexes, GL_R32I);

// 	shader.use();

// 	shader.setInt("colormap0", 0);
// 	shader.setInt("colormap1", 1);
// 	shader.setInt("colormap2", 2);
// 	shader.setInt("highlightBuf", 3);
// 	shader.setInt("filterBuf", 4);
// 	shader.setInt("colormap0Buf", 5);
// 	shader.setInt("colormap1Buf", 6);
// 	shader.setInt("colormap2Buf", 7);
// 	shader.setInt("bufPoints", 8);
// 	shader.setInt("bufFacets", 9);
// 	shader.setInt("bufOffsets", 10);
// 	shader.setInt("bufFacetIndexes", 11);


	
// 	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

// 	#ifdef _DEBUG
// 	std::cout << "mesh setup in: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms" << std::endl;
// 	#endif

// }

// void SurfaceRenderer2::render(RendererView &rv, glm::vec3 &position) {

// 	glBindVertexArray(VAO);

// 	glActiveTexture(GL_TEXTURE0 + 0);
// 	glBindTexture(GL_TEXTURE_2D, texColormap0);

// 	glActiveTexture(GL_TEXTURE0 + 1);
// 	glBindTexture(GL_TEXTURE_2D, texColormap1);

// 	glActiveTexture(GL_TEXTURE0 + 2);
// 	glBindTexture(GL_TEXTURE_2D, texColormap2);

// 	glActiveTexture(GL_TEXTURE0 + 3);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboHighlight);

// 	glActiveTexture(GL_TEXTURE0 + 4);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboFilter);

// 	glActiveTexture(GL_TEXTURE0 + 5);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboColormap0);

// 	glActiveTexture(GL_TEXTURE0 + 6);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboColormap1);

// 	glActiveTexture(GL_TEXTURE0 + 7);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboColormap2);

// 	glActiveTexture(GL_TEXTURE0 + 8);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboPoints);

// 	glActiveTexture(GL_TEXTURE0 + 9);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboFacets);

// 	glActiveTexture(GL_TEXTURE0 + 10);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboOffsets);

// 	glActiveTexture(GL_TEXTURE0 + 11);
// 	glBindTexture(GL_TEXTURE_BUFFER, tboFacetIndexes);

// 	// setPosition(position);

// 	glDrawArrays(GL_TRIANGLES, 0, nelements);
// }

void SurfaceRenderer2::clear() {

}

// void SurfaceRenderer2::clean() {
// 	// Clean up
// 	glDeleteVertexArrays(1, &VAO);
// 	glDeleteBuffers(1, &VBO);

// 	glDeleteBuffers(1, &bufHighlight);
// 	glDeleteTextures(1, &tboHighlight);
// 	glDeleteBuffers(1, &bufFilter);
// 	glDeleteTextures(1, &tboFilter);
// 	glDeleteBuffers(1, &bufColormap0);
// 	glDeleteTextures(1, &tboColormap0);
// 	glDeleteBuffers(1, &bufColormap1);
// 	glDeleteBuffers(1, &bufPoints);
// 	glDeleteBuffers(1, &bufFacets);
// 	glDeleteBuffers(1, &bufOffsets);
// 	glDeleteBuffers(1, &bufFacetIndexes);
// 	glDeleteTextures(1, &tboColormap1);
// 	glDeleteBuffers(1, &bufColormap2);
// 	glDeleteTextures(1, &tboColormap2);
// 	glDeleteTextures(1, &tboPoints);
// 	glDeleteTextures(1, &tboFacets);
// 	glDeleteTextures(1, &tboOffsets);
// 	glDeleteTextures(1, &tboFacetIndexes);
// 	glBindBuffer(GL_TEXTURE_BUFFER, 0);

// 	// Clean shader
// 	shader.clean();
// }