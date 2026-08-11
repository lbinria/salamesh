#pragma once
#include "material_params.h"

#include <map>
#include <memory>

struct Material {


	
	Material(std::map<std::string, MaterialParams> params) : 
		_params(params) {}

	void apply(Shader &shader) {
		for (auto &[paramsName, params] : _params)
			params.apply(shader);
	}

	std::map<std::string, MaterialParams>& getParams() {
		return _params;
	}

	std::optional<std::reference_wrapper<MaterialParams>> getParams(const std::string name) {
		auto it = _params.find(name);
			if (it != _params.end()) {
				return std::ref(it->second);
			}
			return std::nullopt;
	}

	bool isVisible() const {
		return _visible;
	}

	void setVisible(bool visible) {
		_visible = visible;
	}

	void saveState(json &j);



	void set(Material &material);

	void clean();

	private:
	std::map<std::string, MaterialParams> _params;
	bool _visible = true;

};