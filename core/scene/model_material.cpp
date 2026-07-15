#include "model_material.h"

bool ModelMaterial::getLight() {

	for (auto &[_, material] : _node.getMaterials()) {
		auto lightParams = material.getParams("light");
		if (lightParams) {
			auto paramValue = lightParams->get("enabled");
			bool* enabled = std::get_if<bool>(&paramValue);
			if (!enabled || !*enabled)
				return false;
		}
	}

	return true;
}

void ModelMaterial::setLight(bool enabled) {
	for (auto &[_, material] : _node.getMaterials()) {
		auto lightParams = material.getParams("light");
		if (lightParams) {
			lightParams->set("enabled", enabled);
		}
	}
}