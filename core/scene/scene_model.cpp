#include "scene_model.h"

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
	auto &l = layers[Layer::COLORMAP_0];
	return l.getColormap();
}

void SceneModel::setColormap(Colormap colormap) {
	auto &l = layers[Layer::COLORMAP_0];
	return l.setColormap(colormap);
}

std::optional<Attribute> SceneModel::getSelectedAttribute() {
	return _selectedAttribute;
}

void SceneModel::setSelectedAttribute(std::optional<Attribute> attr) {
	// Unset previous colormaps layers for the whole model (colormap for all kind of elements)
	layers[Layer::COLORMAP_0].unset();

	
	if (attr.has_value())
		layers[Layer::COLORMAP_0].setAttribute(attr.value());
	
	_selectedAttribute = attr;
}

void SceneModel::updateLayers() {
	layers.update();
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

	// Apply selected attr if possible
	// If source has same attribute than target, select !
	auto sourceSelectedAttrOpt = model.getSelectedAttribute();
	if (sourceSelectedAttrOpt.has_value() && _mesh->hasAttribute(sourceSelectedAttrOpt.value())) {
		setSelectedAttribute(sourceSelectedAttrOpt);
		setColormap(model.getColormap().value());
	}
}
