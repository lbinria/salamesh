#include "surface_renderer.h"
#include "../../core/utils/opengl_helper.h"
#include "mesh_style_params.h"
#include "layer_params.h"

void SurfaceMaterial::init() {

	_params["style"] = std::make_shared<MeshStyleParams>();
	_params["layers"] = std::make_shared<LayersParams>();
	
	for (auto &[k, p] : _params) {
		p->init();
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	sl::createVBOVec3(shader.id, "p0", sizeof(Vertex), (void*)offsetof(Vertex, p0));
	sl::createVBOVec3(shader.id, "p1", sizeof(Vertex), (void*)offsetof(Vertex, p1));
	sl::createVBOVec3(shader.id, "p2", sizeof(Vertex), (void*)offsetof(Vertex, p2));
	sl::createVBOInteger(shader.id, "facetIndex", sizeof(Vertex), (void*)offsetof(Vertex, facetIndex));
	sl::createVBOInteger(shader.id, "localIndex", sizeof(Vertex), (void*)offsetof(Vertex, localIndex));
	sl::createVBOInteger(shader.id, "cornerIndex", sizeof(Vertex), (void*)offsetof(Vertex, cornerIndex));
	
}

void SurfaceMaterial::render(glm::vec3 &position) {

	if (!visible)
		return;

	glBindVertexArray(VAO);




	setPosition(position);
	for (auto &[paramsName, params] : _params)
		params->apply(shader);

	glDrawArrays(GL_TRIANGLES, 0, nelements);
}

void SurfaceMaterial::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
}

void SurfaceMaterial::clean() {
	// Clean up
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	// TODO important clean params

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
	glBindBuffer(GL_TEXTURE_BUFFER, 0);

	// Clean shader
	shader.clean();
}