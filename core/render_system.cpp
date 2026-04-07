#include "render_system.h"

int getGLPrimFromRenderPrimitive(Renderer::RenderPrimitive primitive) {
	switch (primitive)
	{
	case Renderer::RenderPrimitive::RENDER_POINTS:
		return GL_POINTS;
	case Renderer::RenderPrimitive::RENDER_LINES:
		return GL_LINES;
	case Renderer::RenderPrimitive::RENDER_TRIANGLES:
		return GL_TRIANGLES;
	default:
		return -1;
	}
}

RenderSystem::GeometricBuffer& RenderSystem::getGeometricBuffer(std::string rendererName) {
	if (geometries.count(rendererName) > 0) {
		RenderSystem::GeometricBuffer gbuf;

		glGenVertexArrays(1, &gbuf.vao);
		glGenBuffers(1, &gbuf.vbo);

		glBindVertexArray(gbuf.vao);
		glBindBuffer(GL_ARRAY_BUFFER, gbuf.vbo);

		// UNIFORM TYPE, UNIFORM NAME, STRUCT SIZE, OFFSET
		// renderer->getVBOs()

		// Create VBO
		
		geometries[rendererName] = std::move(gbuf);
		// // VBO
		// sl::createVBOInteger(shader.id, "vertexIndex", sizeof(Vertex), (void*)offsetof(Vertex, vertexIndex));
		// sl::createVBOVec3(shader.id, "aPos", sizeof(Vertex), (void*)offsetof(Vertex, position));
		// sl::createVBOFloat(shader.id, "sizeScale", sizeof(Vertex), (void*)offsetof(Vertex, size));
	}

	return geometries.at(rendererName);
}

void RenderSystem::render(IScene &scene) {

	// Check deleted for cleanup

	for (auto &[viewName, view] : scene.getViews()) {
		for (auto &[modelName, model] : scene.getModels()) {
			render(*model, *view);
		}
	}

}

void RenderSystem::render(Model &model, ISceneView &sceneView) {

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, model.getPosition());

	for (auto &[rendererName, renderer] : model.getRenderers()) {

		if (renderer->isDirty()) {
			// push data

		}
		// auto &mat = sceneView.getMaterialInstance(renderer);
		// render(*renderer, transform, mat);
	}

}

void RenderSystem::render(Renderer &renderer, glm::mat4 &transform, MaterialInstance &mat) {
	if (!mat.getVisible())
		return;

	auto &shader = renderer.getShader();
	shader.use();
	shader.setMat4("model", transform);

	RenderSystem::GeometricBuffer &gbuf = getGeometricBuffer(renderer.getName());

	glBindVertexArray(gbuf.vao);

	mat.apply(shader);

	// TODO maybe can move this top
	for (int i = 0; i < 3; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texColormap[i]);
	}

	// glActiveTexture(GL_TEXTURE0 + 3);
	// glBindTexture(GL_TEXTURE_BUFFER, tboHighlight);

	// glActiveTexture(GL_TEXTURE0 + 4);
	// glBindTexture(GL_TEXTURE_BUFFER, tboFilter);

	// glActiveTexture(GL_TEXTURE0 + 5);
	// glBindTexture(GL_TEXTURE_BUFFER, tboColormap0);

	// glActiveTexture(GL_TEXTURE0 + 6);
	// glBindTexture(GL_TEXTURE_BUFFER, tboColormap1);

	// glActiveTexture(GL_TEXTURE0 + 7);
	// glBindTexture(GL_TEXTURE_BUFFER, tboColormap2);

	auto rp = renderer.getRenderPrimitive();
	glDrawArrays(getGLPrimFromRenderPrimitive(rp), 0, renderer.getElementsCount());
}

