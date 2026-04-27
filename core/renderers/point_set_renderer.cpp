#include "point_set_renderer.h"
#include "../../core/graphic_api.h"

Renderer::GeometricData PointSetRenderer::getData() {
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
	return Renderer::GeometricData{ .vboBuffer = vertices.data(), .texBuffers = {} };
}

MaterialInstance PointSetRenderer::getDefaultMaterial() {
	MaterialInstance mat;
	mat.addParam<LightParams>("light");
	mat.addParam<ClippingParams>("clipping");
	mat.addParam<PointSetParams>("style");
	mat.addParam<LayersParams>("layers");
	mat.addBuffers<LayerBufferGroup>("layers");
	return mat;
}

void PointSetRenderer::clear() {
	vertices.clear();
	nelements = 0;
	clearPoints();
	dirty = true;
}

// TODO clean shader