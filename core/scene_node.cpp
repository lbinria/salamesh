#include "scene_node.h"

#include "renderers/layer_params.h"

// std::optional<Colormap> SceneNode::getSelectedColormap() {
		
// 	for (auto &[_, shaderBuffer] : getShaderBuffers()) {
// 		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
// 		if (!layerParams)
// 			continue;
		
// 		return layerParams->getColormap(ColormapLayer::COLORMAP_LAYER_0);
// 	}

// 	return std::nullopt;
// }

// void SceneNode::setSelectedColormap(Colormap colormap) {

// 	for (auto &[_, shaderBuffer] : getShaderBuffers()) {
// 		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
// 		if (!layerParams)
// 			continue;
		
// 		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormaps);
// 	}
// }


// std::string SceneNode::getLayerAttr(Layer layer, ElementKind kind) {
// 	std::tuple<Layer, ElementKind> k = {layer, kind};
// 	if (_attrNameByLayerAndKind.contains(k))
// 		return _attrNameByLayerAndKind[k];
	
// 	return defaultAttrName(layer);
// }

// // Choose which attribute to bind to layer / kind
// void SceneNode::setLayerAttr(std::string name, Layer layer, ElementKind kind) {
// 	_attrNameByLayerAndKind[{layer, kind}] = name;
// }

void SceneNode::setLayer(Layer layer, ElementKind kind, bool update) {

	auto &geo = getGeometry();
	auto attrOpt = geo.getAttribute(_attrNameByLayerAndKind[{layer, kind}]);

	if (!attrOpt.has_value())
		return;

	auto attr = attrOpt.value();
	auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
	auto [min, max] = sl::getRange(data);

	for (auto &[_, shaderBuffer] : getShaderBuffers()) {
		
		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");

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

void SceneNode::unsetLayer(ElementKind kind, Layer layer, bool reset) {
	
	for (auto &[_, shaderBuffer] : getShaderBuffers()) {
		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");

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