#include "point_set_renderer.h"
#include "../../core/graphic_api.h"

void PointSetRenderer::init() {

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

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

const void * PointSetRenderer::getData() {
	std::vector<Vertex> vertices(ps.size());
	for (int i = 0; i < ps.size(); ++i) {
		auto &v = ps[i];

		vertices[i] = { 
			.vertexIndex = i,
			.position = glm::vec3(v.x, v.y, v.z),
			.size = 1.f
		};
	}

	nelements = vertices.size();
	return vertices.data();
}

MaterialInstance PointSetRenderer::getDefaultMaterial() {
	MaterialInstance mat;
	mat.addParam("light", std::make_shared<LightParams>());
	mat.addParam("clipping", std::make_shared<ClippingParams>());
	mat.addParam("points", std::make_shared<PointSetParams>());
	mat.addParam("layers", std::make_shared<LayersParams>());
	mat.addBufferGroup("layers", std::make_shared<LayerBufferGroup>());
	return mat;
}



void PointSetRenderer::render(RendererView &rv, glm::vec3 &position) {
	if (!rv.visible)
		return;

	glBindVertexArray(VAO);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);

	rv.use(shader);
	shader.setMat4("model", model);

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

	// Clean shader
	shader.clean();
}