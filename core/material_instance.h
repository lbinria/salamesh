#pragma once
#include "shader_params.h"

struct MaterialInstance {


	bool getVisible() const { return visible; }


	void apply(Shader &shader) const {
		if (!visible)
			return;
		
		for (auto &[k, p] : params) { 
			p->update(shader);
		}
	}

	// TODO add saveState, loadState
	std::shared_ptr<ShaderParams> getParam(const std::string name) {
		return params.at(name);
	}

	private:
	bool visible = true;
	std::map<std::string, std::shared_ptr<ShaderParams>> params;

};