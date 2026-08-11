#include "material.h"

void Material::saveState(json &j) {
	j["visible"] = _visible;

	auto jParams = json::object();
	for (auto &[paramsName, params] : _params) {
		params.saveState(jParams[paramsName]);
	}
	j["params"] = jParams;
}

void Material::set(Material &material) {
	for (auto &[paramName, params] : material.getParams()) {
		if (!_params.contains(paramName) || typeid(params) != typeid(_params.at(paramName)))
			continue;

		_params.at(paramName).setValues(params);
	}

	_visible = material._visible;
}

void Material::clean() {
	
}