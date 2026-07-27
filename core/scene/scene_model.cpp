#include "scene_model.h"

#include "layer_params.h"

bool SceneModel::addShaderPass(ShaderBase &shader) {
	auto name = shader.getName();
	
	if (_meshBuffer.contains(name))
		return false;
	
	auto meshBuffer = shader.createMeshBuffer();
	_meshBuffer.emplace(name, std::move(meshBuffer));
	
	auto material = shader.createMaterial();
	_materials.emplace(name, std::move(material));

	return true;
}

std::optional<Colormap> SceneModel::getColormap() {
		
	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		return layerParams->getColormap(ColormapLayer::COLORMAP_LAYER_0);
	}

	return std::nullopt;
}


void SceneModel::setColormap(Colormap colormap) {

	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");
		if (!layerParams)
			continue;
		
		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormap);
	}

}

std::string SceneModel::getLayerAttr(Layer layer, ElementKind kind) {
	std::tuple<Layer, ElementKind> k = {layer, kind};
	if (_attrNameByLayerAndKind.contains(k))
		return _attrNameByLayerAndKind[k];
	
	return defaultAttrName(layer);
}

std::string SceneModel::getSelectedAttribute() {
	return _selectedAttribute;
}

void SceneModel::setSelectedAttribute(const std::string attributeName) {
	setLayer(Layer::COLORMAP_0, attributeName, true);
	_selectedAttribute = attributeName;
}

void SceneModel::setLayer(Layer layer, const std::string attributeName, bool update) {

	auto &mesh = getMesh();
	auto attrOpt = mesh.getAttribute(attributeName);

	if (!attrOpt.has_value())
		return;

	auto attr = attrOpt.value();

	_attrNameByLayerAndKind[{layer, attr.getKind()}] = attr.name;

	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");

		if (layerParams)
			layerParams->setAttribute(attr, layer, update);
	}
}

void SceneModel::updateLayers() {
	auto &mesh = getMesh();

	for (int k = 0; k < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++k) {
		for (int l = 0; l < static_cast<int>(Layer::LAYER_COUNT); ++l) {

			auto layer = static_cast<Layer>(l);
			auto kind = static_cast<ElementKind>(k);

			auto attrOpt = mesh.getAttribute(getLayerAttr(layer, kind));

			if (!attrOpt.has_value())
				continue;

			auto attr = attrOpt.value();

			for (auto &[_, material] : getMaterials()) {
				auto layerParams = material.getParams<LayersParams>("layers");

				if (!layerParams)
					continue;

				auto activatedLayers = layerParams->getActivatedLayers();
				if (!activatedLayers[l][k])
					continue;

				layerParams->setAttribute(attr, layer, true);
			}

		}
	}

}

void SceneModel::unsetLayers(bool reset) {
	for (int l = 0; l < 5; ++l) {
		for (int k = 0; k < 7; ++k) {
			unsetLayer(static_cast<Layer>(l), static_cast<ElementKind>(k), reset);
		}
	}
}


void SceneModel::unsetLayer(Layer layer, ElementKind kind, bool reset) {
	
	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<LayersParams>("layers");

		if (!layerParams)
			continue;

		// Little optimisation, doesn't update data
		// if layer isn't activated, no need to unset
		if (!layerParams->isActivatedLayer(layer, kind))
			continue;
		
		// layerParams->setLayerElement(-1, layer);
		layerParams->deactivateLayer(layer, kind);
	}

}

void SceneModel::loadState(json &j, const std::string filename) {

}

void SceneModel::saveState(json &j, const std::string filename) {
	j["name"] = _name;
	j["visible"] = _visible;

	j["position"] = json::array({position.x, position.y, position.z});

	j["selected_attribute"] = _selectedAttribute;

	auto jAttrNameByLayer = json::object();
	for (auto &[k, attrName] : _attrNameByLayerAndKind) {
		auto [layer, kind] = k;
		std::string compositeKey = 
			std::to_string(static_cast<int>(layer)) + "_" + 
			std::to_string(static_cast<int>(kind));

		jAttrNameByLayer[compositeKey] = attrName;
	}

	j["attr_name_by_layer"] = jAttrNameByLayer;

	j["mesh"] = json::object();
	_mesh->saveState(j["mesh"], filename);

	auto jShaderPasses = json::array();
	for (auto &[materialName, _] : _materials) {
		jShaderPasses.push_back(materialName);
	}
	j["shader_passes"] = jShaderPasses;

	auto jMaterials = json::object();
	for (auto &[materialName, material] : _materials) {
		material.saveState(jMaterials[materialName]);
	}
	j["materials"] = jMaterials;

}

void SceneModel::applyMaterialsFrom(SceneModel &model) {
	for (auto &[materialName, sourceMaterial] : model.getMaterials()) {
		auto targetMaterialOpt = getMaterial(materialName);

		if (!targetMaterialOpt.has_value())
			continue;

		// Try to set source material to target material
		targetMaterialOpt.value().get().set(sourceMaterial);
	}

	// Set layers
	for (int k = 0; k < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++k) {
		for (int l = 0; l < static_cast<int>(Layer::LAYER_COUNT); ++l) {

			auto layer = static_cast<Layer>(l);
			auto kind = static_cast<ElementKind>(k);
			if (model._attrNameByLayerAndKind.contains({layer, kind})) {
				auto attrName = model._attrNameByLayerAndKind[{layer, kind}];
				if (!attrName.empty()) {
					setLayer(layer, attrName, true);
				}
			}
		}
	}
}
