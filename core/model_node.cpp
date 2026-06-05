#include "model_node.h"
#include "scene.h"
#include "scene_node.h"
#include "data/layer.h"
#include "renderers/layer_params.h"

int ModelNode::getSelectedColormap() {
		
	auto &colormaps = _scene.getColormaps();

	for (auto &[_, shaderBuffer] : getShaderBuffers()) {
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

void ModelNode::setSelectedColormap(int idx) {

	auto &colormaps = _scene.getColormaps();

	for (auto &[_, shaderBuffer] : getShaderBuffers()) {
		auto layerParams = shaderBuffer.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormaps[idx]);
	}
}