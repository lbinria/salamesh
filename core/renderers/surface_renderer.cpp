#include "surface_renderer.h"
#include "../../core/graphic_api.h"

void SurfaceRenderer::init() {

}

void SurfaceRenderer::render(RendererView &rv, glm::vec3 &position) {

}

void SurfaceRenderer::clear() {
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, nelements * sizeof(Vertex), nullptr, GL_STATIC_DRAW);
}

void SurfaceRenderer::clean() {
	// // Clean up
	// glDeleteVertexArrays(1, &VAO);
	// glDeleteBuffers(1, &VBO);

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

	// // Clean shader
	// shader.clean();
}

MaterialInstance SurfaceRenderer::getDefaultMaterial() {
	MaterialInstance mat;
	mat.addParam("light", std::make_shared<LightParams>());
	mat.addParam("clipping", std::make_shared<ClippingParams>());
	mat.addParam("mesh", std::make_shared<MeshParams>());
	mat.addParam("layers", std::make_shared<LayersParams>());
	mat.addBufferGroup("layers", std::make_shared<LayerBufferGroup>());
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