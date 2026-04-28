#pragma once
#include "layer.h"

#include <set>
#include <map>

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

	// Use when geometry changed to refresh activated layers according to new geometry
	void refreshLayers() {
		for (auto &[lk, activated] : activatedLayers) {
			if (!activated)
				continue;

			auto layer = std::get<0>(lk);
			auto kind = std::get<1>(lk);

			// Request the layer update to render system !
			updateLayerRequests.insert({layer, kind});
		}
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