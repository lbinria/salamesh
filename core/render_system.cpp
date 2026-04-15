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

void RenderSystem::updateAttr(Model &model, ModelState &modelState, MaterialInstance &mat) {

	for (auto layer : modelState.getUpdateAttrRequest()) {

		auto layerParams = mat.getParams("layers");

		if (layerParams.has_value()) {

			auto oldSelectedAttrIdx = modelState.getOldSelectedAttr(layer);
			if (oldSelectedAttrIdx >= 0) {
				auto oldAttr = model.getAttr(oldSelectedAttrIdx);

				// Deactivate last activated layer if there is one
				auto &p = layerParams.value().get();
				auto &lp = static_cast<LayersParams&>(p);
				lp.setLayerActivation(static_cast<Layer>(layer), oldAttr.kind, false);
			}
		}

		auto selectedAttrIdx = modelState.getSelectedAttr(layer);

		if (selectedAttrIdx < 0)
			continue;

		auto attr = model.getAttr(selectedAttrIdx);

		// Bind attribute name to layer / primitive
		modelState.setLayerAttrName(attr.name, static_cast<Layer>(layer), attr.kind);
		// Request set layer
		modelState.setLayer(static_cast<Layer>(layer), attr.kind, true);
	}
}

void RenderSystem::updateLayer(Model &model, ModelState &modelState, MaterialInstance &mat) {

	for (auto request : modelState.getUpdateLayerRequests()) {
		auto [layer, kind] = request;

		// TODO maybe check mat visibility ? but if i do that i should postpone update

		auto layerParams = mat.getParams("layers");
		auto layerBuffers = mat.getBuffers("layers");

		if (!layerParams.has_value() || !layerBuffers.has_value())
			return;
		
		auto attrName = modelState.getLayerAttrName(layer, kind);
		int nDims = model.getAttributeNDims(attrName, kind);

		// Extract selectedDim from string
		int selectedDim = -1;
		auto lbrPos = attrName.find('[');
		auto rbrPos = attrName.find(']');
		// if "attr[0]" requested for example, we would like to see selectedDim=0, so nDims of attribute = 1
		if (lbrPos != std::string::npos && rbrPos != std::string::npos) {
			selectedDim = std::stoi(attrName.substr(lbrPos + 1, rbrPos - lbrPos));
			attrName = attrName.substr(0, lbrPos);
			nDims = 1;
		}

		auto dataOpt = model.getAttrData(attrName, kind, selectedDim);

		// Silent when no data ?
		// If no data => it means that attr name wasn't found
		if (!dataOpt.has_value())
			continue;

		auto data = dataOpt.value();
		auto [min, max] = sl::getRange(data);

		auto &lb = layerBuffers.value().get();
		auto &lp = static_cast<LayersParams&>(layerParams.value().get());

		lp.setLayerRange(layer, min, max);
		lp.setLayerNDims(layer, nDims);
		lp.setLayerActivation(layer, kind, true);
		lb.set(layerToString(layer), data);
		
	}
}

void RenderSystem::updateGeometry(Renderer &renderer) {
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

void RenderSystem::render(Model &model, ISceneView &view) {

	auto &modelState = view.getState(model);

	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, model.getPosition());

	// Update model data, if request made
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
		if (!view.hasMaterial(*renderer))
			view.addMaterial(*renderer, renderer->getDefaultMaterial());

		auto &mat = view.getMaterial(*renderer);

		updateGeometry(*renderer);

		updateAttr(model, modelState, mat);
		updateLayer(model, modelState, mat);


		render(*renderer, modelState, transform, model.getIndex(), mat);
	}

	modelState.cleanUpdateAttrRequests();
	modelState.cleanUpdateLayerRequests();
}

void RenderSystem::render(Renderer &renderer, ModelState &modelState, glm::mat4 &transform, int meshIndex, MaterialInstance &mat) {
	if (!mat.getVisible())
		return;

	RenderSystem::VertexBuffer &vbuf = getVertexBuffer(renderer);

	glBindVertexArray(vbuf.vao);

	auto &shader = renderer.getShader();
	shader.use();
	shader.setMat4("model", transform);
	shader.setInt("meshIndex", meshIndex); // TODO not doing this everytime as meshIndex doesnt change

	// TODO maybe can move this top
	for (int l = 0; l < 3; ++l) {
		shader.setInt("colormap" + std::to_string(l), l);
		glActiveTexture(GL_TEXTURE0 + l);
		int i = modelState.getSelectedColormap(static_cast<ColormapLayer>(l));
		glBindTexture(GL_TEXTURE_2D, texColormaps[i]);
	}

	mat.apply(shader);

	auto rp = renderer.getRenderPrimitive();
	glDrawArrays(getGLPrimFromRenderPrimitive(rp), 0, renderer.getElementsCount());
}


