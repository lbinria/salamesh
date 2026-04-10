#include "point_set_renderer.h"
#include "../../core/graphic_api.h"

const void * PointSetRenderer::getData() {
	vertices.clear();
	vertices.resize(ps.size());
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
	mat.addParam<LightParams>("light");
	mat.addParam<ClippingParams>("clipping");
	mat.addParam<PointSetParams>("points");
	mat.addParam<LayersParams>("layers");
	mat.addBufferGroup("layers", std::make_shared<LayerBufferGroup>());
	return mat;
}

void PointSetRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	clearPoints();
}

// TODO clean shader