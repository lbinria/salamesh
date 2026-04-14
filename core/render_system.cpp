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

void RenderSystem::processRequests(ISceneView &sceneView, Model &model) {
	auto &modelState = sceneView.getState(model);

	// auto updateAttrRequestOpt = modelState.getUpdateAttrRequest();
	// auto updateLayerRequestOpt = modelState.getUpdateLayerRequest();

	// if (updateAttrRequestOpt.has_value()) {
	while (auto layerOpt = modelState.popUpdateAttrRequest()) {
		auto layer = layerOpt.value();
		auto selectedAttrIdx = modelState.getSelectedAttr(layer);

		// TODO Unset before check below

		if (selectedAttrIdx < 0) {
			continue;
		}

		for (auto &[rendererName, renderer] : model.getRenderers()) {

			// For test just render pointset
			auto psr = std::dynamic_pointer_cast<PointSetRenderer>(renderer);
			auto msr = std::dynamic_pointer_cast<MeshRenderer>(renderer);
			auto hsr = std::dynamic_pointer_cast<HalfedgeRenderer>(renderer);
			auto bbr = std::dynamic_pointer_cast<BBoxRenderer>(renderer);
			if (!psr && !msr)
			// if (!psr && !msr && !hsr && !bbr)
				continue;

			auto &mat = sceneView.getMaterial(*renderer);

			
			auto paramsOpt = mat.getParams("layers");
			if (paramsOpt.has_value()) {
				auto attr = model.getAttr(modelState.getSelectedAttr(layer));

				// Deactivate layer for all primitives
				auto &p = paramsOpt.value().get();
				auto &lp = static_cast<LayersParams&>(p);
				// lp.setLayerActivation(static_cast<Layer>(layer), attr.kind, false);

				// Set layer buffers
				auto layerBuffers = mat.getBuffers("layers");
				if (layerBuffers.has_value()) {
					auto &lb = layerBuffers.value().get();

					auto attr = model.getAttr(modelState.getSelectedAttr(layer));
					int nDims = model.getAttributeNDims(attr.name, attr.kind);

					// Attach attribute name to layer
					modelState.setLayerAttrName(attr.name, static_cast<Layer>(layer), attr.kind);

					// Extract selectedDim from string
					int selectedDim = -1;
					auto lbrPos = attr.name.find('[');
					auto rbrPos = attr.name.find(']');
					// if "attr[0]" requested for example, we would like to see selectedDim=0, so nDims of attribute = 1
					if (lbrPos != std::string::npos && rbrPos != std::string::npos) {
						selectedDim = std::stoi(attr.name.substr(lbrPos + 1, rbrPos - lbrPos));
						attr.name = attr.name.substr(0, lbrPos);
						nDims = 1;
					}

					auto dataOpt = model.getAttrData(attr.name, attr.kind, selectedDim);

					// Silent when no data ?
					// If no data => it means that attr name wasn't found
					if (!dataOpt.has_value())
						continue;

					auto data = dataOpt.value();
					auto [min, max] = sl::getRange(data);

					lp.setLayerRange(static_cast<Layer>(layer), min, max);
					lp.setLayerNDims(static_cast<Layer>(layer), nDims);
					lp.setLayerActivation(static_cast<Layer>(layer), attr.kind, true);
					lb.set(layerToString(static_cast<Layer>(layer)), data);
				}
				
			}
		}
	}
}

void RenderSystem::createMaterial(ISceneView &sceneView, Renderer &renderer) {
	if (!sceneView.hasMaterial(renderer))
		sceneView.addMaterial(renderer, renderer.getDefaultMaterial());
}

void RenderSystem::pushVBO(Renderer &renderer) {
	if (!renderer.isDirty())
		return;

	size_t size = renderer.getElementSize();
	const void *data = renderer.getData();
	// auto usage = renderer->isDrawDynamic() ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;

	auto &vbuf = getVertexBuffer(renderer);
	glBindVertexArray(vbuf.vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf.vbo);
	glBufferData(GL_ARRAY_BUFFER, renderer.getElementsCount() * size, data, GL_STATIC_DRAW);
	renderer.setDirty(false);
}

void RenderSystem::render(Model &model, ISceneView &sceneView) {

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, model.getPosition());

	// Update model data, if request made
	processRequests(sceneView, model);


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

		// Create default material for renderer if not found
		createMaterial(sceneView, *renderer);
		// If data changed, push !
		pushVBO(*renderer);

		auto &mat = sceneView.getMaterial(*renderer);
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
		shader.setInt("colormap" + std::to_string(i), i);
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


