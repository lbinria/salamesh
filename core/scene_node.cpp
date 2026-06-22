#include "scene_node.h"

#include "shaders/layer_params.h"

std::optional<Colormap> SceneNode::getColormap() {
		
	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		return layerParams->getColormap(ColormapLayer::COLORMAP_LAYER_0);
	}

	return std::nullopt;
}


void SceneNode::setColormap(Colormap colormap) {

	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormap);
	}
}


std::string SceneNode::getLayerAttr(Layer layer, ElementKind kind) {
	std::tuple<Layer, ElementKind> k = {layer, kind};
	if (_attrNameByLayerAndKind.contains(k))
		return _attrNameByLayerAndKind[k];
	
	return defaultAttrName(layer);
}

// Choose which attribute to bind to layer / kind
void SceneNode::setLayerAttr(Layer layer, ElementKind kind, const std::string name) {
	_attrNameByLayerAndKind[{layer, kind}] = name;
}

void SceneNode::setLayer(Layer layer, ElementKind kind, const std::string attributeName, bool update) {
	setLayerAttr(layer, kind, attributeName);
	setLayer(layer, kind, update);
}

void SceneNode::setLayer(Layer layer, ElementKind kind, bool update) {

	auto &geo = getGeometry();

	_selectedAttribute = _attrNameByLayerAndKind[{layer, kind}];

	auto attrOpt = geo.getAttribute(_selectedAttribute);

	if (!attrOpt.has_value())
		return;

	auto attr = attrOpt.value();
	auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
	auto [min, max] = sl::getRange(data);

	for (auto &[_, material] : getMaterials()) {
		
		auto layerParams = material.getParams<LayersParams>("layers");

		if (!layerParams)
			continue;

		if (layerParams->isActivatedLayer(layer, kind) && !update)
			continue;

		layerParams->activateLayer(layer, kind);

		layerParams->range[layer] = glm::vec2(min, max);
		layerParams->nDims[layer] = attr.getNDims();
		layerParams->setLayer(data, layer);

	}
}

void SceneNode::updateLayers() {
	auto &geo = getGeometry();

	for (int k = 0; k < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++k) {
		for (int l = 0; l < static_cast<int>(Layer::LAYER_COUNT); ++l) {

			auto layer = static_cast<Layer>(l);
			auto kind = static_cast<ElementKind>(k);

			auto attrOpt = geo.getAttribute(_attrNameByLayerAndKind[{layer, kind}]);

			if (!attrOpt.has_value())
				continue;

			auto attr = attrOpt.value();
			auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
			auto [min, max] = sl::getRange(data);

			for (auto &[_, material] : getMaterials()) {
				auto layerParams = material.getParams<LayersParams>("layers");

				if (!layerParams)
					continue;

				auto activatedLayers = layerParams->getActivatedLayers();
				if (!activatedLayers[l][k])
					continue;

				layerParams->range[l] = glm::vec2(min, max);
				layerParams->nDims[l] = attr.getNDims();
				layerParams->setLayer(data, layer);
			}

		}
	}

}

void SceneNode::unsetLayers(bool reset) {
	for (int l = 0; l < 5; ++l) {
		for (int k = 0; k < 7; ++k) {
			unsetLayer(static_cast<Layer>(l), static_cast<ElementKind>(k), reset);
		}
	}
}


void SceneNode::unsetLayer(Layer layer, ElementKind kind, bool reset) {
	
	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");

		if (!layerParams)
			continue;

		// Little optimisation, doesn't update data
		// if layer isn't activated, no need to unset
		if (!layerParams->isActivatedLayer(layer, kind))
			continue;
		
		// Set requested layer data to zeros
		// if (reset)
		// 	resetLayer(kind, layer);

		layerParams->setLayerElement(-1, layer);
		layerParams->deactivateLayer(layer, kind);
	}

}