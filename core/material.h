#pragma once
#include "renderers/material_params.h"

#include <map>
#include <memory>

struct Material {

	Material() = default;
	Material(std::map<std::string, std::shared_ptr<MaterialParams>> &params) : 
		_params(std::move(params)) {}

	void apply(Shader &shader) {
		for (auto &[paramsName, params] : _params)
			params->apply(shader);
	}

	const std::map<std::string, std::shared_ptr<MaterialParams>> getParams() const {
		return _params;
	}

	std::shared_ptr<MaterialParams> getParams(const std::string name) {
		return _params.contains(name) ? _params.at(name) : nullptr;
	}

	template<typename TParams>
	std::shared_ptr<TParams> getParams(const std::string name) {
		return _params.contains(name) ? std::static_pointer_cast<TParams>(_params.at(name)) : nullptr;
	}

	bool isVisible() const {
		return _visible;
	}

	void setVisible(bool visible) {
		_visible = visible;
	}

	private:
	std::map<std::string, std::shared_ptr<MaterialParams>> _params;
	bool _visible = true;

};