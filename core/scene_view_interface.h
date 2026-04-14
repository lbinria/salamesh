#pragma once
#include "render_surface.h"
#include "models/model_view.h"
#include "material_instance.h"
#include <set>

struct ModelState {

	std::string getLayerAttrName(Layer layer, ElementKind kind) {
		std::tuple<Layer, ElementKind> k = {layer, kind};
		if (attrNameByLayerAndKind.contains(k))
			return attrNameByLayerAndKind[k];
		
		return defaultAttrName(layer);
	}

	// Choose which attribute to bind to layer / kind
	void setLayerAttrName(std::string name, Layer layer, ElementKind kind) {
		attrNameByLayerAndKind[{layer, kind}] = name;
	}

	int getSelectedAttr(ColormapLayer colormapLayer) {
		return selectedAttr[static_cast<int>(colormapLayer)];
	}

	void setSelectedAttr(int idx, ColormapLayer layer) {
		selectedAttr[layer] = idx;
		// updateAttrRequest = layer;
		updateAttrRequests.insert(layer);
	}

	void setLayer(ElementKind kind, Layer layer, bool update) {
		if (activatedLayers[{layer, kind}] && !update)
			return;

		activatedLayers[{layer, kind}] = true;
		updateLayerRequest = {layer, kind};
	}

	void unsetLayer(ElementKind kind, Layer layer, bool reset) {
		// Little optimisation, doesn't update data
		// if layer isn't activated, no need to unset
		if (!activatedLayers[{layer, kind}])
			return;
		

		// for (auto const &[k ,r] : rendererViews) {
		// 	if (r->isRenderElement(kind)) {
		// 		r->setLayerElement(-1, layer); // element -1 means => deactivate layer
		// 	}
		// }

		activatedLayers[{layer, kind}] = false;
		updateLayerRequest = {layer, kind};
	}

	// std::optional<ColormapLayer> getUpdateAttrRequest() { return updateAttrRequest; }
	std::optional<std::pair<Layer, ElementKind>> getUpdateLayerRequest() { return updateLayerRequest; }

	// void updateAttrRequestDone() {
	// 	updateAttrRequest = std::nullopt;
	// }

	std::optional<ColormapLayer> popUpdateAttrRequest() {
		if (updateAttrRequests.size() <= 0)
			return std::nullopt;
		
		auto l = *updateAttrRequests.begin();
		updateAttrRequests.erase(l);
		return l;
	}

	private:

	std::set<ColormapLayer> updateAttrRequests;

	std::optional<std::pair<Layer, ElementKind>> updateLayerRequest = std::nullopt;

	int selectedAttr[3] = {-1, -1, -1};
	int selectedColormap[3] = {0, 0, 0};
	std::map<std::tuple<Layer, ElementKind>, bool> activatedLayers;
	std::map<std::tuple<Layer, ElementKind>, std::string> attrNameByLayerAndKind;
};

struct ISceneView {

	virtual std::string getName() const = 0;

	virtual void setup() = 0;
	virtual void render(Shader &screenShader, unsigned int quadVAO) = 0;
	virtual void clear() = 0;
	virtual void clean() = 0;

	virtual void resize(int w, int h) = 0;

	virtual void setCamera(std::shared_ptr<Camera> camera) = 0;

	virtual RenderSurface &getRenderSurface() = 0;


	virtual MaterialInstance& getMaterial(Renderer &renderer) = 0;
	virtual MaterialInstanceCollection getMaterials(const Model &model) = 0;
	virtual bool hasMaterial(const Renderer &renderer) const = 0;
	virtual void addMaterial(const Renderer &renderer, MaterialInstance mat) = 0;
	virtual bool removeMaterial(const Renderer &renderer) = 0;

	virtual ModelState& getState(Model &model) = 0;

};