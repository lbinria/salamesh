#include "model_proxy.h"
#include "scene.h"
#include "scene_node.h"

#include "renderers/layer_params.h"

// int ModelProxy::nverts() const {
// 	auto node = _scene.getNodeByName(_nodeName);
	
// 	if (!node) 
// 		return 0;

// 	auto &geometry = node->getGeometry();
// 	geometry.
// }
// int ModelProxy::nfacets() const {

// }
// int ModelProxy::ncells() const {

// }
// int ModelProxy::ncorners() const {

// }
// int ModelProxy::nhalfedges() const {

// }

int ModelProxy::getSelectedColormap() {
	
	auto node = _node.lock();

	// Node doesn't exists
	if (!node)
		return 0;
	
	auto &colormaps = _scene.getColormaps();

	for (auto &[_, shaderBuffer] : node->getShaderBuffers()) {
		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		auto &colormaps = _scene.getColormaps();
		auto name = layerParams->getColormap(ColormapLayer::COLORMAP_LAYER_0).name;
		for (int i = 0; i < colormaps.size(); ++i) {
			if (colormaps[i].name == name) {
				return i;
			}
		}
	}

	return 0;
}

void ModelProxy::setSelectedColormap(int idx) {

	auto node = _node.lock();

	// Node doesn't exists
	if (!node)
		return;

	auto &colormaps = _scene.getColormaps();

	for (auto &[_, shaderBuffer] : node->getShaderBuffers()) {
		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormaps[idx]);
	}
}