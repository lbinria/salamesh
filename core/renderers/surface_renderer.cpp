#include "surface_renderer.h"
#include "../../core/graphic_api.h"

void SurfaceRenderer::clear() {
	nelements = 0;
	dirty = true;
}

MaterialInstance SurfaceRenderer::getDefaultMaterial() {
	MaterialInstance mat;
	mat.addParam<LightParams>("light");
	mat.addParam<ClippingParams>("clipping");
	mat.addParam<MeshStyleParams>("style");
	mat.addParam<LayersParams>("layers");
	mat.addBuffers<LayerBufferGroup>("layers");
	return mat;
}

std::vector<Renderer::RendererElementField> SurfaceRenderer::getElementFields() {
	return {
		{
			.name = "p0",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p0))
		},
		{
			.name = "p1",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p1))
		},
		{
			.name = "p2",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, p2))
		},
		{
			.name = "facetIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, facetIndex))
		},
		{
			.name = "localIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, localIndex))
		},
		{
			.name = "cornerIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, cornerIndex))
		}
	};
}