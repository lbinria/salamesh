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

RenderSystem::GeometricBuffer& RenderSystem::getGeometricBuffer(Renderer &renderer) {
	if (geometries.count(renderer.getName()) == 0) {
		RenderSystem::GeometricBuffer gbuf;

		glGenVertexArrays(1, &gbuf.vao);
		glGenBuffers(1, &gbuf.vbo);

		glBindVertexArray(gbuf.vao);
		glBindBuffer(GL_ARRAY_BUFFER, gbuf.vbo);
		
		geometries[renderer.getName()] = std::move(gbuf);

		// Setup VBO
		for (auto &f : renderer.getElementFields()) {

			auto sid = renderer.getShader().id;
			GLsizei stride = renderer.getElementSize();
			auto ptr = (void*)f.offset;

			switch (f.type) {
				case Renderer::RendererElementType::RENDERER_ELEMENT_TYPE_INT: {
					sl::createVBOInteger(sid, f.name.c_str(), stride, ptr);
					break;
				}
				case Renderer::RendererElementType::RENDERER_ELEMENT_TYPE_FLOAT: {
					sl::createVBOFloat(sid, f.name.c_str(), stride, ptr);
					break;
				}
				case Renderer::RendererElementType::RENDERER_ELEMENT_TYPE_VEC3: {
					sl::createVBOVec3(sid, f.name.c_str(), stride, ptr);
					break;
				}
			}
		}
	}

	return geometries.at(renderer.getName());
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

		// For test just render pointset
		auto psr = std::dynamic_pointer_cast<PointSetRenderer>(renderer);
		if (!psr)
			continue;

		// If data changed, push !
		if (renderer->isDirty()) {

			size_t size = renderer->getElementSize();
			const void *data = renderer->getData();
			// auto usage = renderer->isDrawDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

			auto &gbuf = getGeometricBuffer(*renderer);
			glBindVertexArray(gbuf.vao);
			glBindBuffer(GL_ARRAY_BUFFER, gbuf.vbo);
			glBufferData(GL_ARRAY_BUFFER, renderer->getElementsCount() * size, data, GL_STATIC_DRAW);
			renderer->setDirty(false);
		}
		
		if (!sceneView.hasMaterial(renderer->getName())) {
			auto mat = renderer->getDefaultMaterial();
			auto rn = renderer->getName();
			sceneView.setMaterial(mat, rn);
		}

		auto &mat = sceneView.getMaterial(renderer->getName());
		render(*renderer, transform, mat);
	}

}

void RenderSystem::render(Renderer &renderer, glm::mat4 &transform, MaterialInstance &mat) {
	if (!mat.getVisible())
		return;



	RenderSystem::GeometricBuffer &gbuf = getGeometricBuffer(renderer);

	glBindVertexArray(gbuf.vao);

	auto &shader = renderer.getShader();
	shader.use();
	shader.setMat4("model", transform);

	// TODO maybe can move this top
	for (int i = 0; i < 3; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texColormaps[i]);
	}

	mat.apply(shader);

	if (mat.hasParam("layers")) {
		auto layersParams = mat.getParam<LayersParams>("layers");
		if (layersParams) {
			for (int l = 0; l < 3; ++l) {
				layersParams->setColormapTex(l, texColormaps[l]);
			}
		}
	}

	auto rp = renderer.getRenderPrimitive();
	glDrawArrays(getGLPrimFromRenderPrimitive(rp), 0, renderer.getElementsCount());
}

