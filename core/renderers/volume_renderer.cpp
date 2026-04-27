#include "volume_renderer.h"

void VolumeRenderer::clear() {
	// vertices.clear();
	nelements = 0;
	dirty = true;

}

std::vector<Renderer::RendererElementField> VolumeRenderer::getElementFields() {
	return {
		{
			.name = "aPos",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, position))
		},
		{
			.name = "normal",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, normal))
		},
		{
			.name = "aHeights",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_VEC3,
			.offset = static_cast<int>(offsetof(Vertex, heights))
		},
		{
			.name = "facetIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, facetIndex))
		},
		{
			.name = "cellIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, cellIndex))
		},
		{
			.name = "vertexIndex",
			.type = RendererElementType::RENDERER_ELEMENT_TYPE_INT,
			.offset = static_cast<int>(offsetof(Vertex, vertexIndex))
		}
	};
}