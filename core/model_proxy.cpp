#include "model_proxy.h"
#include "scene.h"
#include "scene_node.h"

#include "renderers/layer_params.h"


// int ModelProxy::getSelectedColormap() {
	
// 	auto node = _node.lock();

// 	// Node doesn't exists
// 	if (!node)
// 		return 0;
	
// 	auto &colormaps = _scene.getColormaps();

// 	for (auto &[_, shaderBuffer] : node->getShaderBuffers()) {
// 		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
// 		if (!layerParams)
// 			continue;
		
// 		auto &colormaps = _scene.getColormaps();
// 		auto name = layerParams->getColormap(ColormapLayer::COLORMAP_LAYER_0).name;
// 		for (int i = 0; i < colormaps.size(); ++i) {
// 			if (colormaps[i].name == name) {
// 				return i;
// 			}
// 		}
// 	}

// 	return 0;
// }

// void ModelProxy::setSelectedColormap(int idx) {

// 	auto node = _node.lock();

// 	// Node doesn't exists
// 	if (!node)
// 		return;

// 	auto &colormaps = _scene.getColormaps();

// 	for (auto &[_, shaderBuffer] : node->getShaderBuffers()) {
// 		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
// 		if (!layerParams)
// 			continue;
		
// 		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormaps[idx]);
// 	}
// }

// std::string ModelProxy::getLayerAttr(Layer layer, ElementKind kind) {
// 	std::tuple<Layer, ElementKind> k = {layer, kind};
// 	if (_attrNameByLayerAndKind.contains(k))
// 		return _attrNameByLayerAndKind[k];
	
// 	return defaultAttrName(layer);
// }

// // Choose which attribute to bind to layer / kind
// void ModelProxy::setLayerAttr(std::string name, Layer layer, ElementKind kind) {
// 	_attrNameByLayerAndKind[{layer, kind}] = name;
// }

// void ModelProxy::setLayer(Layer layer, ElementKind kind, bool update) {

// 	auto node = _node.lock();

// 	// Node doesn't exists
// 	if (!node)
// 		return;

// 	auto &geo = node->getGeometry();
// 	auto attrOpt = geo.getAttribute(_attrNameByLayerAndKind[{layer, kind}]);

// 	if (!attrOpt.has_value())
// 		return;

// 	auto attr = attrOpt.value();
// 	auto data = sl::getContainerData(attr.ptr.get(), attr.dim);
// 	auto [min, max] = sl::getRange(data);

// 	for (auto &[_, shaderBuffer] : node->getShaderBuffers()) {
		
// 		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");

// 		if (!layerParams)
// 			continue;

// 		if (layerParams->isActivatedLayer(layer, kind) && !update)
// 			continue;

// 		layerParams->activateLayer(layer, kind);

// 		layerParams->range[layer] = glm::vec2(min, max);
// 		layerParams->nDims[layer] = attr.getNDims();
// 		layerParams->setLayer(data, layer);

// 	}
// }

// void ModelProxy::unsetLayer(ElementKind kind, Layer layer, bool reset) {

// 	auto node = _node.lock();

// 	// Node doesn't exists
// 	if (!node)
// 		return;
	
// 	for (auto &[_, shaderBuffer] : node->getShaderBuffers()) {
// 		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");

// 		if (!layerParams)
// 			continue;

// 		// Little optimisation, doesn't update data
// 		// if layer isn't activated, no need to unset
// 		if (!layerParams->isActivatedLayer(layer, kind))
// 			continue;
		
// 		// Set requested layer data to zeros
// 		// if (reset)
// 		// 	resetLayer(kind, layer);

// 		layerParams->setLayerElement(-1, layer);
// 		layerParams->deactivateLayer(layer, kind);
// 	}

// }