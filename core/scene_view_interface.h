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

	int getOldSelectedAttr(ColormapLayer colormapLayer) {
		return oldSelectedAttr[static_cast<int>(colormapLayer)];
	}

	void setSelectedAttr(int idx, ColormapLayer layer) {
		oldSelectedAttr[layer] = selectedAttr[layer];
		selectedAttr[layer] = idx;
		updateAttrRequests.insert(layer);
	}

	void setLayer(Layer layer, ElementKind kind, bool update) {
		if (activatedLayers[{layer, kind}] && !update)
			return;

		activatedLayers[{layer, kind}] = true;
		updateLayerRequests.insert({layer, kind});
	}

	void unsetLayer(Layer layer, ElementKind kind, bool reset) {
		// Little optimisation, doesn't update data
		// if layer isn't activated, no need to unset
		if (!activatedLayers[{layer, kind}])
			return;
		
		activatedLayers[{layer, kind}] = false;
		updateLayerRequests.insert({layer, kind});
	}

	void setSelectedColormap(int idx, ColormapLayer layer) {
		selectedColormap[static_cast<int>(layer)] = idx;
	}

	int getSelectedColormap(ColormapLayer layer) const {
		return selectedColormap[static_cast<int>(layer)];
	}


	std::set<ColormapLayer> getUpdateAttrRequest() {
		return updateAttrRequests;
	}

	std::set<std::pair<Layer, ElementKind>> getUpdateLayerRequests() {
		return updateLayerRequests;
	}

	void cleanUpdateAttrRequests() {
		updateAttrRequests.clear();
	}

	void cleanUpdateLayerRequests() {
		updateLayerRequests.clear();
	}

	private:

	std::set<ColormapLayer> updateAttrRequests;
	std::set<std::pair<Layer, ElementKind>> updateLayerRequests;

	int oldSelectedAttr[3] = {-1, -1, -1};
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
	virtual MaterialInstance& getMaterialOrDefault(Renderer &renderer) = 0;
	virtual MaterialInstanceCollection getMaterials(const Model &model) = 0;
	virtual bool hasMaterial(const Renderer &renderer) const = 0;
	virtual void addMaterial(const Renderer &renderer, MaterialInstance mat) = 0;
	virtual bool removeMaterial(const Renderer &renderer) = 0;

	virtual ModelState& getState(Model &model) = 0;

};