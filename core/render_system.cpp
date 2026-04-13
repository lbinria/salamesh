#include "render_system.h"

#include "renderers/bbox_renderer/bbox_renderer.h"

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

RenderSystem::VertexBuffer& RenderSystem::getVertexBuffer(Renderer &renderer) {
	if (geometries.count(renderer.getName()) == 0) {
		RenderSystem::VertexBuffer gbuf;

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
		auto msr = std::dynamic_pointer_cast<MeshRenderer>(renderer);
		auto hsr = std::dynamic_pointer_cast<HalfedgeRenderer>(renderer);
		auto bbr = std::dynamic_pointer_cast<BBoxRenderer>(renderer);
		// if (!msr)
		if (!psr && !msr && !hsr && !bbr)
		// if (!psr && !msr )
			continue;

		// If data changed, push !
		if (renderer->isDirty()) {

			size_t size = renderer->getElementSize();
			const void *data = renderer->getData();
			// auto usage = renderer->isDrawDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

			auto &vbuf = getVertexBuffer(*renderer);
			glBindVertexArray(vbuf.vao);
			glBindBuffer(GL_ARRAY_BUFFER, vbuf.vbo);
			glBufferData(GL_ARRAY_BUFFER, renderer->getElementsCount() * size, data, GL_STATIC_DRAW);
			renderer->setDirty(false);
		}
		
		// Create default material if not found
		auto modelName = model.getName();

		if (!sceneView.hasMaterial(*renderer)) {
			sceneView.addMaterial(*renderer, renderer->getDefaultMaterial());
		}

		auto &mat = sceneView.getMaterial(*renderer);


		// // TODO refactor set index (beuuurk !!!)
		// auto styleParamsOpt = mat.getParams("style");
		// if (styleParamsOpt.has_value()) {
		// 	auto &styleParams = styleParamsOpt.value().get();
		// 	if (auto meshStyleParams = dynamic_cast<MeshStyleParams*>(&styleParams)) {
		// 		meshStyleParams->setIndex(model.getIndex());
		// 	}
		// }

		render(*renderer, transform, model.getIndex(), mat);
	}

}

void RenderSystem::render(Renderer &renderer, glm::mat4 &transform, int meshIndex, MaterialInstance &mat) {
	if (!mat.getVisible())
		return;



	RenderSystem::VertexBuffer &vbuf = getVertexBuffer(renderer);

	glBindVertexArray(vbuf.vao);

	auto &shader = renderer.getShader();
	shader.use();
	shader.setMat4("model", transform);
	shader.setInt("meshIndex", meshIndex);

	// TODO maybe can move this top
	for (int i = 0; i < 3; ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, texColormaps[i]);
	}

	mat.apply(shader);

	// // TODO beurrk ! replace by UBO
	// if (mat.hasParam("layers")) {
	// 	// auto layersParams = mat.getParams<LayersParams>("layers");
	// 	auto params = mat.getParams("layers");
	// 	if (params) {
	// 		for (int l = 0; l < 3; ++l) {
	// 			auto &layersParams = static_cast<LayersParams&>(params.value().get());
	// 			layersParams.setColormapTex(l, texColormaps[l]);
	// 		}
	// 	}
	// }

	auto rp = renderer.getRenderPrimitive();
	glDrawArrays(getGLPrimFromRenderPrimitive(rp), 0, renderer.getElementsCount());
}


