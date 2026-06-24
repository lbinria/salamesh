#include "material.h"

void Material::saveState(json &j) {
	j["visible"] = _visible;

	auto jParams = json::object();
	for (auto &[paramsName, params] : _params) {
		params->saveState(jParams[paramsName]);
	}
	j["params"] = jParams;
}