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

// Note: it return the first colormap found (model can have heterogeneous colormaps between materials)
std::optional<Colormap> SceneModel::getColormap() {
		
	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<ColormapLayerParams>("colormap");
		if (!layerParams)
			continue;
		
		return layerParams->getColormap(ColormapLayer::COLORMAP_LAYER_0);
	}

	return std::nullopt;
}


void SceneModel::setColormap(Colormap colormap) {

	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<ColormapLayerParams>("colormap");
		if (!layerParams)
			continue;
		
		layerParams->setColormap(ColormapLayer::COLORMAP_LAYER_0, colormap);
	}

}

std::string SceneModel::getLayerAttr(Layer layer, ElementKind kind) {
	std::tuple<Layer, ElementKind> k = {layer, kind};
	if (_attrNameByLayerAndKind.contains(k))
		return _attrNameByLayerAndKind[k];
	

	// return std::format("{}.{}{}",
	// 	elementKindToString(kind),
	// 	defaultAttrName(layer),
	// 	getNDims() > 1 && dim >= 0 ? std::format("[{}]", dim) : "");

	// Below old version
	// return defaultAttrName(layer);

	// TODO maybe not necessary anymore
	return std::format("{}.{}",
		elementKindToString(kind),
		defaultAttrName(layer));
}

std::optional<Attribute> SceneModel::getSelectedAttribute() {
	return _selectedAttribute;
}

void SceneModel::setSelectedAttribute(std::optional<Attribute> attr) {
	// Unset previous colormap layer for the whole model
	unsetLayers(Layer::COLORMAP_0);

	if (attr.has_value())
		setLayer(Layer::COLORMAP_0, attr.value(), true);
	
	_selectedAttribute = attr;
}

void SceneModel::setLayer(Layer layer, Attribute attr, bool update) {

	_attrNameByLayerAndKind[{layer, attr.getKind()}] = attr.getFullName();

	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<MyLayerParams>(layerToString(layer)); 

		if (layerParams)
			layerParams->setAttribute(attr, true, update);
	}
}

void SceneModel::setLayerRange(Layer layer, Attribute attr, sl::algebra::vec2 range, bool update) {

	_attrNameByLayerAndKind[{layer, attr.getKind()}] = attr.getFullName();

	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<MyLayerParams>(layerToString(layer)); 

		if (layerParams) {
			layerParams->setAttribute(attr, true, update);
			layerParams->range = range;
		}
	}
}


// void SceneModel::updateLayers() {
// 	auto &mesh = getMesh();

// 	for (int k = 0; k < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++k) {
// 		for (int l = 0; l < static_cast<int>(Layer::LAYER_COUNT); ++l) {

// 			auto layer = static_cast<Layer>(l);
// 			auto kind = static_cast<ElementKind>(k);

// 			auto attrOpt = mesh.getAttribute(getLayerAttr(layer, kind));

// 			if (!attrOpt.has_value())
// 				continue;

// 			auto attr = attrOpt.value();

// 			for (auto &[_, material] : getMaterials()) {
// 				auto layerParams = material.getParams<MyLayerParams>(layerToString(layer));

// 				if (!layerParams)
// 					continue;

// 				auto activatedLayers = layerParams->getActivatedLayers();
// 				if (!activatedLayers[k])
// 					continue;

// 				layerParams->setAttribute(attr, true, true);
// 			}

// 		}
// 	}

// }

void SceneModel::updateLayers() {
	auto &mesh = getMesh();

	for (int l = 0; l < static_cast<int>(Layer::LAYER_COUNT); ++l) {
		auto layer = static_cast<Layer>(l);
		_layers[layer].update();
	}
}

void SceneModel::unsetLayers(bool reset) {
	for (int l = 0; l < 5; ++l) {
		for (int k = 0; k < 7; ++k) {
			unsetLayer(static_cast<Layer>(l), static_cast<ElementKind>(k), reset);
		}
	}
}

void SceneModel::unsetLayers(Layer layer, bool reset) {
	for (int k = 0; k < 7; ++k) {
		unsetLayer(layer, static_cast<ElementKind>(k), reset);
	}
}

void SceneModel::unsetLayers(ElementKind kind, bool reset) {
	for (int l = 0; l < 5; ++l) {
		unsetLayer(static_cast<Layer>(l), kind, reset);
	}
}

void SceneModel::unsetLayer(Layer layer, ElementKind kind, bool reset) {
	
	for (auto &[_, material] : getMaterials()) {
		auto layerParams = material.getParams<MyLayerParams>(layerToString(layer));

		if (!layerParams)
			continue;

		// Little optimisation, doesn't update data
		// if layer isn't activated, no need to unset
		if (!layerParams->isActivatedLayer(kind))
			continue;
		
		layerParams->deactivateLayer(kind);
	}

}

void SceneModel::loadState(json &j, const std::string filename) {

}

void SceneModel::saveState(json &j, const std::string filename) {
	j["name"] = _name;
	j["visible"] = _visible;

	j["position"] = json::array({position.x, position.y, position.z});

	// j["selected_attribute"] = _selectedAttribute;

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

	// // Set layers
	// for (int k = 0; k < static_cast<int>(ElementKind::ELEMENT_KIND_COUNT); ++k) {
	// 	for (int l = 0; l < static_cast<int>(Layer::LAYER_COUNT); ++l) {

	// 		auto layer = static_cast<Layer>(l);
	// 		auto kind = static_cast<ElementKind>(k);

	// 		if (!model._attrNameByLayerAndKind.contains({layer, kind}))
	// 			continue;

	// 		auto attrName = model._attrNameByLayerAndKind[{layer, kind}];

	// 		if (attrName.empty())
	// 			continue;
			

	// 	}
	// }

	// Apply selected attr if possible
	// If source has same attribute than target, select !
	auto sourceSelectedAttrOpt = model.getSelectedAttribute();
	if (sourceSelectedAttrOpt.has_value() && _mesh->hasAttribute(sourceSelectedAttrOpt.value())) {
		setSelectedAttribute(sourceSelectedAttrOpt);
		setColormap(model.getColormap().value());
	}
}
